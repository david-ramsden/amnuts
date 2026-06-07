# Amnuts RISC OS Port - Status Report

## Summary

Cross-platform port of Amnuts (amnuts2) targeting both Linux/UNIX (clang/gcc, C23)
and RISC OS (Norcroft 5.18, C90 with extensions, StubsG for C99 runtime).

**Both targets now build a complete amnuts binary.**

## Building

### Linux/UNIX (unchanged)
```sh
make
```

### RISC OS
```sh
cd RISCOS
riscos-amu -f Makefile,fe1 export         # build library (libamnuts)
riscos-amu -f MakefileAmnuts,fe1          # build command (amnuts,ff8)
```

The amnuts command's makefile sets `CLIBTYPE = generic` so the StubsG C
library is linked, giving C99 `snprintf` / `vsnprintf` at runtime.  Output is
`RISCOS/aif32/amnuts,ff8` (~520 KB).

## Cross-Platform Pattern

- `defines.h` is cross-platform: includes `riscos_compat.h` on RISC OS,
  selects `.` vs `/` as `DIRSEP`, and undefines `IDENTD` / `MANDNS` on RISC OS.
- `riscos_compat.h` provides stubs for missing POSIX surfaces under TCPIPLibs:
  `struct sigaction`, signal numbers, errno constants, fcntl flags, BSD
  visibility, `strcasecmp`/`strncasecmp`/`strdup`/`uname`/`fileno`.
- `riscos_posix.c` implements those stubs via RISC OS SWIs (`OS_File`,
  `OS_GBPB`) for `stat`/`opendir`/`readdir`/`closedir`, plus `fileno`, `uname`,
  `sigaction`/`sigemptyset`, `fcntl`, `riscos_yield` (cooperative scheduler via
  `OS_Byte 19`), and the string helpers.
- `riscos_crypt.c` provides a standalone DES-based `crypt()` so user password
  files are bit-identical between RISC OS and UNIX.

## Source Changes

- `__riscos` guards added in `amnuts.c`, `messages.c`, `reboot.c`, `admin.c`
  around `fork()`, `system()`, `execvp()`, `waitpid()`, `popen()`,
  `sigaction()`, `fcntl()`/`O_NONBLOCK`, with the RISC OS branches falling
  back to direct calls or `atexit()`-driven cleanup.  The main `select()`
  loop calls `riscos_yield()` on idle.
- Trailing commas in X-macro driven enums (`LVL_LIST`, `CT_LIST`, `CMD_LIST`,
  `SET_LIST`, `NLC_LIST`, `CONFIG_LIST`, `INITOPT_LIST`, `USERDB_LIST`) get
  a sentinel `*_NORCROFT_END` entry on RISC OS so Norcroft accepts the enum.
- C99 variadic `write_sock(...)` / `write_telnet(...)` dispatcher macros are
  bypassed on RISC OS; call sites use the explicit
  `write_sock_with_size` / `write_sock_with_size_and_flags` /
  `write_telnet_with_size` functions directly (works on both platforms).
- Vendor header include paths (`../vendors/.../sds.h`, `.../libtelnet.h`)
  switched on RISC OS to plain `"sds.h"` / `"libtelnet.h"` so the symlinks in
  `RISCOS/h/` resolve them.
- A few amnuts source files (`commands/cmdcount.c`, `commands/terminal.c`,
  `strings.c`) had their C99 mid-block declarations hoisted to the top of the
  enclosing block.

## Vendor Library Ports

### sds (`src/vendors/sds/`)
- `sds.h`:
  - `__attribute__((packed))` defined away (`-zps1` packs structures);
    flexible array members become `char buf[1]`; `inline` -> `__inline`.
  - `uint64_t` aliased to `uint32_t` on RISC OS (sdshdr64 is never
    instantiated at runtime because `sdsReqType` clamps to TYPE_32 when
    `LONG_MAX == LLONG_MAX` is false).
  - New `sds_llong`/`sds_ullong` typedefs (`long`/`unsigned long` on RISC
    OS, `long long`/`unsigned long long` elsewhere) plus matching
    `SDS_LLONG_MAX`/`SDS_LLONG_MIN` macros.
  - `va_copy` defined as a byte-wise `memcpy` of `va_list` storage.
- `sds.c`:
  - `sdsnewlen`'s mid-block declarations hoisted to top of function.
  - `long long` / `unsigned long long` parameter, local, and `va_arg` types
    in `sdsll2str` / `sdsull2str` / `sdsfromlonglong` / `sdscatvprintf`
    switched to `sds_llong` / `sds_ullong`.
  - Direct references to `LLONG_MAX` / `LLONG_MIN` replaced with
    `SDS_LLONG_MAX` / `SDS_LLONG_MIN`.

### libtelnet (`src/vendors/libtelnet/`)
- `libtelnet.c`:
  - On RISC OS, `__func__` is `#define`d to `"?"` (compiles, keeps error
    messages readable; Norcroft tracks call site via line number).
  - `va_copy` defined as a byte-wise `memcpy` of `va_list` storage.

## Build Tree

- `riscos-cport` generated `RISCOS/Makefile,fe1` (library) and
  `MakefileAmnuts,fe1` (command); `MakefileSds,fe1`/`MakefileAmncat,fe1`/
  `MakefileMove20x210,fe1` (auto-detected because of stray `main()`s and
  utilities) were removed.
- `${TCPIPLIBS}` added to `MakefileAmnuts,fe1` LIBS so the linker pulls in
  `unixlib`, `inetlib`, and `socklib5` for `gethostbyname`, `connect`, etc.
- `CLIBTYPE = generic` set in `MakefileAmnuts,fe1` so the C99 stubs
  (`stubsG`) are linked at the command-final step.

## Build Output

```
$ cd RISCOS
$ riscos-amu -f Makefile,fe1 export
amnuts: Export complete {libs}

$ riscos-amu -f MakefileAmnuts,fe1
amnuts: All built {Disc}

$ ls -l aif32
-rw-rw-r-- 1 david david 528512 Jun  7 16:30 aif32/amnuts,ff8
```

## Known Warnings (Norcroft)

These are non-fatal and come from the way TCPIPLibs and the vendor sources
were written for GCC:

- `Non-ANSI #include <sys/types.h>` (TCPIPLibs headers)
- `Undefined macro '__GNUC__' / '__STDC_VERSION__' in #if - treated as 0`
- `implicit narrowing cast` in sds string-building loops
- `lower precision in wider context` (pointer subtraction)

None affect correctness.
