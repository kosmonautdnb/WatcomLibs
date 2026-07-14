//
// PCX and LBM loading library (loads them into a 32bit true color buffer)
//
//somehow derived from the ZLIB licensed project(on Github): Adaptive Graphics Image Display Library Copyright (c) 2023-2024 Ryandracus Chapman
//The assigned ZLib license (this license is regarded almost fully open source):
//
// Copyright (c) 2023 Ryandracus Chapman (@RyandracusCodesGames)
//
// This software is provided "as-is", without any express or implied warranty. In no event
// will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose, including commercial
// applications, and to alter it and redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not claim that you
//  wrote the original software. If you use this software in a product, an acknowledgment
//  in the product documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be misrepresented
//  as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
// modified/changed on 23.06.2026 by Stefan Mader (there may be have been errors introduced by that)
//
// It should read most of PCX and most of LBM formats as stated by the original author.
// This library may be instable/cause problems with certain PCX/LBM formats so maybe just take it with care.
// If you search for PCX/LBM and other image types also in "writemode" the original project of the original author seems to be the way to go.
//
// required:
// use #define __PCXLBMLOAD_IMPLEMENTATION__ onetime in one of your sourcefiles before including this header file
//
#ifndef __PCXLBML__
#define __PCXLBML__
//---------
//#ifdef __PCXLBMLOAD_MALLOCINSTEADOFNEW__
typedef unsigned int PCXLBM_U32;
typedef signed int PCXLBM_S32;
typedef unsigned short PCXLBM_U16;
typedef unsigned char PCXLBM_U8;

int loadLBM(const char *fileName, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height);
int loadLBM(void *fileData, PCXLBM_U32 dataLength, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height);

int loadPCX(const char *fileName, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height);
int loadPCX(void *fileData, PCXLBM_U32 dataLength, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height);


//---------
#ifdef __PCXLBMLOAD_IMPLEMENTATION__
//---------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PCXLBM_SEEK_SET 1
#define PCXLBM_SEEK_END 2
#define PCXLBM_SEEK_CUR 3
#define PCXLBM_888 1
#define PCXLBM_555 2
#define PCXLBM_ILBM 3
#define PCXLBM_PBM 4

#ifdef __PCXLBMLOAD_MALLOCINSTEADOFNEW__
  void *pcxlbm_allocate(PCXLBM_U32 byteCount) {
    return malloc(byteCount);
  }
  
  void pcxlbm_free(void *a) {
    free(a);
  }
#endif

#ifndef __PCXLBMLOAD_MALLOCINSTEADOFNEW__
  void *pcxlbm_allocate(PCXLBM_U32 byteCount) {
    return new PCXLBM_U8[byteCount];
  }
  
  void pcxlbm_free(void *a) {
    delete[] (PCXLBM_U8*)a;
  }
#endif

bool  lbmpcx_littleEndian = true; // littleEndian <> Intel, bigEndian <> Motorola
FILE *pcxlbm_file = 0;
void *pcxlbm_data = 0;
PCXLBM_U32 pcxlbm_size = 0;
PCXLBM_S32 pcxlbm_pos = 0;

PCXLBM_S32 pcxlbm_read(void *dest, PCXLBM_U32 sizea, PCXLBM_U32 sizeb) {
  if (pcxlbm_file != 0) {
    return fread(dest, sizea, sizeb, pcxlbm_file);
  }
  if (pcxlbm_data != 0) {
    PCXLBM_U8 *write = (PCXLBM_U8 *)dest;
    for (PCXLBM_U32 i = 0; i < sizea * sizeb; i++) {
      if ((PCXLBM_U32)pcxlbm_pos < pcxlbm_size) *write++ = ((PCXLBM_U8*)pcxlbm_data)[pcxlbm_pos++]; else *write++ = 0;
    }
    return sizea * sizeb;
  }
  return 0;
}

