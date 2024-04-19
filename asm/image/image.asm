.psx
.create "image.bin", 0x80010000

.org 0x80010000

IO_BASE_ADDR equ 0x1F80

GP0 equ 0x1810
GP1 equ 0x1814

IMG_WIDTH equ 640
IMG_HEIGHT equ 480
IMG_SIZE_BYTES equ 921600

Main:
	lui a0, IO_BASE_ADDR
	
	li t1, 0x00000000 ; Reset GPU
	sw t1, GP1(a0)
	
	li t1, 0x03000000 ; Display enable
	sw t1, GP1(a0)
	
	li t1, 0x08000037 ; Display mode - 640x480, 24-bit colour, NTSC
	sw t1, GP1(a0)
	
	li t1, 0x06C60260 ; Horizontal display range
	sw t1, GP1(a0)
	
	li t1, 0x0707E018 ; Vertical display range
	sw t1, GP1(a0)
	
	
	li t1, 0xE1000400 ; Draw mode settings
	sw t1, GP0(a0)
	
	li t1, 0xE3000000 ; Drawing area top left (0, 0)
	sw t1, GP0(a0)
	
	li t1, 0xE403BD3F ; Drawing area bottom right (319, 239)
	sw t1, GP0(a0)
	
	li t1, 0xE5000000 ; Drawing offset (0)
	sw t1, GP0(a0)

	; Copy custom pixel rect to VRAM
	li t1, 0xA0000000
	sw t1, GP0(a0)
	
	li t1, 0x00000000 ; top left dest coord = (0,0)
	sw t1, GP0(a0)
	
	li t1, 0x01E003C0 ; width & height in half-words (640x480)
	sw t1, GP0(a0)
	
	li t0, IMG_SIZE_BYTES
	
	sra t0, 2 ; t /= 4
	
	la t2, Image
LoopWords:
	lw t1, 0(t2)
	addiu t2, 4
	sw t1, GP0(a0)
	
	bnez t0, LoopWords
	addi t0, -1 ; (happens before branch!)

LoopForver:
	j LoopForver
	nop
	
Image:
	.incbin "logo.bin"		; 640x480 24bpp image (921,600 bytes)

.close