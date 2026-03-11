#include "GitLabBlogPlugin.h"
#include "../core/Controller.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <display/models/shot_log_format.h>

namespace {
constexpr float TEMP_SCALE = 10.0f;
constexpr float PRESSURE_SCALE = 10.0f;
constexpr float FLOW_SCALE = 100.0f;
constexpr float WEIGHT_SCALE = 10.0f;
constexpr float RESISTANCE_SCALE = 100.0f;
constexpr size_t MAX_SAMPLES_FOR_BLOG = 600; // ~2.5 min at 250ms intervals

String urlEncode(const String &str) {
    String encoded;
    encoded.reserve(str.length() * 3);
    for (size_t i = 0; i < str.length(); i++) {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else {
            char buf[4];
            snprintf(buf, sizeof(buf), "%%%02X", (unsigned char)c);
            encoded += buf;
        }
    }
    return encoded;
}
} // namespace

void GitLabBlogPlugin::setup(Controller *c, PluginManager *pm) {
    controller = c;
    pluginManager = pm;

    pluginManager->on("boiler:currentTemperature:change",
                      [this](Event const &event) { lastTemperature = event.getFloat("value"); });
    pluginManager->on("controller:brew:start", [this](Event const &) { brewStartTime = millis(); });
    pluginManager->on("controller:brew:end", [this](Event const &) {
        lastPressure = controller->getCurrentPressure();
        lastPumpFlow = controller->getCurrentPumpFlow();
        lastPuckFlow = controller->getCurrentPuckFlow();
    });

    // Publish after extended recording completes (shot fully finalized)
    pluginManager->on("controller:brew:clear", [this](Event const &) {
        if (!controller->getSettings().isGitLabBlogActive())
            return;
        if (brewStartTime == 0)
            return;
        unsigned long duration = millis() - brewStartTime;
        if (duration <= 7500) // Skip failed/short shots, same threshold as ShotHistoryPlugin
            return;

        publishShot(controller);
        brewStartTime = 0;
    });

    pluginManager->on("controller:volumetric-measurement:bluetooth:change",
                      [this](Event const &event) { lastBluetoothWeight = event.getFloat("value"); });
    pluginManager->on("controller:volumetric-measurement:estimation:change",
                      [this](Event const &event) { lastEstimatedWeight = event.getFloat("value"); });
}

GitLabBlogStatus GitLabBlogPlugin::testConnection() {
    GitLabBlogStatus status;
    const Settings &settings = controller->getSettings();
    const String host = settings.getGitLabBlogHost();
    const String projectId = settings.getGitLabBlogProjectId();
    const String token = settings.getGitLabBlogToken();

    if (projectId.isEmpty() || token.isEmpty()) {
        status.httpCode = 0;
        status.message = "Missing project ID or token";
        status.timestamp = millis();
        return status;
    }

    String encodedProjectId = urlEncode(projectId);
    String url = "https://" + host + "/api/v4/projects/" + encodedProjectId;

    HTTPClient http;
    http.begin(url);
    http.addHeader("PRIVATE-TOKEN", token);
    http.setTimeout(10000);

    int responseCode = http.GET();
    status.httpCode = responseCode;
    status.timestamp = millis();

    if (responseCode == 200) {
        String response = http.getString();
        JsonDocument doc;
        if (deserializeJson(doc, response) == DeserializationError::Ok) {
            String name = doc["path_with_namespace"] | "unknown";
            status.message = "Connected to " + name;
        } else {
            status.message = "Connected successfully";
        }
    } else if (responseCode == 401) {
        status.message = "Authentication failed - check your token";
    } else if (responseCode == 404) {
        status.message = "Project not found - check project ID/path";
    } else if (responseCode < 0) {
        status.message = "Connection failed - check host and network";
    } else {
        status.message = "HTTP " + String(responseCode);
    }

    http.end();
    return status;
}

