.psx
.create "factorial.bin", 0x80010000

.org 0x80010000

Main:
	li t0, 5
	li t3, 1
	li t4, 1
	li t1, 1
	bgt t1, t0, End
	nop

OuterLoop:
	li t4, 0
	li t2, 0
	bge t2, t1, OuterLoop2
	nop
	
InnerLoop:
	addu t4, t3
	addi t2, 1
	blt t2, t1, InnerLoop
	nop

OuterLoop2:
	move t3, t4
	addi t1, 1
	ble t1, t0, OuterLoop
	nop

End:
	move v0, t4

.close