PCXLBM_S32 pcxlbm_seek(PCXLBM_S32 pos, PCXLBM_U32 mode) {
  if (pcxlbm_file != 0) {
    fseek(pcxlbm_file, pos, mode == PCXLBM_SEEK_SET ? SEEK_SET : (mode == PCXLBM_SEEK_END ? SEEK_END : SEEK_CUR));
    return 0;
  }
  if (pcxlbm_data != 0) {
    switch(mode) {
      case PCXLBM_SEEK_SET: pcxlbm_pos = pos; return 0;
      case PCXLBM_SEEK_END: pcxlbm_pos = pcxlbm_size - pos; return 0;
      case PCXLBM_SEEK_CUR: pcxlbm_pos += pos; return 0;
    }
  }
  return 0;
}

PCXLBM_U32 pcxlbm_tell() {
  if (pcxlbm_file != 0) {
    return ftell(pcxlbm_file);
  }
  if (pcxlbm_data != 0) {
    return pcxlbm_pos;
  }
  return 0;
}

PCXLBM_U32 pcxlbm_readLong() {
  PCXLBM_U32 a = 0; pcxlbm_read(&a, 1, sizeof(PCXLBM_U32));
  if (!lbmpcx_littleEndian) a = (((a>>24)&255)<<0) | (((a>>16)&255)<<8) | (((a>>8)&255)<<16) | (((a>>0)&255)<<24);
  return a;
}

PCXLBM_U16 pcxlbm_readShort() {
  PCXLBM_U16 a = 0; pcxlbm_read(&a, 1, sizeof(PCXLBM_U16));
  if (!lbmpcx_littleEndian) a = (PCXLBM_U16)((((a>>8)&255)<<0) | (((a>>0)&255)<<8));
  return a;
}

PCXLBM_U8 pcxlbm_readByte() {
  PCXLBM_U8 a = 0; pcxlbm_read(&a, 1, sizeof(PCXLBM_U8));
  return a;
}

// PCX

PCXLBM_U32 pcxlbm_readRGB() {
  PCXLBM_S32 r = pcxlbm_readByte();
  PCXLBM_S32 g = pcxlbm_readByte();
  PCXLBM_S32 b = pcxlbm_readByte();
  return r | (g<<8) | (b << 16) | 0xff000000;
}

typedef struct PCXHEADER {
  PCXLBM_U8 id;
  PCXLBM_U8 version;
  PCXLBM_U8 encoding;
  PCXLBM_U8 bits;
  PCXLBM_U16 x_start;
  PCXLBM_U16 y_start;
  PCXLBM_U16 x_end;
  PCXLBM_U16 y_end;
  PCXLBM_U32 width;
  PCXLBM_U32 height;
  PCXLBM_U8 ega[48];
  PCXLBM_U8 reserved;
  PCXLBM_U8 numbitplanes;
  PCXLBM_U16 bytesperline;
  PCXLBM_U16 pal_type;
  PCXLBM_U16 screen_horz;
  PCXLBM_U16 screen_vert;
  PCXLBM_U8 blanks[54];
} PCXHEADER;

PCXLBM_S32 contains_icp(PCXLBM_U32 *pal_coord) {
  pcxlbm_seek(0, PCXLBM_SEEK_END);

  PCXLBM_U32 file_size = pcxlbm_tell();
  PCXLBM_U32 palette_coord = file_size - 769;

  if (file_size <= 769) {
    return 0;
  }

  pcxlbm_seek(palette_coord, PCXLBM_SEEK_SET);

  *pal_coord = palette_coord;

  PCXLBM_U8 byte = 0;
  pcxlbm_read(&byte,1,1);

  pcxlbm_seek(0,PCXLBM_SEEK_SET);

  if (byte == 12) {
    return 1;
  }
  return 0;
}

