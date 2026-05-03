#include <tamtypes.h>
#include <kernel.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>
#include <pad.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static GSGLOBAL *gsGlobal = NULL;
static GSFONT *gsFont = NULL;

typedef struct {
    float x, y, w, h;
} Player;

static Player player = {100.0f, 100.0f, 40.0f, 40.0f};
static float speed = 200.0f;

static void initGraphics(void) {
    gsGlobal = gsKit_init_global();
    gsGlobal->Mode = GS_MODE_PAL;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->ZBuffering = GS_SETTING_OFF;
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
    
    dmaKit_init(DMAKit_INIT_DEFAULT);
    gsKit_init_screen(gsGlobal);
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);
    
    gsFont = gsKit_init_font(gsGlobal, NULL);
    gsKit_font_upload(gsGlobal, gsFont);
}

static void drawPlayer(void) {
    u64 color = GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x80, 0x00);
    
    gsKit_prim_quad(gsGlobal,
        player.x, player.y,
        player.x + player.w, player.y,
        player.x, player.y + player.h,
        player.x + player.w, player.y + player.h,
        0, color);
}

static void handleInput(float dt) {
    struct padButtonStatus buttons;
    int ret = padRead(0, 0, &buttons);
    
    if (ret != 0) {
        if (buttons.btns & PAD_LEFT)  player.x -= speed * dt;
        if (buttons.btns & PAD_RIGHT) player.x += speed * dt;
        if (buttons.btns & PAD_UP)    player.y -= speed * dt;
        if (buttons.btns & PAD_DOWN)  player.y += speed * dt;
    }
    
    // Boundary checking
    if (player.x < 0) player.x = 0;
    if (player.y < 0) player.y = 0;
    if (player.x + player.w > SCREEN_WIDTH)  player.x = SCREEN_WIDTH - player.w;
    if (player.y + player.h > SCREEN_HEIGHT) player.y = SCREEN_HEIGHT - player.h;
}

int main(void) {
    initGraphics();
    
    padInit(0);
    padPortOpen(0, 0, "Pad");
    padPortOpen(1, 0, "Pad");
    
    u64 bgColor = GS_SETREG_RGBAQ(0x12, 0x12, 0x12, 0x80, 0x00);
    
    float lastTime = 0.0f;
    
    while (1) {
        float currentTime = (float)GetTimerSystemTime() / 1000000.0f;
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        
        handleInput(dt);
        
        gsKit_clear(gsGlobal, bgColor);
        drawPlayer();
        gsKit_sync_flip(gsGlobal);
    }
    
    return 0;
}
