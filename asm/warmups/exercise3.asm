.psx
.create "exercise3.bin", 0x80010000
.org 0x80010000
Main:
	li t0, 0x1B; 27
	li t1, 0x3
	li t2, 0
	blt t0, t1, End
	nop
	
Loop:
	sub t0, t0, t1
	addi t2, t2, 0x1
	bge t0, t1, Loop
	nop

End:
.close