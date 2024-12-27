#include <combined32.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>

// Enum to represent the state of a cell
enum CellState { S_EMPTY = 0, S_GOOSE = 1, S_CRAB = 2 };

void loadSprites(u16 *sprite_gfx_mem[], u8 *tiles, int numTiles, int tileSize) {
  for (int i = 0; i < numTiles; i++) {
    sprite_gfx_mem[i] =
        oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    dmaCopy(tiles + tileSize * i, sprite_gfx_mem[i], tileSize);
  }
}

void getButtonXY(int x, int y, int *buttonX, int *buttonY) {
  if (x < 80 || x > 240 || y < 48 || y > 208) {
    *buttonX = -1;
    *buttonY = -1;
    return;
  }

  *buttonX = (x - 80) / 32;
  *buttonY = (y - 48) / 32;
}

int main(void) {

  touchPosition touchXY;

  // put the main screen on the bottom lcd
  lcdMainOnBottom();

  // Sprite initialisation
  videoSetMode(MODE_0_2D);

  consoleDemoInit();

  int spriteTilesLen = 32 * 32;

  oamInit(&oamMain, SpriteMapping_1D_128, false);
  vramSetBankA(VRAM_A_MAIN_SPRITE);

  // copy in the palette - this is shared by all of the sprites
  dmaCopy(combined32Pal, SPRITE_PALETTE, combined32PalLen);

  u16 *sprite_gfx_mem[12];
  loadSprites(sprite_gfx_mem, (u8 *)combined32Tiles, 6, spriteTilesLen);

  u16 *emptyGfx = sprite_gfx_mem[0];
  u16 *emptyClickedGfx = sprite_gfx_mem[1];
  u16 *gooseGfx = sprite_gfx_mem[2];
  u16 *gooseClickedGfx = sprite_gfx_mem[3];
  u16 *crabGfx = sprite_gfx_mem[4];
  u16 *crabClickedGfx = sprite_gfx_mem[5];

  int cells[3][3] = {{S_EMPTY, S_EMPTY, S_EMPTY},
                     {S_EMPTY, S_EMPTY, S_EMPTY},
                     {S_EMPTY, S_EMPTY, S_EMPTY}};

  int gridXStart = 80;
  int gridYStart = 48;

  int clickedX = -1;
  int clickedY = -1;
  int alreadyClicked = 0;

  int isGoose = 1;

  int angle = 0;
  int keys = 0;
  while (pmMainLoop()) {
    scanKeys();
    keys = keysHeld();
    touchRead(&touchXY);

    // check for a winner
    int winner = 0;
    // check rows and columns
    for (int i = 0; i < 3; i++) {
      if ((cells[i][0] != S_EMPTY && cells[i][0] == cells[i][1] &&
           cells[i][0] == cells[i][2]) ||
          (cells[0][i] != S_EMPTY && cells[1][i] == cells[0][i] &&
           cells[0][i] == cells[2][i])) {
        winner = cells[i][i];
      }
    }
    // check diagonals
    if ((cells[0][0] != S_EMPTY && cells[0][0] == cells[1][1] &&
         cells[0][0] == cells[2][2]) ||
        (cells[0][2] != S_EMPTY && cells[0][2] == cells[1][1] &&
         cells[0][2] == cells[2][0])) {
      winner = cells[1][1];
    }

    if (winner != 0) {
      angle = (angle - 512) % 65534;
    }

    if (keys & KEY_TOUCH) {
      alreadyClicked = 1;
    } else {
      if (winner == 0 && alreadyClicked) {
        alreadyClicked = 0;
        if (clickedX != -1 && clickedY != -1) {
          if (cells[clickedY][clickedX] == S_EMPTY) {
            if (isGoose) {
              cells[clickedY][clickedX] = S_GOOSE;
              isGoose = 0;
            } else {
              cells[clickedY][clickedX] = S_CRAB;
              isGoose = 1;
            }
          }
        }
      }
    }

    getButtonXY(touchXY.px, touchXY.py, &clickedX, &clickedY);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int isClicked = clickedX == j && clickedY == i;
        int cell = cells[i][j];

        u16 *gfx = emptyGfx;
        int rotIndex = 0;
        if (cell == S_GOOSE) {
          if (winner == 0 && isClicked) {
            gfx = gooseClickedGfx;
          } else {
            if (winner == S_GOOSE) {
              rotIndex = 1;
            }
            gfx = gooseGfx;
          }
        } else if (cell == S_CRAB) {
          if (winner == 0 && isClicked) {
            gfx = crabClickedGfx;
          } else {
            if (winner == S_CRAB) {
              rotIndex = 1;
            }
            gfx = crabGfx;
          }
        } else {
          if (winner == 0 && isClicked) {
            gfx = emptyClickedGfx;
          }
        }

        oamSet(&oamMain,  // sub display
               i * 3 + j, // oam entry to set
               //  offset position because we are rotating the sprites, so their
               //  center is offset
               gridXStart + j * 32 - 16, gridYStart + i * 32 - 16, // position
               0,                                                  // priority
               0,                // palette for 16 color sprite or alpha for
                                 // bmp sprite
               SpriteSize_32x32, // sprite size
               SpriteColorFormat_256Color, // sprite color fornat
               gfx,                        // pointer to the loaded graphics
               rotIndex,                   // affine rotation to use
               true,                       // double the size of rotated sprites
               false,                      // don't hide the sprite
               false, false,               // vflip, hflip
               false                       // apply mosaic
        );
      }
    }
    oamRotateScale(&oamMain, 0, 0, (1 << 8), (1 << 8));
    oamRotateScale(&oamMain, 1, angle - 32767, (1 << 8), (1 << 8));

    swiWaitForVBlank();

    if (keys & KEY_B) {
      break;
    }

    if (keys & KEY_START) {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          cells[i][j] = S_EMPTY;
        }
      }
      isGoose = 1;
      angle = 0;
      winner = 0;
    }

    oamUpdate(&oamMain);
  }

  return 0;
}
