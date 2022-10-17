;// a1,a2 = 1
a1_2    	  equ 1
;// Cache
cache_enabled equ 1
cache_hz 	  equ 1

;// Get n
inp

main:
	call fib
	out
	halt

fib:
	pop  rax
	push rax

;// Return value if n <= 2
	push rax
	push 2
	jbe  happy_route

;// Check if cache can exist
	push rax
	call check_for_caching

;// If can't
	push 0
	je no_ram_load

;// Use cache if it exists
	push [rax]
	push 0
	jne return_from_ram

no_ram_load:

;// Decrement n => (n-1)
	dec
	pop  rax
	push rax

;// Call fib func

	push rax
	call fib

;// Pop res and n (n-1), change order and push
	pop  rax
	pop  rbx
	push rax
	push rbx

;// Dec n => (n-2)
	dec

;// Duplicate n (n-2) on head 
	pop  rax
	push rax
	push rax

;// Call fib
	call fib

;// Pop res (f(n-2)), n (n-2), push res f(n-2) and sum with f(n-1)
	pop  rax
	pop  rbx
	push rax
	add

;//	n-2 => n
	push rbx
	push 2
	add

;// pop n to rax
	pop  rax

;// Double f(n)
	pop  rbx
	push rbx
	push rbx

;// Load f(n) into RAM
	pop [rax]
	ret

happy_route:
	pop rax
	push a1_2
	ret

return_from_ram:
; cache hit indicator
	push 1337
	out

; Remove n from stack and return
	pop rdx
	push [rax]
	ret

check_for_caching:
;// USE ONLY rdx
;// Check for i = 0 mod 3
;// what is equal
;// i =?= (i/3)*3

	push cache_enabled
	push 0
    je skip_cache

	pop  rdx
	push rdx

	push rdx
	push cache_hz
	div
	push cache_hz
	mul

	je push_true

	push 0
	ret

push_true:
	push 1
	ret

skip_cache:
	pop rdx
	push 0
    ret
