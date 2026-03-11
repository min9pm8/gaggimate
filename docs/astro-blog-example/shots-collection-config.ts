/**
 * Add this to your existing src/content/config.ts (or .mjs) file.
 *
 * If you already have a 'blog' collection, just add the 'shots' collection
 * alongside it in defineCollection.
 */
import { defineCollection, z } from "astro:content";

const shots = defineCollection({
  type: "content",
  schema: z.object({
    title: z.string(),
    description: z.string(),
    pubDate: z.coerce.date(),
    draft: z.boolean().default(false),
    tags: z.array(z.string()).default([]),
    // GaggiMate shot-specific fields
    profile: z.string(),
    duration: z.number(),
    weight: z.number().optional(),
    shotId: z.number(),
  }),
});

// Merge with your existing collections:
// export const collections = { blog, shots };
