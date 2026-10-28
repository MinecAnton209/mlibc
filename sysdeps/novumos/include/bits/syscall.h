#ifndef _MLIBC_SYSCALL_H
#define _MLIBC_SYSCALL_H

#include <errno.h>

#define SYS_EXIT        0
#define SYS_PRINTZ      1
#define SYS_GETCHAR     2
#define SYS_SETCURSOR   3
#define SYS_GETCURSOR   4
#define SYS_CLEAR       5
#define SYS_INB         6
#define SYS_OUTB        7
#define SYS_INW         8
#define SYS_OUTW        9
#define SYS_SLEEP       10
#define SYS_GETTICKS    11
#define SYS_JUMPTOUSER  12
#define SYS_SHUTDOWN    13
#define SYS_REBOOT      14
#define SYS_MEMMAPRANGE 15
#define SYS_INL         16
#define SYS_OUTL        17
#define SYS_DRAWCHARAT  18
#define SYS_GETDATETIME 19
#define SYS_IDEIDENTIFY 20
#define SYS_IDEREAD     21
#define SYS_IDEWRITE    22
#define SYS_MALLOC      30
#define SYS_FREE        31
#define SYS_CHECKCTRLC  32
#define SYS_IDTCHECK    33
#define SYS_IDTMOVE     34
#define SYS_EXECVE      40
#define SYS_YIELD       41
#define SYS_SPEAKER     42
#define SYS_WRITEBUF    43
#define SYS_GETFREEMEM  44
#define SYS_READFILE    45
#define SYS_WRITEFILE   46
#define SYS_DELFILE     47
#define SYS_RENFILE     48
#define SYS_STATFILE    49
#define SYS_GETRES      50
#define SYS_EXISTSFILE  51
#define SYS_COPYFILE    52
#define SYS_SHELLEXEC   53
#define SYS_SETCOLOR    54
#define SYS_OPEN        100
#define SYS_CLOSE       101
#define SYS_READ        102
#define SYS_WRITE       103
#define SYS_LSEEK       104
#define SYS_STAT        105
#define SYS_FSTAT       106
#define SYS_MMAP        107
#define SYS_MUNMAP      108
#define SYS_CLOCK_GETTIME 110
#define SYS_NANOSLEEP   111
#define SYS_GETPID      112
#define SYS_GETPPID     113
#define SYS_UNAME       114
#define SYS_FUTEX       120

#ifndef __cplusplus
#error "This header is currently C++ only"
#endif

extern "C" {
long __do_syscall0(long);
long __do_syscall1(long, long);
long __do_syscall2(long, long, long);
long __do_syscall3(long, long, long, long);
long __do_syscall4(long, long, long, long, long);
long __do_syscall5(long, long, long, long, long, long);
long __do_syscall6(long, long, long, long, long, long, long);
}

extern "C++" {
template<typename T>
inline long sc_cast(T x) {
	return (long)x;
}

template<typename... T>
inline long syscall(long sc, T... args) {
	static_assert(sizeof...(args) <= 6);
	constexpr long nargs = sizeof...(args);
	if constexpr (nargs == 0) {
		return __do_syscall0(sc);
	} else if constexpr (nargs == 1) {
		return __do_syscall1(sc, sc_cast(args)...);
	} else if constexpr (nargs == 2) {
		return __do_syscall2(sc, sc_cast(args)...);
	} else if constexpr (nargs == 3) {
		return __do_syscall3(sc, sc_cast(args)...);
	} else if constexpr (nargs == 4) {
		return __do_syscall4(sc, sc_cast(args)...);
	} else if constexpr (nargs == 5) {
		return __do_syscall5(sc, sc_cast(args)...);
	} else if constexpr (nargs == 6) {
		return __do_syscall6(sc, sc_cast(args)...);
	}
}

}

#endif /* _MLIBC_SYSCALL_H */
