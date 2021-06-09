#include <stdio.h>
#include <stdarg.h>

int __cskyvprintfsprintf(char *str, FILE *stream, const char *format, va_list arg_ptr)
{
    return 0;
}

int __cskyvprintfprintf(FILE *stream, const char *format, va_list arg_ptr)
{
    return 0;
}

// FIXME: for libsonaeq compile pass
#undef stdout
int stdout()
{
    return 0;
}
