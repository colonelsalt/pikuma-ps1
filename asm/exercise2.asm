.psx
.create "exercise2.bin", 0x80010000
.org 0x80010000
Main:
	li t0, 0x1
	li t1, 0
	
Loop:
	addu t1, t1, t0
	addi t0, t0, 0x1
	ble t0, 0xA, Loop
End:
.close