void GitLabBlogPlugin::publishShot(Controller *controller) {
    const Settings &settings = controller->getSettings();
    const String host = settings.getGitLabBlogHost();
    const String projectId = settings.getGitLabBlogProjectId();
    const String token = settings.getGitLabBlogToken();
    const String basePath = settings.getGitLabBlogPath();

    if (projectId.isEmpty() || token.isEmpty()) {
        printf("GitLabBlog: Missing project ID or token\n");
        return;
    }

    // Get the most recent shot ID
    int shotIndex = settings.getHistoryIndex() - 1;
    if (shotIndex < 0)
        return;

    String paddedId = String(shotIndex);
    while (paddedId.length() < 6)
        paddedId = "0" + paddedId;

    // Read the shot header from .slog file
    FS *fs = controller->isSDCard() ? (FS *)&SD_MMC : (FS *)&SPIFFS;
    String slogPath = "/h/" + paddedId + ".slog";
    File shotFile = fs->open(slogPath, "r");
    if (!shotFile) {
        printf("GitLabBlog: Cannot open %s\n", slogPath.c_str());
        return;
    }

    ShotLogHeader hdr{};
    if (shotFile.read(reinterpret_cast<uint8_t *>(&hdr), sizeof(hdr)) != sizeof(hdr) || hdr.magic != SHOT_LOG_MAGIC) {
        shotFile.close();
        printf("GitLabBlog: Invalid shot file\n");
        return;
    }

    // Read samples for chart data
    uint32_t samplesToRead = hdr.sampleCount;
    if (samplesToRead > MAX_SAMPLES_FOR_BLOG)
        samplesToRead = MAX_SAMPLES_FOR_BLOG;

    // Build the markdown content
    String content;
    content.reserve(4096);

    // Format timestamp for Astro frontmatter
    time_t ts = hdr.startEpoch;
    struct tm *tmInfo = gmtime(&ts);
    char dateBuf[32];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%dT%H:%M:%SZ", tmInfo);
    char datePart[16];
    strftime(datePart, sizeof(datePart), "%Y-%m-%d", tmInfo);

    float finalWeight = hdr.finalWeight > 0 ? (float)hdr.finalWeight / WEIGHT_SCALE : 0.0f;
    float durationSec = (float)hdr.durationMs / 1000.0f;

    // Astro-compatible frontmatter
    content += "---\n";
    content += "title: \"Shot #" + String(shotIndex) + " - " + String(hdr.profileName) + "\"\n";
    content += "date: " + String(dateBuf) + "\n";
    content += "profile: \"" + String(hdr.profileName) + "\"\n";
    content += "duration: " + String(durationSec, 1) + "\n";
    if (finalWeight > 0.0f)
        content += "weight: " + String(finalWeight, 1) + "\n";
    content += "shotId: " + String(shotIndex) + "\n";
    content += "---\n\n";

    // Summary section
    content += "## Shot Summary\n\n";
    content += "| Metric | Value |\n";
    content += "|--------|-------|\n";
    content += "| Profile | " + String(hdr.profileName) + " |\n";
    content += "| Duration | " + String(durationSec, 1) + "s |\n";
    if (finalWeight > 0.0f)
        content += "| Output | " + String(finalWeight, 1) + "g |\n";
    content += "\n";

    // Phase transitions
    if (hdr.phaseTransitionCount > 0) {
        content += "## Phases\n\n";
        for (uint8_t i = 0; i < hdr.phaseTransitionCount && i < 12; i++) {
            float phaseTime = (float)(hdr.phaseTransitions[i].sampleIndex * SHOT_LOG_SAMPLE_INTERVAL_MS) / 1000.0f;
            content += "- **" + String(hdr.phaseTransitions[i].phaseName) + "** at " + String(phaseTime, 1) + "s\n";
        }
        content += "\n";
    }

    // Chart data as JSON in a code block for Astro components to consume
    content += "## Shot Data\n\n";
    content += "```json\n";
    content += "[";

    for (uint32_t i = 0; i < samplesToRead; i++) {
        ShotLogSample sample{};
        if (shotFile.read(reinterpret_cast<uint8_t *>(&sample), sizeof(sample)) != sizeof(sample))
            break;

        if (i > 0)
            content += ",";
        content += "\n{";
        content += "\"t\":" + String((float)sample.t * SHOT_LOG_SAMPLE_INTERVAL_MS / 1000.0f, 2);
        content += ",\"ct\":" + String((float)sample.ct / TEMP_SCALE, 1);
        content += ",\"tt\":" + String((float)sample.tt / TEMP_SCALE, 1);
        content += ",\"cp\":" + String((float)sample.cp / PRESSURE_SCALE, 1);
        content += ",\"tp\":" + String((float)sample.tp / PRESSURE_SCALE, 1);
        content += ",\"fl\":" + String((float)sample.fl / FLOW_SCALE, 2);
        content += ",\"pf\":" + String((float)sample.pf / FLOW_SCALE, 2);
        if (sample.v > 0)
            content += ",\"v\":" + String((float)sample.v / WEIGHT_SCALE, 1);
        content += "}";
    }

    content += "\n]\n```\n";
    shotFile.close();

    // Load notes if available
    String notesPath = "/h/" + paddedId + ".json";
    if (fs->exists(notesPath)) {
        File notesFile = fs->open(notesPath, "r");
        if (notesFile) {
            String notesStr = notesFile.readString();
            notesFile.close();

            JsonDocument notesDoc;
            if (deserializeJson(notesDoc, notesStr) == DeserializationError::Ok) {
                if (notesDoc["notes"].is<String>() && notesDoc["notes"].as<String>().length() > 0) {
                    content += "\n## Notes\n\n";
                    content += notesDoc["notes"].as<String>() + "\n";
                }
            }
        }
    }

    // POST to GitLab API
    String filePath = basePath + "/" + String(datePart) + "-shot-" + String(shotIndex) + ".md";
    String encodedPath = urlEncode(filePath);
    String encodedProjectId = urlEncode(projectId);

    String url = "https://" + host + "/api/v4/projects/" + encodedProjectId + "/repository/files/" + encodedPath;

    // Build JSON payload for GitLab API
    JsonDocument payload;
    payload["branch"] = "main";
    payload["content"] = content;
    payload["commit_message"] = "Add shot #" + String(shotIndex) + " - " + String(hdr.profileName);

    String payloadStr;
    serializeJson(payload, payloadStr);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("PRIVATE-TOKEN", token);
    http.setTimeout(15000);

    int responseCode = http.POST(payloadStr);
    lastStatus.httpCode = responseCode;
    lastStatus.timestamp = millis();
    if (responseCode == 201) {
        lastStatus.message = "Shot #" + String(shotIndex) + " published";
        printf("GitLabBlog: Shot #%d published successfully\n", shotIndex);
    } else {
        String response = http.getString();
        lastStatus.message = "Shot #" + String(shotIndex) + " failed: HTTP " + String(responseCode);
        printf("GitLabBlog: Failed to publish shot #%d, HTTP %d\n", shotIndex, responseCode);
        printf("GitLabBlog: %s\n", response.c_str());
    }
    http.end();

    // Broadcast status to WebSocket clients
    if (pluginManager) {
        pluginManager->trigger("gitlab-blog:publish-status", "code", lastStatus.httpCode);
    }
}
