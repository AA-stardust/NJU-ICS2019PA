#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
//#include<cstdio>
int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  //printf("aa0\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  //printf("aa1\n");
  assert(bmp->pixels);
  //printf("aa\n");
  NDL_OpenDisplay(bmp->w, bmp->h);
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  NDL_Render();
  NDL_CloseDisplay();
  while (1);
  return 0;
}
