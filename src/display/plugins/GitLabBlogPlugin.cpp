#include "GitLabBlogPlugin.h"
#include "../core/Controller.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <display/models/shot_log_format.h>

namespace {
constexpr float WEIGHT_SCALE = 10.0f;

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

    // Mark shot as pending after extended recording completes
    pluginManager->on("controller:brew:clear", [this](Event const &) {
        if (!controller->getSettings().isGitLabBlogActive())
            return;
        if (brewStartTime == 0)
            return;
        unsigned long duration = millis() - brewStartTime;
        if (duration <= 7500) // Skip failed/short shots
            return;

        int shotIndex = controller->getSettings().getHistoryIndex() - 1;
        if (shotIndex < 0)
            return;

        if (pendingShotCount < MAX_PENDING_SHOTS) {
            pendingShots[pendingShotCount++] = shotIndex;
        }
        if (pendingShotCount == 1) {
            activityCount = 0; // Reset activities for new session
        }
        lastEventTime = millis();
        brewStartTime = 0;
        printf("GitLabBlog: Shot #%d queued (%d pending), waiting for activities...\n", shotIndex, pendingShotCount);
    });

    // Track steam/water process start
    pluginManager->on("controller:mode:change", [this](Event const &event) { activeMode = event.getInt("value"); });
    pluginManager->on("controller:process:start", [this](Event const &) {
        if (activeMode == 2 || activeMode == 3) { // MODE_STEAM or MODE_WATER
            processStartTime = millis();
            processStartTemp = lastTemperature;
        }
    });
    pluginManager->on("controller:process:end", [this](Event const &) {
        if (processStartTime == 0)
            return;
        if (activeMode != 2 && activeMode != 3)
            return;
        if (pendingShotCount == 0) {
            processStartTime = 0;
            return; // No shots pending, skip
        }
        unsigned long duration = millis() - processStartTime;
        if (duration < 3000) { // Skip accidental taps
            processStartTime = 0;
            return;
        }

        // Buffer the activity
        if (activityCount < MAX_ACTIVITIES) {
            activities[activityCount].type = (activeMode == 2) ? "Steam" : "Hot Water";
            activities[activityCount].startTemp = processStartTemp;
            activities[activityCount].endTemp = lastTemperature;
            activities[activityCount].durationMs = duration;
            activities[activityCount].timestamp = time(nullptr);
            activityCount++;
            lastEventTime = millis(); // Reset the publish timer
            printf("GitLabBlog: Buffered %s activity (%lu ms)\n",
                   activities[activityCount - 1].type.c_str(), duration);
        }
        processStartTime = 0;
    });

    // Manual publish trigger from web UI (publishes immediately with whatever is buffered)
    pluginManager->on("gitlab-blog:publish", [this](Event const &) {
        int shotIndex = controller->getSettings().getHistoryIndex() - 1;
        if (shotIndex >= 0) {
            if (pendingShotCount == 0) {
                pendingShots[pendingShotCount++] = shotIndex;
            }
            publishCombinedPost();
        }
    });

    pluginManager->on("controller:volumetric-measurement:bluetooth:change",
                      [this](Event const &event) { lastBluetoothWeight = event.getFloat("value"); });
    pluginManager->on("controller:volumetric-measurement:estimation:change",
                      [this](Event const &event) { lastEstimatedWeight = event.getFloat("value"); });
}

