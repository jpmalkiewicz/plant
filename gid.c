#include <stdlib.h>
#include <unistd.h>

#include "gid.h"

struct plant_gid {
    const char *str;
    gid_t gid;
};

struct plant_gid *str_to_plant_gid(struct plant_gid *gid, const char *str)
{
    if (!gid && ((gid = malloc(sizeof(*gid))) == NULL)) {
        return 0;
    }
    gid->str = str;
    /* TODO: do this properly */
    gid->gid = 0;
    return 0;
}