int loadPCX(PCXLBM_U32 **destPixels, PCXLBM_U32 *destWidth, PCXLBM_U32 *destHeight) {

  lbmpcx_littleEndian = true;

  *destPixels = 0;
  *destWidth = 0;
  *destHeight = 0;

  PCXHEADER header;

  header.id = pcxlbm_readByte();
  header.version = pcxlbm_readByte();
  header.encoding = pcxlbm_readByte();
  header.bits = pcxlbm_readByte();
  header.x_start = pcxlbm_readShort();
  header.y_start = pcxlbm_readShort();
  header.x_end = pcxlbm_readShort();
  header.y_end = pcxlbm_readShort();
  header.width = pcxlbm_readShort();
  header.height = pcxlbm_readShort();

  PCXLBM_S32 i;
  for( i = 0; i < 48; i++) {
    char clr = pcxlbm_readByte();
    header.ega[i] = clr;
  }

  header.reserved = pcxlbm_readByte();
  header.numbitplanes = pcxlbm_readByte();
  header.bytesperline = pcxlbm_readShort();
  header.pal_type = pcxlbm_readShort();
  header.screen_horz = pcxlbm_readShort();
  header.screen_vert = pcxlbm_readShort();

  for( i = 0; i < 54; i++) {
    char blank = pcxlbm_readByte();
    header.blanks[i] = blank;
  }

  if( (header.x_end - header.x_start) == 0) {

    header.width++;
    header.height++;

    if (header.width > 258) {
      header.width = ((header.width>>8) & 0x00ff) | ((header.width<<8) & 0xff00);
    }

    if(header.height > 258) {
      header.height = ((header.height>>8) & 0x00ff) | ((header.height<<8) & 0xff00);
    }
  }
  else {

    header.width = (header.x_end - header.x_start) + 1;
    header.height = (header.y_end - header.y_start) + 1;
  }

  if (header.id != 10 || !(header.version == 0 || header.version == 2 || header.version == 3 || header.version == 4 || header.version == 5)) {
    return -1;
  }
  else if(header.x_end > 4096 || header.y_end > 4096) {
    return -2;
  }

  PCXLBM_U32 fmt = 0;
  (void)(fmt); // unused   

  if(header.bits == 24 || header.bits == 8 || header.bits == 1) {
    fmt = PCXLBM_888;
  }

  if(header.bits == 16) {
    fmt = PCXLBM_555;
  }

  PCXLBM_U32 width = header.width, height = header.height;

  *destWidth = width;
  *destHeight = height;
  *destPixels = (PCXLBM_U32*)pcxlbm_allocate(width * height * sizeof(PCXLBM_U32));

  long scanlinelength = header.bytesperline * header.numbitplanes;

  PCXLBM_U16 roffset = 0, goffset = (PCXLBM_U16)header.bytesperline, boffset = (PCXLBM_U16)(goffset * 2);

  PCXLBM_U32 pal_coord = 0;

  if(!contains_icp(&pal_coord)) {

    pcxlbm_seek(128,PCXLBM_SEEK_SET);

    PCXLBM_U32 scanline;
    for(scanline = 0; scanline < height; scanline++) {


      PCXLBM_U8 *buf = (PCXLBM_U8*)pcxlbm_allocate(sizeof(PCXLBM_U8)*scanlinelength);

      PCXLBM_S32 count = 0;

      while(count < scanlinelength) {

        PCXLBM_U8 byte = pcxlbm_readByte();
        PCXLBM_U8 fbit = (PCXLBM_U8)((byte >> 7) & 1);
        PCXLBM_U8 sbit = (PCXLBM_U8)((byte >> 6) & 1);
        PCXLBM_U8 bot  = (PCXLBM_U8)(byte & 0x3f);

        if(fbit == 1 && sbit == 1) {

          PCXLBM_U16 rle = bot;
          PCXLBM_U8 read = pcxlbm_readByte();

          PCXLBM_U32 i;

          for(i = 0; i < rle; i++){
            buf[count++] = read;
          }
        }
        else
        {

          PCXLBM_U32 iszero = 1;
          PCXLBM_U32 zero;

          for(zero = 1; zero < 8; zero++) {
            if((byte >> zero) != 0) {
              iszero = 0;
            }
          }

          if(iszero) {
            buf[count] = 0;
            count++;
          }
          else
          {
            buf[count] = byte;
            count++;
          }
        }
      }

      if(header.bits == 24 || header.bits == 8) {

        PCXLBM_U32 x;

        for(x = 0; x < width; x++) {
          PCXLBM_S32 r = buf[roffset+x];
          PCXLBM_S32 g = buf[goffset+x];
          PCXLBM_S32 b = buf[boffset+x];
          (*destPixels)[x+scanline*width] = r | (g<<8) | (b<<16) | 0xff000000;
        }
      }
      else
      {

        PCXLBM_U32 x;

        for(x = 0; x < width; x++) {
          PCXLBM_S32 r = buf[roffset+x]<<3;
          PCXLBM_S32 g = buf[goffset+x]<<3;
          PCXLBM_S32 b = buf[boffset+x]<<3;
          (*destPixels)[x+scanline*width] = r | (g<<8) | (b<<16) | 0xff000000;
        }
      }

      pcxlbm_free(buf);
    }
  }
  else
  {

    pcxlbm_seek(pal_coord + 1, PCXLBM_SEEK_SET);

    PCXLBM_U32 *palette = (PCXLBM_U32*)pcxlbm_allocate(256 * sizeof(PCXLBM_U32));

    PCXLBM_U32 i;
    for(i = 0; i < 256; i++) {
      palette[i] = pcxlbm_readRGB();
    }

    pcxlbm_seek(128, PCXLBM_SEEK_SET);

    PCXLBM_U32 scanline;
    for (scanline = 0; scanline < height; scanline++) {

      PCXLBM_U8 *buf = (PCXLBM_U8*)pcxlbm_allocate(sizeof(PCXLBM_U8) * scanlinelength);

      PCXLBM_S32 count = 0;
      while(count < scanlinelength) {

        PCXLBM_U8 byte = pcxlbm_readByte();
        PCXLBM_U8 fbit = (PCXLBM_U8)((byte >> 7) & 1);
        PCXLBM_U8 sbit = (PCXLBM_U8)((byte >> 6) & 1);
        PCXLBM_U8 bot  = (PCXLBM_U8)(byte & 0x3f);

        if(sbit == 1 && fbit == 1) {

          PCXLBM_U16 rle = bot;
          PCXLBM_U8 read = pcxlbm_readByte();

          PCXLBM_U32 i;
          for(i = 0; i < rle; i++) {
            buf[count] = read;
            count++;
          }
        }
        else
        {

          PCXLBM_U32 iszero = 1;
          PCXLBM_S32 zero;

          for(zero = 1; zero < 8; zero++) {
            if ((byte >> zero) != 0) {
              iszero = 0;
            }
          }

          if (iszero) {
            buf[count] = 0;
            count++;
          }
          else
          {
            buf[count] = byte;
            count++;
          }
        }
      }

      PCXLBM_U32 x;
      for(x = 0; x < width; x++) {
        (*destPixels)[x+scanline*width] = palette[buf[x]];
      }

      pcxlbm_free(buf);
    }

    pcxlbm_free(palette);
  }
  return 0;
}

