#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gid.h"
#include "mode.h"
#include "uid.h"

/*
 *  plant - similar to install(1) (from BSD), but more down-to-earth
 */

static int usage(FILE *file, int ret)
{
    /*
     *             0         1         2         3         4         5         6         7
     *             01234567890123456789012345678901234567890123456789012345678901234567890123456789
     */
    fprintf(file, "Usage: plant [option] <file> ... <fileN>\n");
    fprintf(file, "  where [option] can be:\n");
    fprintf(file, "  -D <destdir>  <destdir> is the top of the file hierarchy that the items are\n");
    fprintf(file, "                put in to.  If the -M option is supplied, the leading string\n");
    fprintf(file, "                of <destdir> will be removed from the file names logged.\n");
    fprintf(file, "  -d            create directories.  Missing parent directories are created\n");
    fprintf(file, "                as required.\n");
    fprintf(file, "  -g <gid>      set group <gid> like chgrp(1).\n");
    fprintf(file, "  -h            This usage statement.\n");
    fprintf(file, "  -H <hash>     when copying a file, calcualte the digest of the file using\n");
    fprintf(file, "                the <hash> algorithm.  Supported <hash> algorithms include\n");
    fprintf(file, "                crc32, md5, and sha1.  This option may supplied multiple times\n");
    fprintf(file, "                to request multiple digest algorithms.  The hash is stored in\n");
    fprintf(file, "                the <mfile> if the -M option is supplied.\n");
    fprintf(file, "  -l <lflags>   create a link to the source, where the flags are:\n");
    fprintf(file, "                'a' (absolute), 'r' (relative), 's' soft, 'h' hard.\n");
    fprintf(file, "  -M <mfile>    record the metadata associated with each item into <mfile> as\n");
    fprintf(file, "                an mtree(8) formatted file.\n");
    fprintf(file, "  -m <mode>     set permission mode to <mode> like chmod(1).\n");
    fprintf(file, "  -o <uid>      set user to <uid> like chown(1).\n");
    fprintf(file, "  -s            strip symbol tables.  Uses the environment variable STRIPBIN\n");
    fprintf(file, "                as the command if present in the environment, otherwise it\n");
    fprintf(file, "                invokes the command $(CROSS_COMPILE)strip.\n");
    fprintf(file, "  -T <tag>      specify the tag to write out if the -M option is supplied.\n");
    fprintf(file, "                Repeat the -T <tag> option for multiple tags\n");
    fprintf(file, "\n");
    fprintf(file, " When copying a file, plant will always copy to a temporary file first, and\n");
    fprintf(file, "then atomically renamed in place.  If the program is interrupted, the\n");
    fprintf(file, "temporary files may remain.\n");
    return ret;
}

#define PLANT_FLAGS_DESTDIR    (1<<0)
#define PLANT_FLAGS_DIRMODE    (1<<1)
#define PLANT_FLAGS_GID        (1<<2)
#define PLANT_FLAGS_HASH       (1<<3)
#define PLANT_FLAGS_LINKMODE   (1<<4)
#define PLANT_FLAGS_MTREE      (1<<5)
#define PLANT_FLAGS_MODE       (1<<6)
#define PLANT_FLAGS_UID        (1<<7)
#define PLANT_FLAGS_STRIP      (1<<8)
#define PLANT_FLAGS_TAGS       (1<<9)

#define PLANT_HASH_CRC32       (1<<0)
#define PLANT_HASH_MD5         (1<<1)
#define PLANT_HASH_SHA1        (1<<2)

struct plant {
    unsigned int flags;
    const char *destdir;
    struct plant_gid *gid;
    unsigned int hash;
    const char *lflags;
    const char *mtree;
    struct plant_mode *mode;
    struct plant_uid *uid;
    const char *tags;
};

static int parse_opts(struct plant *plant, int argc, char *argv[])
{
    int c;

    if (!plant) {
        return -1;
    }

    optind = 1;
    while ((c=getopt(argc, argv, "?D:dg:h:l:M:m:o:sT:")) != -1) {
        switch(c) {
            case 'D':
                plant->flags |= PLANT_FLAGS_DESTDIR;
                plant->destdir = optarg;
                break;

            case 'd':
                plant->flags |= PLANT_FLAGS_DIRMODE;
                break;

            case 'g':
                plant->flags |= PLANT_FLAGS_GID;
                plant->gid = str_to_plant_gid(plant->gid, optarg);
                if (!plant->gid) {
                    plant->flags &= ~PLANT_FLAGS_GID; /* clear old flag */
                    return usage(stderr, -3);
                }
                break;

            case 'h':
                return usage(stdout, 1);
                break;

            case 'l':
                plant->flags |= PLANT_FLAGS_LINKMODE;
                plant->lflags = optarg;
                break;

            case 'M':
                plant->flags |= PLANT_FLAGS_MTREE;
                plant->mtree = optarg;
                break;

            case 'm':
                plant->flags |= PLANT_FLAGS_MODE;
                plant->mode = str_to_plant_mode(plant->mode, optarg);
                if (!plant->mode) {
                    plant->flags &= ~PLANT_FLAGS_MODE; /* clear old flag */
                    return usage(stderr, -3);
                }
                break;

            case 'o':
                plant->flags |= PLANT_FLAGS_UID;
                plant->uid = str_to_plant_uid(plant->uid, optarg);
                if (!plant->uid) {
                    plant->flags &= ~PLANT_FLAGS_UID; /* clear old flag */
                    return usage(stderr, -3);
                }
                break;

            case 's':
                plant->flags |= PLANT_FLAGS_STRIP;
                break;

            case 'T':
                plant->flags |= PLANT_FLAGS_TAGS;
                plant->tags = optarg;
                break;

            default:
                if (('?' == c) && ('?' == optopt)) { /* alias to 'h' */
                    return usage(stdout, 0);
                }
                return usage(stderr, -1);
                break;
        }
    }
    if (argc <= optind)
        return usage(stdout, -2);
    return optind;
}

int main(int argc, char *argv[])
{
    int i, r;
    const char *str;
    struct plant plant;

    memset(&plant, 0, sizeof(plant));

    r = parse_opts(&plant, argc, argv);
    if (r < 1) {
        return -r;
    }

    if (plant.flags & PLANT_FLAGS_DIRMODE) {
        for (i=r; i<argc; i++) {
            fprintf(stdout,"mkdir %s\n", argv[i]);
        }
    } else {
        if ((argc - r) < 2) {
            return usage(stderr, 3);
        }
        if (plant.flags & PLANT_FLAGS_LINKMODE) {
            str = "ln";
        } else {
            str = "cp";
        }
        for (i=r; i<argc-1; i++) {
            fprintf(stdout,"%s %s --> %s\n", str, argv[i], argv[argc-1]);
        }
    }
    return 0;
}
