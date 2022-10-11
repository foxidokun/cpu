start equ 1000

push start


deadins:
	call calc

	pop  rax
	push rax
	push rax
	push rax
	out

	push 7
	ja deadins
	halt


calc:
	zxc
	ret
