.section .text
.global _start
_start:
	# __mlibc_entry(stack_pointer, main)
	# Push main (rightmost arg first in cdecl)
	push $main
	# Push stack pointer
	mov %esp, %eax
	push %eax
	call __mlibc_entry
	# Should never return; if it does, halt
	cli
	hlt

.section .note.GNU-stack,"",@progbits
