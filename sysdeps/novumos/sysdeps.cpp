#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/syscall.h>
#include <abi-bits/vm-flags.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <stdint.h>

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc panic!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	// Write to stderr (fd 2) via syscall
	ssize_t unused;
	syscall(SYS_WRITE, 2, msg, strlen(msg), &unused);
}

int Sysdeps<Isatty>::operator()(int fd) {
	// Only the standard streams are terminals; other descriptors are not.
	return (fd >= 0 && fd <= 2) ? 0 : ENOTTY;
}

int Sysdeps<Write>::operator()(int fd, const void *buf, size_t size, ssize_t *bytes_written) {
	auto ret = syscall(SYS_WRITE, fd, buf, size);
	if (ret < 0)
		return -(int)ret;
	*bytes_written = ret;
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	auto ret = syscall(SYS_READ, fd, buf, count);
	if (ret < 0)
		return -(int)ret;
	*bytes_read = ret;
	return 0;
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	(void)mode;
	auto ret = syscall(SYS_OPEN, pathname, flags, 0);
	if (ret < 0)
		return -(int)ret;
	*fd = (int)ret;
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	auto ret = syscall(SYS_CLOSE, fd);
	if (ret < 0)
		return -(int)ret;
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto ret = syscall(SYS_LSEEK, fd, offset, whence);
	if (ret < 0)
		return -(int)ret;
	*new_offset = ret;
	return 0;
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	auto ret = syscall(SYS_CLOCK_GETTIME, clock, secs);
	if (ret < 0)
		return -(int)ret;
	// secs and nanos are filled by kernel; but we used the libc struct, not kernel struct
	// Actually the kernel fills a struct timespec directly into secs pointer
	// For our kernel, clock_gettime fills (tv_sec, tv_nsec) which is (time_t, long)
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	// Use mmap with MAP_ANONYMOUS for page-aligned allocation
	auto ret = syscall(SYS_MMAP, 0, size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret < 0 || ret == (long)MAP_FAILED)
		return ENOMEM;
	*pointer = (void *)ret;
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	auto ret = syscall(SYS_MUNMAP, (long)pointer, size);
	if (ret < 0)
		return -(int)ret;
	return 0;
}

int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	// The kernel maps anonymous memory only; file-backed mappings are not supported.
	if (!(flags & MAP_ANONYMOUS) || fd != -1)
		return ENODEV;
	auto ret = syscall(SYS_MMAP, (long)hint, size, prot, flags, fd, offset);
	if (ret < 0 || ret == (long)MAP_FAILED) {
		*window = nullptr;
		return ENOMEM;
	}
	*window = (void *)ret;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	auto ret = syscall(SYS_MUNMAP, (long)pointer, size);
	if (ret < 0)
		return -(int)ret;
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	if (static_cast<unsigned long>(syscall(SYS_SET_TLS_BASE, reinterpret_cast<uintptr_t>(pointer))) == 0xFFFFFFFFul)
		return EFAULT;
	// The kernel also sets the selector on return; reloading here keeps the cached descriptor fresh.
	asm volatile("mov $0xB3, %%ax\n\tmov %%ax, %%gs" ::: "ax");
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer);
	asm volatile("mov %0, %%gs:0" ::"r"(thread_data));
	return 0;
}

void Sysdeps<Exit>::operator()(int status) {
	syscall(SYS_EXIT, status);
	__builtin_unreachable();
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	(void)time;
	while (__atomic_load_n(pointer, __ATOMIC_RELAXED) == expected) {
		__builtin_ia32_pause();
	}
	return 0;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	(void)pointer;
	(void)all;
	return 0;
}

} // namespace mlibc