int loadPCX(const char *fileName, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height) {
  pcxlbm_file = fopen(fileName,"rb");
  if (pcxlbm_file != 0) {
    int ret = loadPCX(dest, width, height);
    fclose(pcxlbm_file);
    pcxlbm_file = 0;
    return ret;
  }
  return -3;
}

int loadPCX(void *fileData, PCXLBM_U32 dataLength, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height) {
  pcxlbm_data = fileData;
  pcxlbm_size = dataLength;
  pcxlbm_pos = 0;
  int ret = loadPCX(dest, width, height);
  pcxlbm_data = 0;
  return ret;
}

// LBM

typedef enum ILBM_TYPE {
  TYPE_ILBM = 0x1,
  TYPE_PBM = 0x2,
} AGIDL_ILBM_TYPE;

typedef struct ILBM_FORM {
  char formid[4];
  PCXLBM_U32 size;
  char ilbmid[4];
} ILBM_FORM;

typedef struct ILBM_BMHD {
  char bmhdid[4];
  PCXLBM_U32 size;
  PCXLBM_U32 width;
  PCXLBM_U32 height;
  PCXLBM_U16 x_off;
  PCXLBM_U16 y_off;
  PCXLBM_U8 bpp;
  PCXLBM_U8 mask;
  PCXLBM_U8 compress;
  PCXLBM_U8 pad;
  PCXLBM_U16 chromakey;
  PCXLBM_U8 x_aspect;
  PCXLBM_U8 y_aspect;
  PCXLBM_U16 horz_res;
  PCXLBM_U16 vert_res;
} ILBM_BMHD;

