color equ 1

push color
push color
push color
push color
pop [0]
pop [1]
pop [2]
pop [3]

push 0
pop  rax

loop:
    push 0
    pop [rax]

; inc rax
    push rax
    inc
    pop  rax

    push color
    pop [rax+3]

    video

    push rax
    push 100
    jb no_reset

    push 0
    push 0
    push 0
    push 0
    pop [rax]
    pop [rax+1]
    pop [rax+2]
    pop [rax+3]
    push 0
    pop  rax

no_reset:
    jmp loop

