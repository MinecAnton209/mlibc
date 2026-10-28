#include <bits/syscall.h>
#include <errno.h>

extern "C" long __do_syscall_ret(unsigned long ret) {
	if ((long)ret < 0 && (long)ret > -4096) {
		errno = (int)(-(long)ret);
		return -1;
	}
	return (long)ret;
}

using sc_word_t = long;

static inline long int80h(long n, long a1, long a2, long a3, long a4, long a5) {
	register long ret asm("eax");
	register long arg1 asm("ebx") = a1;
	register long arg2 asm("ecx") = a2;
	register long arg3 asm("edx") = a3;
	register long arg4 asm("esi") = a4;
	register long arg5 asm("edi") = a5;
	register long num asm("eax") = n;
	asm volatile ("int $0x80"
		: "=r"(ret)
		: "r"(num), "r"(arg1), "r"(arg2), "r"(arg3),
		  "r"(arg4), "r"(arg5)
		: "memory");
	return ret;
}

sc_word_t __do_syscall0(long sc) {
	long dummy = 0;
	return int80h(sc, dummy, dummy, dummy, dummy, dummy);
}

sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	long dummy = 0;
	return int80h(sc, arg1, dummy, dummy, dummy, dummy);
}

sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	long dummy = 0;
	return int80h(sc, arg1, arg2, dummy, dummy, dummy);
}

sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	long dummy = 0;
	return int80h(sc, arg1, arg2, arg3, dummy, dummy);
}

sc_word_t __do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4) {
	long dummy = 0;
	return int80h(sc, arg1, arg2, arg3, arg4, dummy);
}

sc_word_t __do_syscall5(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
	return int80h(sc, arg1, arg2, arg3, arg4, arg5);
}

sc_word_t __do_syscall6(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
	(void)arg6;
	return int80h(sc, arg1, arg2, arg3, arg4, arg5);
}
