#include <sys/stat.h>
#include <stdlib.h>

#include "mode.h"

struct plant_mode {
    const char *str;
    mode_t mode;
};

struct plant_mode *str_to_plant_mode(struct plant_mode *mode, const char *str)
{
    if (!mode && ((mode = malloc(sizeof(*mode))) == NULL)) {
        return 0;
    }
    mode->str = str;
    /* TODO: do this properly */
    mode->mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH; /* 0644 */
    return 0;
}
