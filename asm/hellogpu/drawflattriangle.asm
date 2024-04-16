.psx
.create "drawflattriangle.bin", 0x80010000

.org 0x80010000

IO_BASE_ADDR equ 0x1F80

GP0 equ 0x1810
GP1 equ 0x1814


Main:
	lui a0, IO_BASE_ADDR

	li s0, 0xFFFF00
	li s1, 200
	li s2, 40
	li s3, 288
	li s4, 56
	li s5, 224
	li s6, 200
	jal DrawFlatTriangle
	nop
	
LoopForever:
	j LoopForever
	nop


; a0 = IO_BASE_ADDR
; s0 = colour (0x00BBGGRR)
; s1 = x1
; s2 = y1
; s3 = x2
; s4 = y2
; s5 = x3
; s6 = y3
DrawFlatTriangle:
	li t1, 0
	
	lui t1, 0x2000
	or t1, s0
	sw t1, GP0(a0) ; send command + colour
	
	sll s2, 16
	andi s1, 0xFFFF
	or t1, s2, s1
	sw t1, GP0(a0) ; send vertex1
	
	sll s4, 16
	andi s3, 0xFFFF
	or t1, s4, s3
	sw t1, GP0(a0) ; send vertex2
	
	sll s6, 16
	andi s5, 0xFFFF
	or t1, s6, s5
	sw t1, GP0(a0) ; send vertex3
	
	jr ra
	nop
	
	
.close