typedef struct ILBM_CMAP {
  char cmapid[4];
  PCXLBM_U32 size;
  PCXLBM_U32 palette[256];
} ILBM_CMAP;

typedef struct ILBM_CLUT {
  char clutid[4];
  PCXLBM_U32 size;
  PCXLBM_U8 rlut[256];
  PCXLBM_U8 glut[256];
  PCXLBM_U8 blut[256];
  PCXLBM_U8 ilut[256];
} ILBML_CLUT;

typedef struct ILBM_CRNG {
  char crngid[4];
  PCXLBM_U32 size;
  PCXLBM_U16 pad;
  PCXLBM_U16 rate;
  PCXLBM_U16 flags;
  PCXLBM_U8 low;
  PCXLBM_U8 high;
} ILBM_CRNG;

typedef struct ILBM_CMAG {
  char cmagid[4];
  PCXLBM_U32 size;
  PCXLBM_U32 viewport_mode;
} ILBM_CMAG;

typedef struct ILBM_ANNO {
  char annoid[4];
  PCXLBM_U32 size;
} ILBM_ANNO;

typedef struct ILBM_BODY {
  char bodyid[4];
  PCXLBM_U32 size;
  PCXLBM_U8* src;
  PCXLBM_U8* interleave;
  PCXLBM_U8* dest;
} ILBM_BODY;

typedef struct ILBM_HEADER {
  ILBM_FORM form;
  ILBM_BMHD bmhd;
  ILBM_CMAP cmap;
  ILBM_CLUT clut;
  ILBM_CRNG crng;
  ILBM_CMAG cmag;
  ILBM_BODY body;
  ILBM_ANNO anno;
} ILBM_HEADER;

PCXLBM_U32 Align(PCXLBM_U32 l){
  if (l&1)
    return l+1;
  return l;
}

void PCXLBM_LBMDeinterleaveData(ILBM_HEADER *header, PCXLBM_U8 *src, PCXLBM_U8 *dest) {
  PCXLBM_U8 bpp = header->bmhd.bpp;
  PCXLBM_U32 bytesperline = ((header->bmhd.width + 15) / 16) * 2;
  
  if (header->bmhd.mask == 1) {
    bpp += 1;
  }
  
  PCXLBM_U32 y,b,p,i;
  for(y = 0; y < header->bmhd.height; y++) {
    for(p = 0; p < bpp; p++) {
      PCXLBM_U16 plane_mask = (PCXLBM_U16)(1 << p);
      for(i = 0; i < bytesperline; i++) {
        PCXLBM_U32 bit_offset = (y * bpp * bytesperline) + (p * bytesperline) + i;
        PCXLBM_U8 bit_value = src[bit_offset];
        for(b = 0; b < 8; b++) {
          PCXLBM_U32 pixel_mask = 1 << (7 - b);
          if(bit_value & pixel_mask) {
            PCXLBM_U32 x = (i * 8) + b;
            dest[(y * header->bmhd.width) + x] |= (PCXLBM_U8)plane_mask; // 16bit/8bit problem (should be no problem)
          }
        }
      }
    }
  }
}


void PCXLBM_PackBits(PCXLBM_U8 *src, PCXLBM_U8 *dest, PCXLBM_U32 max_rle) {
  PCXLBM_U32 count = 0;

  do {
    PCXLBM_U8 byte = *src++;

    if (byte > 128) {
      PCXLBM_U8 nextbyte = *src++;

      PCXLBM_S32 i;
      for(i = 0; i < 257 - byte; i++, count++) {
        *dest++ = nextbyte;
      }
    } else if (byte < 128) {
      PCXLBM_U32 i;
      for(i = 0; i < (PCXLBM_U32)byte + 1; i++, count++) {
        *dest++ = *src++;
      }
    } else {
      count = max_rle;
    }
  } while (count < max_rle);
}

