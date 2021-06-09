#include <stdio.h>
#include <stdarg.h>
#include <aos/debug.h>
#include <assert.h>

void __assert_fail (const char *file, unsigned int line,
                        const char *func, const char *failedexpr)
{
    fprintf(stderr,
             "assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
             failedexpr, file, line,
             func ? ", function: " : "", func ? func : "");

    except_process(0);
    __builtin_unreachable();
}

void __assert_func (const char *file, int line,
                        const char *func, const char *failedexpr)
{
    fprintf(stderr,
             "assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
             failedexpr, file, line,
             func ? ", function: " : "", func ? func : "");

    except_process(0);
    __builtin_unreachable();
}