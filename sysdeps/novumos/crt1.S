.section .text
.global _start
_start:
	# __mlibc_entry(stack_pointer, main): the stack pointer must reference argc,
	# so it is taken before main is pushed.
	mov %esp, %eax
	push $main
	push %eax
	call __mlibc_entry
	# Should never return; if it does, halt
	cli
	hlt

.section .note.GNU-stack,"",@progbits
