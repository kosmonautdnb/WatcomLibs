#include "IMAGE.HPP"
#define __SUPERXBR_IMPLEMENTATION__
#include "SUPERXBR.HPP"

int main(int argc, const char *argv[]) {

  // startup
  printf("Pixelart Image Upscaler\n");
  printf("-------------------------\n");
  if (argc != 2) {
    printf("usage: app_scal.exe <imageFileName>\n");
    printf("supported formats: JPEG,PNG,BMP,PSD,TGA,GIF,HDR,PIC,PNM\n");
    printf("the source folder should contain a test.png\n");
    printf("currently just Super-XBR supported.\n");
    exit(0);
  }
  const char *fileName = argv[1];
  printf("loading image <%s>\n", fileName);
  RGBAImage img = RGBAImage::fromFile(fileName);
  if (img.data == NULL) {
    printf("couldn't load / not supported image <%s>.\n", fileName);
    exit(0);
  }
  printf("Generating SCALED.PNG\n");

  RGBAImage scaled;
  scaled.width = img.width * 2;
  scaled.height = img.height * 2;
  scaled.data = (unsigned int*)malloc(scaled.width*scaled.height*sizeof(unsigned int));
  superXBR(img.data,scaled.data,img.width,img.height);
  scaled.savePNG("SCALED.PNG");
  scaled.free();

  // return to DOS
  return 0;
}
