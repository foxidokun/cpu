float_mult equ 1000

; //Loop (for (rax = 3; rax > 0; ++rax) {scanf})
push 3
pop rax

read_inp:
    inp
    push float_mult
    mul

; // Dec rax
    push rax
    dec
    pop rax

; // Loop if i > 0
    push rax
    push 0
    ja read_inp

; // Stack: c - b - a
; // Load registers
    pop rcx
    pop rbx
    pop rax

;// Jump if a == 0
    push rax
    push 0
    je zero_a

;// Calc D (b^2 + (-1)(4ac))
    push rbx
    push rbx
    mul
    push rax
    push rcx
    push 4
    push -1
    mul
    mul
    mul
    add
    pop  rdx
    push rdx

;// D == 0
    push rdx
    push 0
    je zero_d

;// D < 0
    push rdx
    push 0
    jb bright_future

;// D => sqrt(D)/2a
    sqrt
    push 2
    push rax
    mul
    call divf
    pop rdx

;// Calc -b/2a

    push -1
    push rbx
    mul
    push 2
    push rax
    mul
    call divf

;// rax = -b/2a
    pop rax

    push 2
    out

    push rax
    push rdx
    sub
    call print

    push rax
    push rdx
    add
    call print

    jmp bright_future

zero_a:
    push rbx
    push 0
    je zero_b

    push 1
    out

    push rcx
    push -1
    mul
    push rbx
    call divf

    call print
    jmp bright_future

zero_b:
    push rcx
    push 0
    je inf_roots
    push 0
    out
    jmp bright_future

inf_roots:
    push -1
    out
    jmp bright_future

zero_d:
    push -1
    push rbx
    mul

    push 2
    push rax
    mul

    call divf

    push 1
    out

    call print
    jmp bright_future

print:
    out
    ret

divf:
    pop rex
    push float_mult
    mul
    push rex
    div
    ret

bright_future:
    halt
