import { defineConfig } from 'vite';
import preact from '@preact/preset-vite';
import tailwindcss from '@tailwindcss/vite';

// https://vitejs.dev/config/
export default defineConfig({
  define: {
    __SPIFFS_BUILD_TIME__: JSON.stringify(new Date().toISOString()),
  },
  plugins: [preact(), tailwindcss()],

  server: {
    proxy: {
      '/api': {
        target: 'http://gaggimate.local/',
        changeOrigin: true,
      },
      '/ws': {
        target: 'ws://gaggimate.local',
        ws: true,
      },
    },
    watch: {
      usePolling: true,
    },
  },
});
