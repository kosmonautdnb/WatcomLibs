#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#ifdef _MSC_VER
#define NOT_DOS
#define ON_WINDOWS
#else
#define ON_DOS
#endif

#ifdef NOT_DOS
#include <stdint.h>
#else
#if defined(__WATCOMC__) && (__WATCOMC__ < 1200)
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#else
#include <stdint.h>
#endif
typedef unsigned int DWORD;
typedef unsigned int UINT;
#endif

#define INT8_MAX (0x7f)
#define INT8_MIN (-0x80)
#define INT16_MAX (0x7fff)
#define INT16_MIN (-0x8000)
#define INT32_MAX (0x7fffffff)
#define INT32_MIN (-0x80000000)
#define INT64_MAX (0x7fffffffffffffff)
#define INT64_MIN (-0x8000000000000000)
#define UINT8_MAX (0xff)
#define UINT8_MIN (0x00)
#define UINT16_MAX (0xffff)
#define UINT16_MIN (0x0000)
#define UINT32_MAX (0xffffffff)
#define UINT32_MIN (0x00000000)
#define UINT64_MAX (0xffffffffffffffff)
#define UINT64_MIN (0x0000000000000000)
#define SIZE_MAX UINT32_MAX

#ifndef NOT_DOS
#define snprintf_x4(__a__,__b__,__c__,__d__) sprintf(__a__,__c__,__d__)
#define snprintf_x5(__a__,__b__,__c__,__d__,__e__) sprintf(__a__,__c__,__d__,__e__)
#else
#define snprintf_x4(__a__,__b__,__c__,__d__) snprintf(__a__,__b__,__c__,__d__)
#define snprintf_x5(__a__,__b__,__c__,__d__,__e__) snprintf(__a__,__b__,__c__,__d__,__e__)
#endif

__inline float saturatef(float v) {return v<0?0:(v>1?1:v);}
__inline double saturated(double v) {return v<0?0:(v>1?1:v);}
__inline float clampf(float v, float l, float h) {return v<l?l:(v>h?h:v);}
__inline double clampd(double v, double l, double h) {return v<l?l:(v>h?h:v);}
__inline int clampi(int v, int l, int h) {return v<l?l:(v>h?h:v);}
__inline unsigned int clampui(unsigned int v, unsigned int l, unsigned int h) {return v<l?l:(v>h?h:v);}
__inline int maxi(int a, int b) {return a>b?a:b;}
__inline unsigned int maxui(unsigned int a, unsigned int b) {return a>b?a:b;}
__inline float maxf(float a, float b) {return a>b?a:b;}
__inline double maxd(double a, double b) {return a>b?a:b;}
__inline int mini(int a, int b) {return a<b?a:b;}
__inline unsigned int minui(unsigned int a, unsigned int b) {return a<b?a:b;}
__inline float minf(float a, float b) {return a<b?a:b;}
__inline double mind(double a, double b) {return a<b?a:b;}


#endif //__TYPES_HPP__