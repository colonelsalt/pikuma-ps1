.psx
.create "hellogpu.bin", 0x80010000

.org 0x80010000

IO_BASE_ADDR equ 0x1F80

GP0 equ 0x1810
GP1 equ 0x1814

Main:
	lui t0, IO_BASE_ADDR
	
	li t1, 0x00000000 ; Reset GPU
	sw t1, GP1(t0)
	
	li t1, 0x03000000 ; Display enable
	sw t1, GP1(t0)
	
	li t1, 0x08000001 ; Display mode - 320x240, 15-bit colour, NTSC
	sw t1, GP1(t0)
	
	li t1, 0x06C60260 ; Horizontal display range
	sw t1, GP1(t0)
	
	li t1, 0x07042018 ; Vertical display range
	sw t1, GP1(t0)
	
	
	li t1, 0xE1000400 ; Draw mode settings
	sw t1, GP0(t0)
	
	li t1, 0xE3000000 ; Drawing area top left (0, 0)
	sw t1, GP0(t0)
	
	li t1, 0xE403BD3F ; Drawing area bottom right (319, 239)
	sw t1, GP0(t0)
	
	li t1, 0xE5000000 ; Drawing offset (0)
	sw t1, GP0(t0)
	
	; Clear screen rect
	li t1, 0x02913399 ; colour = (153R, 51G, 145B)
	sw t1, GP0(t0)
	li t1, 0x00000000 ; top left corner (0, 0)
	sw t1, GP0(t0)
	li t1, 0x0EF0013F ; width/height (320x240)
	sw t1, GP0(t0)

LoopForver:
	j LoopForver
	nop

.close