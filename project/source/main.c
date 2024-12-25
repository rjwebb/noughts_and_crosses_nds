#include <crab.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>

// glImage Crab[1];

int main(void) {
  // put the main screen on the bottom lcd
  // lcdMainOnBottom();

  // Sprite initialisation
  videoSetMode(MODE_0_2D);

  consoleDemoInit();

  int keys = 0;

  oamInit(&oamMain, SpriteMapping_1D_64, false);
  vramSetBankA(VRAM_A_MAIN_SPRITE);

  u16 *gfx =
      oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);

  // copy in our ball graphics
  dmaCopy(crabBitmap, gfx, crabBitmapLen);
  dmaCopy(crabPal, SPRITE_PALETTE, crabPalLen);

  while (pmMainLoop()) {

    scanKeys();
    keys = keysHeld();

    oamSet(&oamMain,         // sub display
           0,                // oam entry to set
           0, 0,             // position
           0,                // priority
           0,                // palette for 16 color sprite or alpha for
                             // bmp sprite
           SpriteSize_64x64, // sprite size
           SpriteColorFormat_256Color, // sprite color fornat
           gfx,                        // pointer to the loaded graphics
           0,                          // affine rotation to use
           false,                      // double the size of rotated sprites
           false,                      // don't hide the sprite
           false, false,               // vflip, hflip
           false                       // apply mosaic
    );

    swiWaitForVBlank();

    if (keys & KEY_B) {
      break;
    }

    oamUpdate(&oamMain);
  }

  return 0;
}
