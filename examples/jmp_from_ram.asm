push 1000

push deadins
pop  rbx

jmp deadins

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
