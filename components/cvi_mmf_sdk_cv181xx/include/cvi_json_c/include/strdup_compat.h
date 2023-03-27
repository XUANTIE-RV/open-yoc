#ifndef __strdup_compat_h
#define __strdup_compat_h

/**
 * @file
 * @brief Do not use, cvi_json-c internal, may be changed or removed at any time.
 */

#if !defined(HAVE_STRDUP) && defined(_MSC_VER)
/* MSC has the version as _strdup */
#define strdup _strdup
#elif !defined(HAVE_STRDUP)
//#error You do not have strdup on your system.
char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = malloc(len);
    if (dup_str == NULL)
        return NULL;

    return (char *)memcpy(dup_str, s, len);
}
#endif /* HAVE_STRDUP */

#endif
