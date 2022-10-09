push 1000

inf_while:
	push 7
	sub
	pop  rax
	push rax
	out
	push rax

	push rax
	push 7
	ja inf_while

halt
