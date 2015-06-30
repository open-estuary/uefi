/*************************************************

Copyright (C), 1988-2010, Huawei Tech. Co., Ltd.

File name: HwPkg\Library\HwSafeOperationLib\HwSafeOperationLib.c

Description:

*************************************************/

#include "HwSafeOpInternal.h"

typedef struct _SECUREC_XPRINTF_STREAM
{
    int _cnt;
    char* _ptr;
} SECUREC_XPRINTF_STREAM;


#define __putc_nolock(_c,_stream)    (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) :  EOF)

int vsnprintf_helper (char* string, unsigned int count, const char* format, VA_LIST arglist)
{
    SECUREC_XPRINTF_STREAM str;
    int retval;
    
    ASSERT(format != NULL);                   /*lint !e506*/
    ASSERT(count != 0);                       /*lint !e506*/
    ASSERT(count <= SECUREC_STRING_MAX_LEN);  /*lint !e506*/
    ASSERT(string != NULL);                   /*lint !e506*/

    if((format != NULL) && (string != NULL))
    {
        str._cnt = (int)count;

        str._ptr = string;

        retval = AsciiVSPrint(string, count, format, arglist );

        if ((retval >= 0) && (__putc_nolock('\0', &str) != EOF))
        {
            return (retval);
        }

        if (string != NULL)
        {
            string[count - 1] = 0;
        }

        if (str._cnt < 0)
        {
            /* the buffer was too small; we return -2 to indicate truncation */
            return -2;
        }
    }
    return -1;
}


int vsnprintf_s (char* strDest, unsigned int destMax, unsigned int count, const char* format, VA_LIST arglist)
{
    int retvalue = -1;

    if (format == NULL || strDest == NULL || destMax == 0 || destMax > SECUREC_STRING_MAX_LEN || count > (SECUREC_STRING_MAX_LEN - 1))
    {
        if (strDest != NULL && destMax > 0)
        {
            strDest[0] = '\0';
        }
        ASSERT(format != NULL && strDest != NULL && destMax != 0 && destMax <= SECUREC_STRING_MAX_LEN && count <= (SECUREC_STRING_MAX_LEN - 1));
        return -1;
    }

    if (destMax > count)
    {
        retvalue = vsnprintf_helper(strDest, count + 1, format, arglist);
        if (retvalue == -2)    /* lsd add to keep dest buffer not destroyed 2014.2.18 */
        {
            /* the string has been truncated, return  -1 */
            return -1;     /* to skip error handler,  return strlen(strDest) or -1 */
        }
    }
    else /* destMax <= count */
    {
        retvalue = vsnprintf_helper(strDest, destMax, format, arglist);
        strDest[destMax - 1] = 0;
    }

    if (retvalue < 0)
    {
        strDest[0] = '\0';    /* empty the dest strDest */

        if (retvalue == -2)
        {
            /* Buffer too small */
            ASSERT(retvalue != -2);
        }

        ASSERT(retvalue >= 0);
        return -1;
    }

    return retvalue;
}


int snprintf_s (char* strDest, unsigned int destMax, unsigned int count, const char* format, ...)
{
    int ret = 0;
    VA_LIST arglist = {0};

    VA_START(arglist, format);
    ret = vsnprintf_s(strDest, destMax, count, format, arglist);
    VA_END(arglist);

    return ret;
}


