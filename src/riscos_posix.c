/****************************************************************************
 * riscos_posix.c - POSIX compatibility for RISC OS (amnuts port)
 *
 * Provides: stat(), opendir(), readdir(), closedir(), fileno()
 * These are declared in TCPIPLibs headers but not implemented in unixlib.
 ***************************************************************************/

#ifdef __riscos

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#ifndef ENOENT
# define ENOENT   2
#endif
#ifndef EIO
# define EIO      5
#endif
#ifndef ENOMEM
# define ENOMEM  12
#endif
#ifndef ENOTDIR
# define ENOTDIR 20
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "kernel.h"
#include "swis.h"
#include "riscos_compat.h"

/* ---- Filesystem mode bits (if not already defined by sys/stat.h) ------- */
#ifndef S_IFMT
# define S_IFMT   0170000
# define S_IFREG  0100000
# define S_IFDIR  0040000
#endif
#ifndef S_ISDIR
# define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
# define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

/* ---- stat() -------------------------------------------------------------- */
/*
 * OS_File 5: read catalogue information
 *   In:  r0=5, r1=pathname
 *   Out: r0=0 not found, 1 file, 2 dir, 3 image
 *        r2=load, r3=exec, r4=length, r5=attributes
 */
int stat(const char *path, struct stat *buf)
{
    _kernel_swi_regs r;
    _kernel_oserror *err;

    r.r[0] = 5;
    r.r[1] = (int)path;
    err = _kernel_swi(OS_File, &r, &r);
    if (err) { errno = EIO; return -1; }
    if (r.r[0] == 0) { errno = ENOENT; return -1; }

    memset(buf, 0, sizeof(*buf));
    if (r.r[0] == 2) {
        buf->st_mode = (mode_t)(S_IFDIR | 0755);
    } else {
        buf->st_mode = (mode_t)(S_IFREG | 0644);
        buf->st_size = (long)r.r[4];
    }
    return 0;
}

/* ---- opendir() ----------------------------------------------------------- */
/*
 * DIR is typedef struct _dirdesc from TCPIPLibs <dirent.h>.
 * We use dd_buf for the directory path and dd_seek as the GBPB offset.
 */
DIR *opendir(const char *path)
{
    DIR *d;
    _kernel_swi_regs r;
    _kernel_oserror *err;

    r.r[0] = 5;
    r.r[1] = (int)path;
    err = _kernel_swi(OS_File, &r, &r);
    if (err || r.r[0] == 0) { errno = ENOENT; return NULL; }
    if (r.r[0] != 2) { errno = ENOTDIR; return NULL; }

    d = (DIR *)malloc(sizeof(DIR));
    if (!d) { errno = ENOMEM; return NULL; }
    memset(d, 0, sizeof(DIR));

    d->dd_buf = (char *)malloc(strlen(path) + 1);
    if (!d->dd_buf) { free(d); errno = ENOMEM; return NULL; }
    strcpy(d->dd_buf, path);
    d->dd_seek = 0;
    d->dd_fd   = -1;
    return d;
}

/* ---- readdir() ----------------------------------------------------------- */
/*
 * OS_GBPB 9: read directory entries (names only)
 *   In:  r0=9, r1=dir path, r2=buf, r3=count, r4=offset, r5=buflen, r6=0
 *   Out: r3=entries read, r4=next offset (-1 if end)
 * Buffer contains NUL-terminated filenames.
 * (OS_GBPB 10 returns full file info with the name at offset +20; use 9.)
 */
struct dirent *readdir(DIR *dir)
{
    static struct dirent entry;
    static char namebuf[256];
    _kernel_swi_regs r;
    _kernel_oserror *err;

    if (!dir || dir->dd_seek == -1) return NULL;

    r.r[0] = 9;
    r.r[1] = (int)dir->dd_buf;
    r.r[2] = (int)namebuf;
    r.r[3] = 1;
    r.r[4] = (int)dir->dd_seek;
    r.r[5] = (int)sizeof(namebuf);
    r.r[6] = 0;
    err = _kernel_swi(OS_GBPB, &r, &r);
    if (err || r.r[3] == 0) return NULL;

    dir->dd_seek = r.r[4];

    memset(&entry, 0, sizeof(entry));
    strncpy(entry.d_name, namebuf, sizeof(entry.d_name) - 1);
    entry.d_namlen = (unsigned char)strlen(entry.d_name);
    return &entry;
}

/* ---- closedir() ---------------------------------------------------------- */
int closedir(DIR *dir)
{
    if (!dir) return -1;
    if (dir->dd_buf) free(dir->dd_buf);
    free(dir);
    return 0;
}

/* ---- fileno() ------------------------------------------------------------ */
/*
 * The Norcroft FILE structure has __file as the OS file descriptor.
 * See the struct __FILE_struct definition in the Norcroft stdio.h.
 */
int fileno(FILE *fp)
{
    if (!fp) return -1;
    return fp->__file;
}

/* ---- uname() ------------------------------------------------------------- */
int uname(struct utsname *uts)
{
    const char *n;
    strncpy(uts->sysname,  "RISC OS", _SYS_UTSNAME_LENGTH - 1);
    strncpy(uts->machine,  "ARM",     _SYS_UTSNAME_LENGTH - 1);
    strncpy(uts->release,  "5",       _SYS_UTSNAME_LENGTH - 1);
    strncpy(uts->version,  "0",       _SYS_UTSNAME_LENGTH - 1);
    n = getenv("Inet$HostName");
    strncpy(uts->nodename, n ? n : "riscos", _SYS_UTSNAME_LENGTH - 1);
    return 0;
}

/* ---- Signal stubs -------------------------------------------------------- */
int sigemptyset(sigset_t *s) { (void)s; return 0; }

int sigaction(int sig, const struct sigaction *act, struct sigaction *old)
{
    (void)sig; (void)act; (void)old;
    return 0;
}

/* ---- fcntl stub ---------------------------------------------------------- */
int fcntl(int fd, int cmd, int arg) { (void)fd; (void)cmd; (void)arg; return 0; }

/* ---- riscos_yield() ------------------------------------------------------ */
void riscos_yield(void)
{
    _swix(OS_Byte, _INR(0,2), 19, 0, 0);
}

/* ---- strcasecmp / strncasecmp -------------------------------------------- */
int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2) {
        int d = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (d != 0) return d;
        ++s1; ++s2;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s2) {
        int d = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (d != 0) return d;
        ++s1; ++s2; --n;
    }
    if (!n) return 0;
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/* ---- strdup -------------------------------------------------------------- */
char *strdup(const char *s)
{
    size_t len;
    char *p;
    if (!s) return NULL;
    len = strlen(s) + 1;
    p = (char *)malloc(len);
    if (p) memcpy(p, s, len);
    return p;
}

#endif /* __riscos */
