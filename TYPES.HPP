#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#ifdef _MSC_VER
#define NOT_DOS
#define ON_WINDOWS
#endif

#ifdef NOT_DOS
#include <stdint.h>
#else
typedef long int int64_t;
typedef unsigned long int uint64_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef uint32_t intptr_t;
typedef unsigned int DWORD;
typedef unsigned int UINT;
#endif

#ifndef NOT_DOS
#define snprintf_x4(__a__,__b__,__c__,__d__) sprintf(__a__,__c__,__d__)
#define snprintf_x5(__a__,__b__,__c__,__d__,__e__) sprintf(__a__,__c__,__d__,__e__)
#else
#define snprintf_x4(__a__,__b__,__c__,__d__) snprintf(__a__,__b__,__c__,__d__)
#define snprintf_x5(__a__,__b__,__c__,__d__,__e__) snprintf(__a__,__b__,__c__,__d__,__e__)
#endif


#endif //__TYPES_HPP__