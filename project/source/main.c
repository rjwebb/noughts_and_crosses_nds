#include <combined32.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>

enum CellState { S_EMPTY = 0, S_GOOSE = 1, S_CRAB = 2 };

int main(void) {
  // put the main screen on the bottom lcd
  lcdMainOnBottom();

  // Sprite initialisation
  videoSetMode(MODE_0_2D);

  consoleDemoInit();

  int keys = 0;

  int spriteTilesLen = 32 * 32;

  oamInit(&oamMain, SpriteMapping_1D_128, false);
  vramSetBankA(VRAM_A_MAIN_SPRITE);

  // copy in the palette - this is shared by all of the sprites
  dmaCopy(combined32Pal, SPRITE_PALETTE, combined32PalLen);

  // empty sprite
  u16 *emptyGfx =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  u8 *emptyTiles = (u8 *)combined32Tiles;
  dmaCopy(emptyTiles, emptyGfx, spriteTilesLen);

  // goose sprite
  u16 *gooseGfx =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  u8 *gooseTiles = (u8 *)combined32Tiles + spriteTilesLen;
  dmaCopy(gooseTiles, gooseGfx, spriteTilesLen);

  // crab sprite
  u16 *crabGfx =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  u8 *crabTiles = (u8 *)combined32Tiles + (spriteTilesLen * 2);
  dmaCopy(crabTiles, crabGfx, spriteTilesLen);

  int cells[3][3] = {{S_EMPTY, S_EMPTY, S_EMPTY},
                     {S_EMPTY, S_EMPTY, S_EMPTY},
                     {S_EMPTY, S_EMPTY, S_EMPTY}};

  int gridXStart = 80;
  int gridYStart = 48;

  while (pmMainLoop()) {

    scanKeys();
    keys = keysHeld();

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int cell = cells[i][j];
        u16 *gfx = emptyGfx;
        if (cell == S_GOOSE) {
          gfx = gooseGfx;
        } else if (cell == S_CRAB) {
          gfx = crabGfx;
        }

        oamSet(&oamMain,                                 // sub display
               i * 3 + j,                                // oam entry to set
               gridXStart + j * 32, gridYStart + i * 32, // position
               0,                                        // priority
               0,                // palette for 16 color sprite or alpha for
                                 // bmp sprite
               SpriteSize_32x32, // sprite size
               SpriteColorFormat_256Color, // sprite color fornat
               gfx,                        // pointer to the loaded graphics
               0,                          // affine rotation to use
               false,                      // double the size of rotated sprites
               false,                      // don't hide the sprite
               false, false,               // vflip, hflip
               false                       // apply mosaic
        );
      }
    }

    swiWaitForVBlank();

    if (keys & KEY_B) {
      break;
    }

    oamUpdate(&oamMain);
  }

  return 0;
}
