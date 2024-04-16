.psx
.create "hellogpu.bin", 0x80010000

.org 0x80010000

IO_BASE_ADDR equ 0x1F80

GP0 equ 0x1810
GP1 equ 0x1814

Main:
	lui a0, IO_BASE_ADDR
	
	la sp, 0x00103CF0
	
	li t1, 0x00000000 ; Reset GPU
	sw t1, GP1(a0)
	
	li t1, 0x03000000 ; Display enable
	sw t1, GP1(a0)
	
	li t1, 0x08000001 ; Display mode - 320x240, 15-bit colour, NTSC
	sw t1, GP1(a0)
	
	li t1, 0x06C60260 ; Horizontal display range
	sw t1, GP1(a0)
	
	li t1, 0x07042018 ; Vertical display range
	sw t1, GP1(a0)
	
	
	li t1, 0xE1000400 ; Draw mode settings
	sw t1, GP0(a0)
	
	li t1, 0xE3000000 ; Drawing area top left (0, 0)
	sw t1, GP0(a0)
	
	li t1, 0xE403BD3F ; Drawing area bottom right (319, 239)
	sw t1, GP0(a0)
	
	li t1, 0xE5000000 ; Drawing offset (0)
	sw t1, GP0(a0)
	
	; Clear screen rect
	li t1, 0x02913399 ; colour = (153R, 51G, 145B)
	sw t1, GP0(a0)
	li t1, 0x00000000 ; top left corner (0, 0)
	sw t1, GP0(a0)
	li t1, 0x00EF013F ; width/height (320x240)
	sw t1, GP0(a0)

	; Draw triangle
	li t1, 0x200080DB ; draw flat-shaded triangle, colour=(219R, 128G, 0B)
	sw t1, GP0(a0)
	
	li t1, 0x00EF0000 ; vertex1 = (0, 239)
	sw t1, GP0(a0)
	
	li t1, 0x000000A0 ; vertex2 = (160, 0)
	sw t1, GP0(a0)
	
	li t1, 0x00EF013F ; vertex3 = (319, 239)
	sw t1, GP0(a0)	
	
	; Draw quad
	li t1, 0x2800A000 ; draw flat-shaded quad, colour=(0R, 160G, 0B)
	sw t1, GP0(a0)	
	
	li t1, 0x00B40050 ; v1 = (80, 180)
	sw t1, GP0(a0)	
	
	li t1, 0x003C0050 ; v4 = (80, 60)
	sw t1, GP0(a0)	
	
	li t1, 0x00B400F0 ; v2 = (240, 180)
	sw t1, GP0(a0)	
	
	li t1, 0x003C00F0 ; v3 = (240, 60)
	sw t1, GP0(a0)	
	
	; Draw Gourad-shaded triangle
	li t1, 0x30FF0000 ; shaded triangle, c1 = blue
	sw t1, GP0(a0)	
	
	li t1, 0x003C0050 ; v1 = (80, 60)
	sw t1, GP0(a0)	
	
	li t1, 0x0000FF00 ; c2 = green
	sw t1, GP0(a0)
	
	li t1, 0x00B400A0 ; v2 = (160, 180)
	sw t1, GP0(a0)	
	
	li t1, 0x000000FF ; c3 = red
	sw t1, GP0(a0)	
	
	li t1, 0x003C00F0 ; v3 = (240, 60)
	sw t1, GP0(a0)


	; Prep DrawFlatTriangle call
	subi sp, 4 * 7

	li t1, 0xFFFF00 ; colour
	sw t1, 0(sp)
	
	li t1, 200 ; x1
	sw t1, 4(sp)
	
	li t1, 40 ; y1
	sw t1, 8(sp)
	
	li t1, 288 ; x2
	sw t1, 12(sp)

	li t1, 56 ; y2
	sw t1, 16(sp)
	
	li t1, 224 ; x3
	sw t1, 20(sp)
	
	li t1, 200 ; y3
	sw t1, 24(sp)
	
	jal DrawFlatTriangle
	nop

LoopForver:
	j LoopForver
	nop

; a0 = IO_BASE_ADDR
; sp = colour (0x00BBGGRR)
; sp+4 = x1
; sp+8 = y1
; sp+12 = x2
; sp+16 = y2
; sp+20 = x3
; sp+24 = y3
DrawFlatTriangle:
	lw t0, 0(sp) ; colour
	nop
	li t1, 0
	
	lui t1, 0x2000
	or t1, t0
	sw t1, GP0(a0) ; send command + colour
	
	lw t0, 4(sp) ; x1
	lw t1, 8(sp) ; y1
	nop
	
	sll t1, 16
	andi t0, 0xFFFF
	or t1, t0
	sw t1, GP0(a0) ; send vertex1
	
	lw t0, 12(sp) ; x2
	lw t1, 16(sp) ; y2
	nop
	
	sll t1, 16
	andi t0, 0xFFFF
	or t1, t0
	sw t1, GP0(a0) ; send vertex2
	
	lw t0, 20(sp) ; x3
	lw t1, 24(sp) ; y3
	nop
	
	sll t1, 16
	andi t0, 0xFFFF
	or t1, t0
	sw t1, GP0(a0) ; send vertex3
	
	addiu sp, 4 * 7
	
	jr ra
	nop

.close