void PCXLBM_LBMDecompressData(ILBM_HEADER *header, PCXLBM_U8 *src, PCXLBM_U8 *dest) {
  header->bmhd.width = (header->bmhd.width + 15) & 0xFFFFFFF0;
  PCXLBM_U32 bytesperline = ((header->bmhd.width + 15) / 16) * 2;

  if(header->bmhd.bpp != 1) {
    PCXLBM_PackBits(src,dest,header->bmhd.width * header->bmhd.height);
  }
  else {

    PCXLBM_U32 y;
    for(y = 0; y < header->bmhd.height; y++) {
      PCXLBM_U32 count = 0;

      do {
        PCXLBM_U8 byte = *src++;

        if(byte > 128) {
          PCXLBM_U8 nextbyte = *src++;

          PCXLBM_S32 i;
          for(i = 0; i < 257 - byte; i++, count++) {
            *dest++ = nextbyte;
          }
        }
        else if(byte < 128) {
          PCXLBM_U32 i;
          for(i = 0; i < (PCXLBM_U32)byte + 1; i++, count++) {
            *dest++ = *src++;
          }
        }
        else {
          count = bytesperline;
        }
      } while(count < bytesperline);
    }
  }
}

int loadLBM(PCXLBM_U32 **destPixels, PCXLBM_U32 *destWidth, PCXLBM_U32 *destHeight) {

  lbmpcx_littleEndian = false;

  *destPixels = 0;
  *destWidth = 0;
  *destHeight = 0;

  ILBM_HEADER header;

  PCXLBM_S32 clut = 0;
  PCXLBM_S32 type = 0;
  PCXLBM_S32 fmt = 0;
  (void)(clut); // unused
  (void)(fmt); // unused

  header.form.formid[0] = pcxlbm_readByte();
  header.form.formid[1] = pcxlbm_readByte();
  header.form.formid[2] = pcxlbm_readByte();
  header.form.formid[3] = pcxlbm_readByte();
  
  header.form.size = pcxlbm_readLong();
  
  header.form.ilbmid[0] = pcxlbm_readByte();
  header.form.ilbmid[1] = pcxlbm_readByte();
  header.form.ilbmid[2] = pcxlbm_readByte();
  header.form.ilbmid[3] = pcxlbm_readByte();
  
  header.bmhd.bmhdid[0] = pcxlbm_readByte();
  header.bmhd.bmhdid[1] = pcxlbm_readByte();
  header.bmhd.bmhdid[2] = pcxlbm_readByte();
  header.bmhd.bmhdid[3] = pcxlbm_readByte();
  
  header.bmhd.size = pcxlbm_readLong();
  header.bmhd.width = pcxlbm_readShort();
  header.bmhd.height = pcxlbm_readShort();
  header.bmhd.x_off = pcxlbm_readShort();
  header.bmhd.y_off = pcxlbm_readShort();
  header.bmhd.bpp = pcxlbm_readByte();
  header.bmhd.mask = pcxlbm_readByte();
  header.bmhd.compress = pcxlbm_readByte();
  header.bmhd.pad = pcxlbm_readByte();
  header.bmhd.chromakey = pcxlbm_readShort();
  header.bmhd.x_aspect = pcxlbm_readByte();
  header.bmhd.y_aspect = pcxlbm_readByte();
  header.bmhd.horz_res = pcxlbm_readShort();
  header.bmhd.vert_res = pcxlbm_readShort();

  PCXLBM_U8 fourcc[4] = {0,0,0,0};

  do {

    fourcc[0] = pcxlbm_readByte();
    fourcc[1] = pcxlbm_readByte();
    fourcc[2] = pcxlbm_readByte();
    fourcc[3] = pcxlbm_readByte();
    
    PCXLBM_U32 size = pcxlbm_readLong();
    
    if(fourcc[0] == 'B' && fourcc[1] == 'O' && fourcc[2] == 'D' && fourcc[3] == 'Y') {
      header.body.size = size;
      header.body.src = (PCXLBM_U8*)pcxlbm_allocate(size);
      pcxlbm_read(header.body.src,1,size);
    }
    else
    {
      if(fourcc[0] == 'C' && fourcc[1] == 'M' && fourcc[2] == 'A' && fourcc[3] == 'P') {
        PCXLBM_U32 num_of_colors = 1 << header.bmhd.bpp;

        if(num_of_colors > 256) {
          return -1;
        }

        PCXLBM_U32 i;
        for(i = 0; i < num_of_colors; i++) {
          PCXLBM_U8 r = pcxlbm_readByte();
          PCXLBM_U8 g = pcxlbm_readByte();
          PCXLBM_U8 b = pcxlbm_readByte();

          header.cmap.palette[i] = r | (g<<8) | (b<<16) | 0xff000000;
        }
      }
      else if(fourcc[0] == 'C' && fourcc[1] == 'L' && fourcc[2] == 'U' && fourcc[3] == 'T') {
        PCXLBM_U32 cluttype = pcxlbm_readLong();
        PCXLBM_U32 reserved = pcxlbm_readLong();
        (void)(reserved); // unused
        
        clut = 1;
        
        switch(cluttype) {
          case 0: {
            PCXLBM_U32 i;
            for(i = 0; i < 256; i++) {
              header.clut.ilut[i] = pcxlbm_readByte();
            }
          } break;
          case 1: {
            PCXLBM_U32 i;
            for(i = 0; i < 256; i++) {
              header.clut.rlut[i] = pcxlbm_readByte();
            }
          } break;
          case 2: {
            PCXLBM_U32 i;
            for(i = 0; i < 256; i++) {
              header.clut.glut[i] = pcxlbm_readByte();
            }
          } break;
          case 3: {
            PCXLBM_U32 i;
            for(i = 0; i < 256; i++){
              header.clut.blut[i] = pcxlbm_readByte();
            }
          }break;
        }
      }
      else
      {
        size = Align(size);
        pcxlbm_seek(size,PCXLBM_SEEK_CUR);
      }
    }

    //if(fourcc[0] == 'T' || fourcc[1] != 'I' || fourcc[2] != 'N' || fourcc[3] != 'Y') {
    if(fourcc[0] == 'T' && fourcc[1] != 'I' && fourcc[2] != 'N' && fourcc[3] != 'Y') { // maybe message to author asking why there where ||'s
      fourcc[0] = 'C';
      fourcc[1] = 'R';
      fourcc[2] = 'N';
      fourcc[3] = 'G';
    }
   
  } while(fourcc[0] != 'B' && fourcc[1] != 'O' && fourcc[2] != 'D' && fourcc[3] != 'Y' && pcxlbm_tell() < 6000);
  
  if((header.form.size % 2) != 0) {
    pcxlbm_readByte();
  }

  if(header.form.ilbmid[0] == 'I' && header.form.ilbmid[1] == 'L' && header.form.ilbmid[2] == 'B' && header.form.ilbmid[3] && 'M') {
    type = PCXLBM_ILBM;
  }
  else
  {
    type = PCXLBM_PBM;
  }
  
  if(header.form.formid[0] != 'F' || header.form.formid[1] != 'O' || header.form.formid[2] != 'R'  || header.form.formid[3] != 'M'
   || !(header.form.ilbmid[0] == 'I' || header.form.ilbmid[1] == 'L' || header.form.ilbmid[2] == 'B' || header.form.ilbmid[3] == 'M' ||
   header.form.ilbmid[0] == 'P' || header.form.ilbmid[1] == 'B' || header.form.ilbmid[2] == 'M' || header.form.ilbmid[3] == ' ')) {
    return -2;
  }
  else if(header.bmhd.bmhdid[0] != 'B' || header.bmhd.bmhdid[1] != 'M' || header.bmhd.bmhdid[2] != 'H' || header.bmhd.bmhdid[3] != 'D' ||
    header.bmhd.size != 0x14) {
    return -3;
  }
  else if(header.bmhd.bpp > 8 || header.bmhd.bpp < 1 || !(header.bmhd.compress == 0 || header.bmhd.compress == 1)) {
    return -4;
  }

  if(header.bmhd.bpp != 24 && header.bmhd.bpp != 32) {

    fmt = PCXLBM_888;
    *destWidth = header.bmhd.width;
    *destHeight = header.bmhd.height;
    *destPixels = (PCXLBM_U32*)pcxlbm_allocate(header.bmhd.width * header.bmhd.height * sizeof(PCXLBM_U32));
    
    if(header.bmhd.compress == 1) {

      header.body.dest = (PCXLBM_U8*)pcxlbm_allocate(header.bmhd.width * header.bmhd.height);
      
      if(type == TYPE_ILBM) {
        
        if(header.bmhd.bpp != 1) {
          header.body.interleave = (PCXLBM_U8*)pcxlbm_allocate(header.bmhd.width * header.bmhd.height);
        }
        else
        {
          header.body.interleave = (PCXLBM_U8*)pcxlbm_allocate((header.bmhd.width * header.bmhd.height+7)/8);
        }

        PCXLBM_LBMDecompressData(&header,header.body.src,header.body.interleave);
        PCXLBM_LBMDeinterleaveData(&header,header.body.interleave,header.body.dest);
        
        PCXLBM_U32 i;
        for(i = 0; i < header.bmhd.width * header.bmhd.height; i++) {
          ((PCXLBM_U32*)*destPixels)[i] = header.cmap.palette[header.body.dest[i]];
        }
        
        pcxlbm_free(header.body.interleave);
        pcxlbm_free(header.body.dest);
        pcxlbm_free(header.body.src);
      }
      else 
      {
        PCXLBM_LBMDecompressData(&header,header.body.src,header.body.dest);
      
        PCXLBM_U32 i;
        for(i = 0; i < header.bmhd.width * header.bmhd.height; i++) {
          ((PCXLBM_U32*)*destPixels)[i] = header.cmap.palette[header.body.dest[i]];
        }
      
        pcxlbm_free(header.body.dest);
        pcxlbm_free(header.body.src);
      }
    }
    else
    {
      header.body.dest = (PCXLBM_U8*)pcxlbm_allocate(header.bmhd.width * header.bmhd.height);
      
      if(type == TYPE_ILBM) {
        PCXLBM_LBMDeinterleaveData(&header,header.body.src, header.body.dest);
      }
      else
      {
        memcpy(header.body.dest, header.body.src, header.bmhd.width * header.bmhd.height);
      }
      
      PCXLBM_U32 i;
      for(i = 0; i < header.bmhd.width * header.bmhd.height; i++){
        ((PCXLBM_U32*)*destPixels)[i] = header.cmap.palette[header.body.dest[i]];
      }
      
      pcxlbm_free(header.body.dest);
      pcxlbm_free(header.body.src);
    }
  }
  return 0;
}

int loadLBM(const char *fileName, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height) {
  pcxlbm_file = fopen(fileName,"rb");
  if (pcxlbm_file != 0) {
    int ret = loadLBM(dest, width, height);
    fclose(pcxlbm_file);
    pcxlbm_file = 0;
    return ret;
  }
  return -3;
}

int loadLBM(void *fileData, PCXLBM_U32 dataLength, PCXLBM_U32 **dest, PCXLBM_U32 *width, PCXLBM_U32 *height) {
  pcxlbm_data = fileData;
  pcxlbm_size = dataLength;
  pcxlbm_pos = 0;
  int ret = loadLBM(dest, width, height);
  pcxlbm_data = 0;
  return ret;
}

//---------
#endif // __PCXLBMLOAD_IMPLEMENTATION__
//---------

#endif // __PCXLBML__