#include <math.h>
#include <stdio.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

// Game state
static int canvasWidth = 800;
static int canvasHeight = 450;
static float playerX = 100.0f;
static float playerY = 100.0f;
static float playerW = 40.0f;
static float playerH = 40.0f;
static float speed = 200.0f; // pixels per second

static int keyLeft = 0;
static int keyRight = 0;
static int keyUp = 0;
static int keyDown = 0;

// JS helpers: create a canvas and draw a simple rectangle scene
EM_JS(void, js_init_canvas, (int w, int h), {
  if (!document.getElementById('canvas')) {
    const c = document.createElement('canvas');
    c.id = 'canvas';
    document.body.style.margin = '0';
    document.body.style.background = '#121212';
    c.width = w;
    c.height = h;
    c.style.width = '100vw';
    c.style.height = 'calc(100vh)';
    c.style.display = 'block';
    document.body.appendChild(c);
  }
});

EM_JS(void, js_draw_scene, (int w, int h, int px, int py, int pw, int ph), {
  const c = document.getElementById('canvas');
  const ctx = c.getContext('2d');
  if (c.width !== w || c.height !== h) {
    c.width = w;
    c.height = h;
  }
  ctx.fillStyle = '#121212';
  ctx.fillRect(0, 0, c.width, c.height);
  ctx.fillStyle = '#f0f0f0';
  ctx.fillRect(px, py, pw, ph);
});

static EM_BOOL on_key(int type, const EmscriptenKeyboardEvent* e, void* userData) {
  (void)userData;
  // Only care about arrows
  int down = (type == EMSCRIPTEN_EVENT_KEYDOWN) ? 1 : 0;
  if (strcmp(e->key, "ArrowLeft") == 0)  { keyLeft = down;  return EM_TRUE; }
  if (strcmp(e->key, "ArrowRight") == 0) { keyRight = down; return EM_TRUE; }
  if (strcmp(e->key, "ArrowUp") == 0)    { keyUp = down;    return EM_TRUE; }
  if (strcmp(e->key, "ArrowDown") == 0)  { keyDown = down;  return EM_TRUE; }
  return EM_FALSE;
}

static double lastMs = 0.0;

static void tick(void) {
  double now = emscripten_get_now();
  double dt = (lastMs > 0.0) ? (now - lastMs) / 1000.0 : 0.016; // ~60 FPS first frame
  lastMs = now;

  if (keyLeft)  playerX -= (float)(speed * dt);
  if (keyRight) playerX += (float)(speed * dt);
  if (keyUp)    playerY -= (float)(speed * dt);
  if (keyDown)  playerY += (float)(speed * dt);

  if (playerX < 0) playerX = 0;
  if (playerY < 0) playerY = 0;
  if (playerX + playerW > canvasWidth)  playerX = (float)(canvasWidth - playerW);
  if (playerY + playerH > canvasHeight) playerY = (float)(canvasHeight - playerH);

  js_draw_scene(canvasWidth, canvasHeight, (int)playerX, (int)playerY, (int)playerW, (int)playerH);
}

int main(void) {
  js_init_canvas(canvasWidth, canvasHeight);
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, on_key);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, on_key);
  emscripten_set_main_loop(tick, 0, 1);
  return 0;
}



