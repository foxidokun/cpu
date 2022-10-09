; Variables
start equ 1000

push start

deadins:
	zxc
	pop  rax
	push rax
	push rax
	push rax
	out
	push 7
	ja deadins

halt
