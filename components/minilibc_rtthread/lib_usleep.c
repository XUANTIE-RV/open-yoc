#include <stdint.h>
#include <unistd.h>
#include <errno.h>

extern uint32_t msleep(uint32_t ms);
__attribute__((weak)) int usleep(useconds_t usec)
{
    if (usec && (usec <= 1000000)) {
        if (usec < 1000) {
            usec = 1000;
        }

        msleep(usec / 1000);
        return 0;
    }

    return -EINVAL;
}
