push 1000

deadins:
	push 7
	sub
	pop rax
	push rax
	push rax
	push rax
	out
	push 7
	ja deadins

halt
