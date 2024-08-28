.code

ZwAllocateVirtualMemory proc
	mov r10, rcx
	mov eax, 18h
	test byte ptr [7FFE0308h], 1h
	jnz short isHyperVisor
	syscall                
	ret
isHyperVisor:                       
	int 2Eh             
	ret
ZwAllocateVirtualMemory endp

ZwFreeVirtualMemory proc
	mov r10, rcx
	mov eax, 1Eh
	test byte ptr [7FFE0308h], 1h
	jnz short isHyperVisor
	syscall
	ret
isHyperVisor:
	int 2Eh
	ret
ZwFreeVirtualMemory endp

ZwQueryVirtualMemory proc
	mov r10, rcx
	mov eax, 23h
	test byte ptr [7FFE0308h], 1h
	jnz short isHyperVisor
	syscall
	ret
isHyperVisor:
	int 2Eh
	ret
ZwQueryVirtualMemory endp

end