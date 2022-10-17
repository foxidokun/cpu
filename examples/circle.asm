width    equ 210
height   equ 54

radius^2 equ 200
center_x equ 105
center_y equ 27

color equ 1

; x -- rax
; y -- rbx

y_loop:
    push 0
    pop rax

x_loop:
        push rax
        push center_x
        sub
        push rax
        push center_x
        sub
        mul

        push rbx
        push center_y
        sub
        push rbx
        push center_y
        sub
        mul

        add

        push radius^2
        ja no_draw

        push rbx
        push width
        mul
        push rax
        add

        pop rcx
        push color
        pop [rcx]

no_draw:
        push rax
        inc
        pop rax

        push rax
        push width
        jb x_loop

    push rbx
    inc
    pop rbx

    push rbx
    push height
    jb y_loop

video
halt