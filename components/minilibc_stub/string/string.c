#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *tmp = (char *)malloc(len);

    if (!tmp)
    {
        return NULL;
    }

    memcpy(tmp, s, len);

    return tmp;
}

size_t strlcpy(char *dest, const char *src, size_t len)
{
    size_t ret = strlen(src);

    if (len != 0)
    {
        if (ret < len)
            strcpy(dest, src);
        else
        {
            strncpy(dest, src, len - 1);
            dest[len - 1] = 0;
        }
    }
    return ret;
}