void GitLabBlogPlugin::loop() {
    if (pendingShotCount == 0)
        return;
    if (millis() - lastEventTime < PUBLISH_DELAY_MS)
        return;

    printf("GitLabBlog: Publishing %d shot(s) with %d activities\n", pendingShotCount, activityCount);
    publishCombinedPost();
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

void GitLabBlogPlugin::publishCombinedPost() {
    const Settings &settings = controller->getSettings();
    const String host = settings.getGitLabBlogHost();
    const String projectId = settings.getGitLabBlogProjectId();
    const String token = settings.getGitLabBlogToken();
    const String basePath = settings.getGitLabBlogPath();

    if (projectId.isEmpty() || token.isEmpty()) {
        printf("GitLabBlog: Missing project ID or token\n");
        pendingShotCount = 0;
        return;
    }

    // Snapshot and clear pending state
    int shotCount = pendingShotCount;
    int shots[MAX_PENDING_SHOTS];
    for (int i = 0; i < shotCount; i++)
        shots[i] = pendingShots[i];
    pendingShotCount = 0;

    if (shotCount == 0)
        return;

    FS *fs = controller->isSDCard() ? (FS *)&SD_MMC : (FS *)&SPIFFS;

    // Read first shot header for frontmatter timestamp and profile
    String firstPaddedId = String(shots[0]);
    while (firstPaddedId.length() < 6)
        firstPaddedId = "0" + firstPaddedId;

    String firstSlogPath = "/h/" + firstPaddedId + ".slog";
    File firstFile = fs->open(firstSlogPath, "r");
    if (!firstFile) {
        printf("GitLabBlog: Cannot open %s\n", firstSlogPath.c_str());
        return;
    }

    ShotLogHeader firstHdr{};
    if (firstFile.read(reinterpret_cast<uint8_t *>(&firstHdr), sizeof(firstHdr)) != sizeof(firstHdr) ||
        firstHdr.magic != SHOT_LOG_MAGIC) {
        firstFile.close();
        printf("GitLabBlog: Invalid shot file\n");
        return;
    }
    firstFile.close();

    // Format timestamp from first shot — fall back to current time if NTP wasn't synced
    time_t ts = firstHdr.startEpoch;
    if (ts < 1600000000) { // Before 2020-09-13 = NTP wasn't synced when shot was recorded
        ts = time(nullptr);
    }
    struct tm *tmInfo = localtime(&ts);
    char dateBuf[32];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%dT%H:%M:%S", tmInfo);
    char datePart[16];
    strftime(datePart, sizeof(datePart), "%Y-%m-%d", tmInfo);

    // Build title: "Shot #1, #2 - ProfileName"
    String title = "Shot";
    for (int i = 0; i < shotCount; i++) {
        if (i > 0)
            title += ",";
        title += " #" + String(shots[i]);
    }
    title += " - " + String(firstHdr.profileName);

    // Build markdown content
    String content;
    content.reserve(4096);

    // Astro-compatible frontmatter
    content += "---\n";
    content += "title: \"" + title + "\"\n";
    content += "pubDate: " + String(dateBuf) + "\n";
    content += "draft: false\n";
    content += "tags: [\"espresso\", \"shot-log\"]\n";
    content += "profile: \"" + String(firstHdr.profileName) + "\"\n";
    if (shotCount == 1) {
        content += "shotId: " + String(shots[0]) + "\n";
    } else {
        content += "shotIds: [";
        for (int i = 0; i < shotCount; i++) {
            if (i > 0)
                content += ", ";
            content += String(shots[i]);
        }
        content += "]\n";
    }
    content += "---\n\n";

    // Per-shot sections
    for (int s = 0; s < shotCount; s++) {
        String paddedId = String(shots[s]);
        while (paddedId.length() < 6)
            paddedId = "0" + paddedId;

        String slogPath = "/h/" + paddedId + ".slog";
        File shotFile = fs->open(slogPath, "r");
        if (!shotFile) {
            printf("GitLabBlog: Cannot open %s\n", slogPath.c_str());
            continue;
        }

        ShotLogHeader hdr{};
        if (shotFile.read(reinterpret_cast<uint8_t *>(&hdr), sizeof(hdr)) != sizeof(hdr) ||
            hdr.magic != SHOT_LOG_MAGIC) {
            shotFile.close();
            printf("GitLabBlog: Invalid shot file for #%d\n", shots[s]);
            continue;
        }

        float finalWeight = hdr.finalWeight > 0 ? (float)hdr.finalWeight / WEIGHT_SCALE : 0.0f;
        float durationSec = (float)hdr.durationMs / 1000.0f;

        // Shot heading
        content += "## Shot #" + String(shots[s]) + "\n\n";

        // Summary table
        content += "| Metric | Value |\n";
        content += "|--------|-------|\n";
        content += "| Profile | " + String(hdr.profileName) + " |\n";
        content += "| Duration | " + String(durationSec, 1) + "s |\n";
        if (finalWeight > 0.0f)
            content += "| Output | " + String(finalWeight, 1) + "g |\n";
        content += "\n";

        // Phase transitions
        if (hdr.phaseTransitionCount > 0) {
            content += "### Phases\n\n";
            for (uint8_t i = 0; i < hdr.phaseTransitionCount && i < 12; i++) {
                float phaseTime = (float)(hdr.phaseTransitions[i].sampleIndex * SHOT_LOG_SAMPLE_INTERVAL_MS) / 1000.0f;
                content += "- **" + String(hdr.phaseTransitions[i].phaseName) + "** at " + String(phaseTime, 1) + "s\n";
            }
            content += "\n";
        }

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
                        content += "### Notes\n\n";
                        content += notesDoc["notes"].as<String>() + "\n\n";
                    }
                }
            }
        }
    }

    // Steam/water activities (shared across all shots in the session)
    if (activityCount > 0) {
        content += "## Activities\n\n";
        content += "| Type | Duration | Start Temp | End Temp |\n";
        content += "|------|----------|------------|----------|\n";
        for (int i = 0; i < activityCount; i++) {
            float actDuration = (float)activities[i].durationMs / 1000.0f;
            content += "| " + activities[i].type;
            content += " | " + String(actDuration, 1) + "s";
            content += " | " + String(activities[i].startTemp, 1) + " C";
            content += " | " + String(activities[i].endTemp, 1) + " C |\n";
        }
        content += "\n";
    }

    // POST to GitLab API
    String fileName = String(datePart) + "-shot-" + String(shots[0]);
    if (shotCount > 1)
        fileName += "-" + String(shots[shotCount - 1]);
    fileName += ".md";
    String filePath = basePath + "/" + fileName;
    String encodedPath = urlEncode(filePath);
    String encodedProjectId = urlEncode(projectId);

    String url = "https://" + host + "/api/v4/projects/" + encodedProjectId + "/repository/files/" + encodedPath;

    // Build JSON payload for GitLab API
    JsonDocument payload;
    payload["branch"] = "main";
    payload["content"] = content;

    String commitMsg = "Add " + title;
    payload["commit_message"] = commitMsg;

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
        lastStatus.message = title + " published";
        printf("GitLabBlog: %s published with %d activities\n", title.c_str(), activityCount);
    } else {
        String response = http.getString();
        lastStatus.message = title + " failed: HTTP " + String(responseCode);
        printf("GitLabBlog: Failed to publish, HTTP %d\n", responseCode);
        printf("GitLabBlog: %s\n", response.c_str());
    }
    http.end();

    activityCount = 0; // Clear buffered activities

    // Broadcast status to WebSocket clients
    if (pluginManager) {
        pluginManager->trigger("gitlab-blog:publish-status", "code", lastStatus.httpCode);
    }
}
