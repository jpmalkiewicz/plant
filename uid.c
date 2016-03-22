#include <stdlib.h>
#include <unistd.h>

#include "uid.h"

struct plant_uid {
    const char *str;
    uid_t uid;
};

struct plant_uid *str_to_plant_uid(struct plant_uid *uid, const char *str)
{
    if (!uid && ((uid = malloc(sizeof(*uid))) == NULL)) {
        return 0;
    }
    uid->str = str;
    /* TODO: do this properly */
    uid->uid = 0;
    return 0;
}
