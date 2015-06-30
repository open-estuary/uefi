/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Include\Library\HwSafeOperationLib.h

Description:

*************************************************/

#ifndef __HW_SAFE_OPERATION_LIB_H__
#define __HW_SAFE_OPERATION_LIB_H__

/* 
 * Override BaseLib.h
 */
CHAR8 *
EFIAPI
strncpy_s(
  OUT char* strDest, 
  IN  unsigned int destMax,
  IN  const char* strSrc, 
  IN  unsigned int count);

int strcpy_s(char* strDest, unsigned int destMax, const char* strSrc);

int strcat_s(char* strDest, unsigned int destMax, const char* strSrc);
 //end

/* 
 * Override BaseMemoryLib.h
 */
VOID * 
EFIAPI
memcpy_s(
    OUT void* dest,
    IN  UINTN destMax,
    IN  const void* src,
    IN  UINTN count);

VOID *
EFIAPI
memset_s (
  OUT void* dest,
  IN  UINTN destMax,
  IN UINT8 c,
  IN UINTN count);
int memmove_s(void* dest, UINT32 destMax, const void* src, UINT32 count);
//end

/* 
 * Override PrintLib.h
 */
int vsnprintf_s (char* strDest, unsigned int destMax, unsigned int count, const char* format, VA_LIST arglist);
int vsnprintf_helper (char* string, unsigned int count, const char* format, VA_LIST arglist);
int snprintf_s (char* strDest, unsigned int destMax, unsigned int count, const char* format, ...);
//end

#endif

