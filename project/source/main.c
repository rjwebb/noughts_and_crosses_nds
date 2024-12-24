#include <gl2d.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>

glImage Crab[1];

int main(void) {
  // put the main screen on the bottom lcd
  lcdMainOnBottom();

  // Sprite initialisation
  videoSetMode(MODE_5_3D);

  consoleDemoInit();

  int keys = 0;

  glScreen2D();

  while (pmMainLoop() && !(keys & KEY_B)) {

    glBegin2D();

    // draw a triangle
    glTriangleFilled(20, 100, 200, 30, 60, 40, RGB15(31, 0, 31));

    glEnd2D();

    glFlush(0);

    swiWaitForVBlank();
    scanKeys();
    keys = keysHeld();
  }

  return 0;
}
