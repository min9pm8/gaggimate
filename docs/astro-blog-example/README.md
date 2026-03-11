# Astro Blog Integration for GaggiMate Shot Logs

The GitLab Blog plugin pushes espresso shot data as markdown files to your GitLab-hosted Astro blog. This directory contains example components to render shot charts.

## Setup

### 1. Create the content directory

```
mkdir -p src/content/shots
```

### 2. Add the shots collection to your content config

Merge `shots-collection-config.ts` into your existing `src/content/config.ts`:

```ts
const shots = defineCollection({
  type: "content",
  schema: z.object({
    title: z.string(),
    description: z.string(),
    pubDate: z.coerce.date(),
    draft: z.boolean().default(false),
    tags: z.array(z.string()).default([]),
    profile: z.string(),
    duration: z.number(),
    weight: z.number().optional(),
    shotId: z.number(),
  }),
});

export const collections = { blog, shots };
```

### 3. Copy files to your blog

| File | Destination |
|------|-------------|
| `ShotChart.astro` | `src/components/ShotChart.astro` |
| `[...slug].astro` | `src/pages/shots/[...slug].astro` |

### 4. Configure the GaggiMate plugin

In the GaggiMate web UI, set:

- **Host**: `gitlab.com` (or your self-hosted instance)
- **Project ID**: your project path, e.g. `pministry/pministry.gitlab.io`
- **Token**: a GitLab Personal Access Token with `api` scope
- **Content Path**: `src/content/shots`

### 5. GitLab CI/CD

If your blog auto-deploys via GitLab Pages CI, each shot commit will trigger a rebuild and your new shot post will appear automatically.
