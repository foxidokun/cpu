push 1000

back:
	push 7
	jmp forward

forward:
	sub
	pop rax
; double arg for jb
	push rax
	push rax
	out
	push 0
; jump if 1000-i*7 > 0
	ja back
