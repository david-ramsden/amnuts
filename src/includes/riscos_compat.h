/*
 * RISC OS compatibility shims for amnuts.
 * Included by defines.h when __riscos is defined.
 *
 * Keep this header minimal: TCPIPLibs already provides pid_t, getpid(),
 * the socket types, and many POSIX declarations.  Only provide things
 * that are genuinely absent on RISC OS.
 */

#ifndef AMNUTS_RISCOS_COMPAT_H
#define AMNUTS_RISCOS_COMPAT_H

#ifdef __riscos

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* ---- Integer types ----------------------------------------------- */
/* Provide unsigned variants that TCPIPLibs may not supply. */
#ifndef AMNUTS_UINT_TYPES
# define AMNUTS_UINT_TYPES
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
#endif

/* socklen_t: use int if the system doesn't define it */
#ifndef __socklen_t_defined
# define __socklen_t_defined
typedef int socklen_t;
#endif

/* ---- uname / utsname stub --------------------------------------- */

#define _SYS_UTSNAME_LENGTH 65

struct utsname {
    char sysname[_SYS_UTSNAME_LENGTH];
    char nodename[_SYS_UTSNAME_LENGTH];
    char release[_SYS_UTSNAME_LENGTH];
    char version[_SYS_UTSNAME_LENGTH];
    char machine[_SYS_UTSNAME_LENGTH];
};

extern int uname(struct utsname *uts);

/* ---- Signals ----------------------------------------------------- */
/*
 * RISC OS does not have POSIX signals.  Provide stub constants and
 * no-op implementations so the code compiles under RISC OS.
 */

#ifndef SIGTERM
# define SIGTERM  15
#endif
#ifndef SIGHUP
# define SIGHUP    1
#endif
#ifndef SIGPIPE
# define SIGPIPE  13
#endif
#ifndef SIGINT
# define SIGINT    2
#endif
#ifndef SIGQUIT
# define SIGQUIT   3
#endif
#ifndef SIGABRT
# define SIGABRT   6
#endif
#ifndef SIGILL
# define SIGILL    4
#endif
#ifndef SIGFPE
# define SIGFPE    8
#endif
#ifndef SIGSEGV
# define SIGSEGV  11
#endif
#ifndef SIGBUS
# define SIGBUS   10
#endif

#ifndef SIG_DFL
# define SIG_DFL  ((void (*)(int))0)
#endif
#ifndef SIG_IGN
# define SIG_IGN  ((void (*)(int))1)
#endif

struct sigaction {
    void (*sa_handler)(int);
    int   sa_flags;
    int   sa_mask;  /* simplified; POSIX uses sigset_t */
};

typedef int sigset_t;

extern int sigemptyset(sigset_t *s);
extern int sigaction(int sig, const struct sigaction *act,
                     struct sigaction *old);

/* ---- POSIX errno constants --------------------------------------- */
/*
 * The Norcroft errno.h declares 'errno' but not the constants.
 * Provide BSD/POSIX values that are compatible with the TCPIPLibs runtime.
 */
#ifndef EPERM
# define EPERM    1
#endif
#ifndef ENOENT
# define ENOENT   2
#endif
#ifndef EINTR
# define EINTR    4
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
#ifndef ECONNRESET
# define ECONNRESET  54
#endif
#ifndef ETIMEDOUT
# define ETIMEDOUT   60
#endif

/* ---- Socket shutdown constants ----------------------------------- */
/* TCPIPLibs may not define SHUT_* constants; provide them here. */
#ifndef SHUT_RD
# define SHUT_RD   0
#endif
#ifndef SHUT_WR
# define SHUT_WR   1
#endif
#ifndef SHUT_RDWR
# define SHUT_RDWR 2
#endif

/* ---- fcntl stubs ------------------------------------------------- */
/*
 * <fcntl.h> is not provided by TCPIPLibs.  The only fcntl usage in
 * amnuts is to set O_NONBLOCK on listening sockets, which is guarded
 * by #ifndef __riscos in amnuts.c.  Provide the constants as stubs
 * so the header compiles even if the guard is missed.
 */
#define O_NONBLOCK 0x4000
#define F_GETFL    3
#define F_SETFL    4

extern int fcntl(int fd, int cmd, int arg);

/* ---- OS_Byte 19 cooperative yield -------------------------------- */
/*
 * On RISC OS, OS_Byte 19 waits for the next vertical sync interrupt,
 * yielding control to the cooperative multitasking system.
 * Call this in the main select loop when there is nothing to do.
 */
extern void riscos_yield(void);

/* ---- fileno() ---------------------------------------------------- */
/* Norcroft stdio.h does not declare fileno(); implemented in riscos_posix.c */
extern int fileno(FILE *fp);

/* ---- BSD visibility flag ----------------------------------------- */
/*
 * TCPIPLibs' netinet/in.h guards IPv6 extensions with __BSD_VISIBLE.
 * Define it so Norcroft does not warn "Undefined macro in #if".
 */
#ifndef __BSD_VISIBLE
# define __BSD_VISIBLE 1
#endif

/* ---- strcasecmp / strncasecmp ------------------------------------ */
/*
 * The Norcroft C library does not provide strcasecmp/strncasecmp.
 */
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);

/* ---- strdup ----------------------------------------------------- */
/* Norcroft does not provide strdup; supplied by riscos_posix.c. */
extern char *strdup(const char *s);

#endif /* __riscos */
#endif /* AMNUTS_RISCOS_COMPAT_H */
