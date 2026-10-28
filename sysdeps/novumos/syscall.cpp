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

// NovumOS has no int 0x80 gate: syscalls enter via SYSENTER with the SDK stub layout
// (stack [EFLAGS][0][0] and ECX = that stack pointer). The kernel reads four arguments
// (EBX, ESI, EDI, EBP), so a5 is not forwarded.
static inline long sysenter_call(long n, long a1, long a2, long a3, long a4, long a5) {
	(void)a5;
	long ret;
	asm volatile (
		"pushl %%ebp\n\t"
		"movl %[a4], %%ebp\n\t"
		"pushl $0\n\t"
		"pushl $0\n\t"
		"pushfl\n\t"
		"movl %%esp, %%ecx\n\t"
		"call 1f\n"
		"1:\n\t"
		"popl %%edx\n\t"
		"sysenter\n\t"
		"popl %%ebp\n\t"
		: "=a"(ret)
		: "a"(n), "b"(a1), "S"(a2), "D"(a3), [a4] "m"(a4)
		: "ecx", "edx", "ebp", "memory"
	);
	return ret;
}
sc_word_t __do_syscall0(long sc) {
	long dummy = 0;
	return sysenter_call(sc, dummy, dummy, dummy, dummy, dummy);
}

sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	long dummy = 0;
	return sysenter_call(sc, arg1, dummy, dummy, dummy, dummy);
}

sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	long dummy = 0;
	return sysenter_call(sc, arg1, arg2, dummy, dummy, dummy);
}

sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	long dummy = 0;
	return sysenter_call(sc, arg1, arg2, arg3, dummy, dummy);
}

sc_word_t __do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4) {
	long dummy = 0;
	return sysenter_call(sc, arg1, arg2, arg3, arg4, dummy);
}

sc_word_t __do_syscall5(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
	return sysenter_call(sc, arg1, arg2, arg3, arg4, arg5);
}

sc_word_t __do_syscall6(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
	(void)arg6;
	return sysenter_call(sc, arg1, arg2, arg3, arg4, arg5);
}
