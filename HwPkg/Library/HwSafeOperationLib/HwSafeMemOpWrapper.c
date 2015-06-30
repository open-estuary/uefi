/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Library\HwSafeOperationLib\HwSafeMemOpWrapper.c

Description: 

*************************************************/

#include "HwSafeOpInternal.h"

VOID *
EFIAPI
memset_s (
  OUT void* dest,
  IN  UINTN destMax,
  IN UINT8 c,
  IN UINTN count)
{
    if (destMax == 0 || destMax > 0xc0000000UL)
    {
        ASSERT (destMax != 0 && destMax <= 0xc0000000UL);
        return 0;
    }
    
    //if (dest == NULL)
    //{
    //    SECUREC_ERROR_INVALID_PARAMTER("memset_s");
    //    return;
    //}
    
    if (count > destMax)
    {
        (void)SetMem (dest, destMax, c);   /*set entire buffer to value c*/
        ASSERT (count <= destMax);
        return dest;
    }

    /*use underlying memcpy for performance consideration*/
    (void)SetMem (dest, count, c);

    return dest;
}


VOID * 
EFIAPI
memcpy_s(
    OUT void* dest,
    IN  UINTN destMax,
    IN  const void* src,
    IN  UINTN count)
{
    if (destMax == 0 || destMax > 0xc0000000UL )
    {
        ASSERT (destMax != 0 && destMax <= 0xc0000000UL);
        return 0;
    }

    //if (dest == NULL || src == NULL)
    //{
    //    if (dest != NULL )
    //    {
    //        (void)memset(dest, 0, destMax);
    //    }
    //    SECUREC_ERROR_INVALID_PARAMTER("memcpy_s");
    //    return EINVAL;
    //}
    
    if (count > destMax)
    {
        (void)SetMem (dest, destMax, 0);
        ASSERT (count <= destMax);
        return 0;
    }
    if (dest == src)
    {
        return dest;
    }
    if ((dest > src && dest < (void*)((UINT8*)src + count)) ||
        (src > dest && src < (void*)((UINT8*)dest + count)) )
    {
        (void)SetMem (dest, destMax, 0);
        ASSERT (!((dest > src && dest < (void*)((UINT8*)src + count)) ||
        (src > dest && src < (void*)((UINT8*)dest + count))));
        return 0;
    }

    /*use underlying memcpy for performance consideration*/
    (void)CopyMem (dest, src, count);

    return dest;
}


int memmove_s(void* dest, UINT32 destMax, const void* src, UINT32 count)
{
    if (destMax == 0 || destMax > 0xc0000000UL )
    {
        ASSERT(destMax != 0 && destMax <= 0xc0000000UL );
        return 0;
    }
    if (dest == NULL || src == NULL)
    {
        if (dest != NULL)
        {
            (void)SetMem(dest, destMax, 0);
        }
        ASSERT(dest != NULL && src != NULL);
        return 0;
    }
    if (count > destMax)
    {
        (void)SetMem(dest, destMax, 0);
        ASSERT(count <= destMax);
        return 0;
    }
    if (dest == src)
    {
        return 1;
    }

    if (count > 0)
    {
        /*use underlying memmove for performance consideration*/
        (void)CopyMem (dest, src, count);
    }
    return 1;
}



