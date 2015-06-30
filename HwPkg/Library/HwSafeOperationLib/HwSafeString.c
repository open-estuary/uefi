/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Library\HwSafeOperationLib\HwSafeOperationLib.c

Description:

*************************************************/

#include "HwSafeOpInternal.h"

CHAR8 *
EFIAPI
strncpy_s(
  OUT char* strDest, 
  IN  unsigned int destMax,
  IN  const char* strSrc, 
  IN  unsigned int count)
{
    char*  pHeader = strDest;
    unsigned int maxSize = destMax;
    register const char* overlapGuard = NULL;

    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN)
    {
        ASSERT (destMax != 0 && destMax <= SECUREC_STRING_MAX_LEN);
        return 0;
    }

    if (strDest == NULL || strSrc == NULL )
    {
        if (strDest != NULL )
        {
            pHeader[0] = '\0';
        }
        ASSERT (strDest != NULL && strSrc != NULL);
        return 0;
    }
    if (count > SECUREC_STRING_MAX_LEN)
    {
        pHeader[0] = '\0'; /*clear dest string*/
        ASSERT (count <= SECUREC_STRING_MAX_LEN);
        return 0;
    }
    if (count == 0)
    {
        pHeader[0] = '\0';
        return strDest;
    }
    if (strDest < strSrc)
    {
        overlapGuard = strSrc;
        while ((*(strDest++) = *(strSrc++)) != '\0' && --maxSize > 0 && --count > 0)
        {
            if (strDest == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT (strDest != overlapGuard);
                return 0;
            }
        }
    }
    else
    {
        overlapGuard = strDest;
        while ((*(strDest++) = *(strSrc++)) != '\0' && --maxSize > 0 && --count > 0)
        {
            if (strSrc == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT (strDest != overlapGuard);
                return 0;
            }
        }
    }

    if (count == 0)
    {
        *strDest = '\0';
    }

    //if (maxSize == 0)
    //{
    //    pHeader[0] = '\0';
    //    ASSERT (maxSize != 0);
    //    return 0;
    //}

    return strDest;
}


int strcpy_s(char* strDest, unsigned int destMax, const char* strSrc)
{
    char* pHeader = strDest;
    unsigned int maxSize = destMax;
    register const char* overlapGuard = NULL;

    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN)
    {
        ASSERT (destMax != 0 && destMax <= SECUREC_STRING_MAX_LEN);
        return 0;
    }
    if (strDest == NULL || strSrc == NULL )
    {
        if (strDest != NULL )
        {
            pHeader[0] = '\0';
        }
        ASSERT (strDest != NULL && strSrc != NULL);
        return 0;
    }

    if (strDest == strSrc)
    {
        return 0;
    }

    if (strDest < strSrc)
    {
        overlapGuard = strSrc;
        while ((*(strDest++) = *(strSrc++)) != '\0' && --maxSize > 0)
        {
            if (strDest == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT (strDest != overlapGuard);
                return 0;
            }
        }
    }
    else
    {
        overlapGuard = strDest;
        while ((*(strDest++) = *(strSrc++)) != '\0' && --maxSize > 0)
        {
            if (strSrc == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT (strDest != overlapGuard);
                return 0;
            }
        }
    }


    if (maxSize == 0)
    {
        pHeader[0] = '\0';
        ASSERT (maxSize != 0);
        return 0;
    }

    return 1;
}


int strcat_s(char* strDest, unsigned int destMax, const char* strSrc)
{
    char*    pHeader = strDest;
    unsigned int  availableSize = destMax;
    register const char* overlapGuard = NULL;

    if (destMax == 0 || destMax > SECUREC_STRING_MAX_LEN)
    {
        ASSERT(destMax != 0 && destMax <= SECUREC_STRING_MAX_LEN);
        return 0;
    }

    if (strDest == NULL || strSrc == NULL)
    {
        if (strDest != NULL)
        {
            pHeader[0] = '\0';
        }
        ASSERT(strDest != NULL && strSrc != NULL);
        return 0;
    }


    if (strDest < strSrc)
    {
        overlapGuard = strSrc;
        while (availableSize > 0 && *strDest != 0)
        {
            if (strDest == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT(strDest != overlapGuard);
                return 0;
            }
            /*seek to string end*/
            strDest++;
            availableSize--;
        }

        /* strDest unterminated, return error. */
        if (availableSize == 0)
        {
            pHeader[0] = '\0';
            ASSERT(availableSize != 0);
            return 0;
        }
        /* if available > 0, then excute the strcat operation */
        while ((*strDest++ = *strSrc++) != 0 && --availableSize > 0)
        {
            if ( strDest == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT( strDest != overlapGuard);
                return 0;
            }
        }
    }
    else
    {
        overlapGuard = strDest;
        while (availableSize > 0 && *strDest != '\0')
        {
            /*seek to string end, and no need to check overlap*/
            strDest++;
            availableSize--;
        }

        /* strDest unterminated, return error. */
        if (availableSize == 0)
        {
            pHeader[0] = '\0';
            ASSERT(availableSize != 0);
            return 0;
        }
        while ((*strDest++ = *strSrc++) != '\0' && --availableSize > 0)
        {
            if ( strSrc == overlapGuard)
            {
                pHeader[0] = '\0';
                ASSERT( strSrc != overlapGuard);
                return 0;
            }
        }
    }

    /* strDest have not enough space, return error */
    if (availableSize == 0)
    {
        pHeader[0] = '\0';
        ASSERT(availableSize != 0);
        return 0;
    }

    return 1;
}





