float_mult equ 100000

; //Loop with inp (for rax = 3; rax > 0; ++rax)
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

    push 0
    je zero_d

    push rdx
    sqrt
    push 2
    push rax
    mul
    div
    pop rdx

    push -1
    push rbx
    mul
    push 2
    push rax
    mul
    div
;// Destroy rax with -b/2a
    pop rax

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
    push rcx
    push -1
    mul
    push rbx
    div

    call print
    jmp bright_future

zero_d:
    push -1
    push rbx
    mul

    push 2
    push rax
    mul

    div

    call print
    jmp bright_future

print:
    pop  rex
    push rex

    push rex
    push float_mult
    div

    pop  rex
    push rex
    push rex
    out

    push float_mult
    mul
    sub
    out
    ret

bright_future:
    halt
