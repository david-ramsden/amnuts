# RISC OS Port Report

## Overview

- Project: `amnuts`
- Type: `library` (library port selected for this run)
- Source root: `.`
- Output tree: `RISCOS`

## Name And Type

- Inferred name: `amnuts` (explicit --name override, confidence: high)
- Project mode: library port

## Sources And Entry Points

- Source files: 183
- src/admin.c
- src/games.c
- src/messages.c
- src/netlinks.c
- src/objects.c
- src/pager.c
- src/reboot.c
- src/riscos_crypt.c
- src/riscos_posix.c
- src/rooms.c
- src/speech.c
- src/spodlist.c
- src/strings.c
- src/telnet.c
- src/commands/account.c
- src/commands/accreq.c
- src/commands/addhistory.c
- src/commands/afk.c
- src/commands/allclones.c
- src/commands/arrest.c
- src/commands/ban.c
- src/commands/bcast.c
- src/commands/beep.c
- src/commands/bring.c
- src/commands/cafk.c
- src/commands/calendar.c
- src/commands/cedit.c
- src/commands/change_pass.c
- src/commands/charecho.c
- src/commands/clearline.c
- src/commands/clone_emote.c
- src/commands/clone_hear.c
- src/commands/clone_say.c
- src/commands/clone_switch.c
- src/commands/cls.c
- src/commands/cmdcount.c
- src/commands/colour.c
- src/commands/connect_netlink.c
- src/commands/copyto.c
- src/commands/create_clone.c
- src/commands/cshouts.c
- src/commands/ctells.c
- src/commands/ctopic.c
- src/commands/delete_suggestions.c
- src/commands/delete_user.c
- src/commands/demote.c
- src/commands/destroy_clone.c
- src/commands/disconnect_netlink.c
- src/commands/display.c
- src/commands/dmail.c
- src/commands/dump.c
- src/commands/echo.c
- src/commands/emote.c
- src/commands/enter_profile.c
- src/commands/examine.c
- src/commands/expires.c
- src/commands/fix.c
- src/commands/flagged.c
- src/commands/fmail.c
- src/commands/force.c
- src/commands/friend_emote.c
- src/commands/friend_say.c
- src/commands/friends.c
- src/commands/fsmail.c
- src/commands/gcom.c
- src/commands/go.c
- src/commands/greet.c
- src/commands/grepusers.c
- src/commands/help.c
- src/commands/history.c
- src/commands/home.c
- src/commands/ignlist.c
- src/commands/ignuser.c
- src/commands/invite.c
- src/commands/join.c
- src/commands/kill_user.c
- src/commands/last.c
- src/commands/letmein.c
- src/commands/listbans.c
- src/commands/listen.c
- src/commands/lmail.c
- src/commands/logging.c
- src/commands/look.c
- src/commands/macros.c
- src/commands/mail_from.c
- src/commands/minlogin.c
- src/commands/mkinvis.c
- src/commands/mkvis.c
- src/commands/morph.c
- src/commands/move.c
- src/commands/mutter.c
- src/commands/muzzle.c
- src/commands/myclones.c
- src/commands/myroom.c
- src/commands/myroom_admin.c
- src/commands/myroom_bgone.c
- src/commands/myroom_key.c
- src/commands/myroom_lock.c
- src/commands/myroom_paint.c
- src/commands/myroom_rename.c
- src/commands/netdata.c
- src/commands/netstat.c
- src/commands/pemote.c
- src/commands/picture_all.c
- src/commands/picture_tell.c
- src/commands/plead.c
- src/commands/preview.c
- src/commands/promote.c
- src/commands/promote_temporary.c
- src/commands/purge.c
- src/commands/quick_call.c
- src/commands/ranks.c
- src/commands/rcountu.c
- src/commands/read_board.c
- src/commands/reboot_talker.c
- src/commands/recount.c
- src/commands/reload_room.c
- src/commands/reminder.c
- src/commands/remote_stat.c
- src/commands/resite.c
- src/commands/retire.c
- src/commands/revafk.c
- src/commands/revclr.c
- src/commands/revedit.c
- src/commands/review.c
- src/commands/revshout.c
- src/commands/revtell.c
- src/commands/rmail.c
- src/commands/room.c
- src/commands/samesite.c
- src/commands/say.c
- src/commands/sayto.c
- src/commands/search_boards.c
- src/commands/semote.c
- src/commands/set.c
- src/commands/set_desc.c
- src/commands/set_ignore.c
- src/commands/set_iophrase.c
- src/commands/set_room_access.c
- src/commands/set_topic.c
- src/commands/setcmdlev.c
- src/commands/sfrom.c
- src/commands/shackle.c
- src/commands/shout.c
- src/commands/show.c
- src/commands/show_version.c
- src/commands/shutdown_com.c
- src/commands/sing_it.c
- src/commands/site.c
- src/commands/smail.c
- src/commands/sreboot.c
- src/commands/status.c
- src/commands/sto.c
- src/commands/suggestions.c
- src/commands/suicide.c
- src/commands/system.c
- src/commands/tell_user.c
- src/commands/terminal.c
- src/commands/think_it.c
- src/commands/time.c
- src/commands/toggle_ignall.c
- src/commands/toggle_mode.c
- src/commands/toggle_prompt.c
- src/commands/toggle_swearban.c
- src/commands/unarrest.c
- src/commands/unban.c
- src/commands/uninvite.c
- src/commands/unmuzzle.c
- src/commands/unretire.c
- src/commands/unshackle.c
- src/commands/verify.c
- src/commands/viewlog.c
- src/commands/visibility.c
- src/commands/visit.c
- src/commands/wake.c
- src/commands/who.c
- src/commands/wipe_board.c
- src/commands/wizemote.c
- src/commands/wizlist.c
- src/commands/wizshout.c
- src/commands/write_board.c
- src/commands/xcom.c
- src/vendors/libtelnet/libtelnet.c

- Entry point sources: 4
  - `src/amnuts.c` -> command source (contains main())
  - `src/vendors/sds/sds.c` -> command source (contains main())
  - `utils/amncat.c` -> command source (contains main())
  - `utils/move20x_210.c` -> command source (contains main())

## Headers And Makefiles

- Exported headers:
  - None

- Compile-time headers:
  - src/includes/commands.h
  - src/includes/defines.h
  - src/includes/globals.h
  - src/includes/netlinks.h
  - src/includes/prototypes.h
  - src/includes/riscos_compat.h
  - src/includes/rooms.h
  - src/includes/spodlist.h
  - src/includes/telnet.h
  - src/vendors/sds/sds.h
  - src/vendors/sds/sdsalloc.h
  - src/vendors/libtelnet/libtelnet.h

- Generated command makefiles:
  - MakefileAmnuts,fe1
  - MakefileSds,fe1
  - MakefileAmncat,fe1
  - MakefileMove20x210,fe1

## Build Commands

- Run these commands from the source root to build each component:
  - cd RISCOS && riscos-amu export
  - cd RISCOS && riscos-amu -f MakefileAmnuts,fe1
  - cd RISCOS && riscos-amu -f MakefileSds,fe1
  - cd RISCOS && riscos-amu -f MakefileAmncat,fe1
  - cd RISCOS && riscos-amu -f MakefileMove20x210,fe1

## External Libraries

- crypt (-lcrypt)
- nsl (-lnsl)
- socket (-lsocket)
- zlib (zlib.h / -lz)

## Portability Warnings

- src/admin.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/messages.c: uses process creation/wait APIs such as fork() or waitpid()
- src/messages.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/netlinks.c: uses designated initialisers
- src/netlinks.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/objects.c: uses designated initialisers
- src/reboot.c: uses designated initialisers
- src/reboot.c: uses process creation/wait APIs such as fork() or waitpid()
- src/reboot.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/riscos_posix.c: uses designated initialisers
- src/riscos_posix.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/riscos_posix.c: includes Unix/POSIX headers that usually need replacement on RISC OS
- src/telnet.c: uses designated initialisers
- src/commands/morph.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/commands/recount.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/commands/reminder.c: uses designated initialisers
- src/vendors/libtelnet/libtelnet.c: uses designated initialisers
- src/includes/defines.h: includes Unix/POSIX headers that usually need replacement on RISC OS
- src/vendors/sds/sds.h: uses 64-bit integer types that are not portable to 32-bit Norcroft
- src/vendors/sds/sds.h: includes Unix/POSIX headers that usually need replacement on RISC OS
- src/amnuts.c: uses designated initialisers
- src/amnuts.c: uses process creation/wait APIs such as fork() or waitpid()
- src/amnuts.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- src/vendors/sds/sds.c: uses 64-bit integer types that are not portable to 32-bit Norcroft
- src/vendors/sds/sds.c: uses C99-style for-loop declarations
- utils/amncat.c: includes Unix/POSIX headers that usually need replacement on RISC OS
- utils/move20x_210.c: uses Unix file/process APIs such as open(), read(), stat(), or lseek()
- utils/move20x_210.c: includes Unix/POSIX headers that usually need replacement on RISC OS

## Confidence Notes

- Project name: high confidence from explicit --name override.
- Source discovery: autodiscovered source directory.
- Entry points: high confidence because the scanner only classifies sources that contain main().
- Dependencies: medium confidence because they are inferred from headers and link flags.
- Portability warnings: high confidence because they come from direct pattern matches in the source.
- Command makefiles: high confidence because they were generated from each discovered command source.

