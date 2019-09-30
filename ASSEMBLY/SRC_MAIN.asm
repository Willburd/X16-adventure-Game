; HEADER
*=$0801
!byte $0C,$08,$0A,$00,$9E,$20
!byte $32,$30,$36,$34,$00,$00
!byte $00

; Utility table
GFXRAM=$01FFFF							; pointer to Start of the VERA graphics ram
HIMEM=$A000								; pointer to start of HIMEM bank
HIMEMCONTROLLER=$9F61					; pointer to HIMEM bank control byte

; KERNAL calls
KERNAL_CINT=$FF81						; Init kernal video defaults
KERNAL_CHROUT=$FFD2						; CHROUT outputs the character in A
KERNAL_CHRIN=$FFCF						; CHRIN read from default input
KERNAL_SETNAM=$FFBD						; Set filename     		A = File name length; X = LO name address	; Y = HI name address
KERNAL_SETLFS=$FFBA						; Set filesystem   		A = Logical number	; X = Device number	; Y = Secondary address
KERNAL_LOAD=$FFD5						; LOAD file to memory	A = load/verify		; X = LO goal address	; Y = HI goal address
KERNAL_SAVE=$FFD8						; SAVE file to disk

; VERA interface
VERA_ADD_HI=$9F22 						; IIIIAAAA 20 bit address, incrimenter at top
VERA_ADD_MI=$9F21 						; MMMMMMMM 20 bit address
VERA_ADD_LO=$9F20						; LLLLLLLL 20 bit address
VERA_DATA0=$9F23	
VERA_DATA1=$9F24					
VERA_CONTROL=$9F25						; R------A R=reset, A=port1/2
VERA_IEN=$9F26							; ----USLV Uart, Sprcol, Line, Vsync  ; interupt clearing, write 1 to the interupt to clear it
VERA_ISR=$9F27							; ----USLV Uart, Sprcol, Line, Vsync  ; interupt output

; ZP registers
R0=$00
R1=$01
R2=$02
R3=$03
R4=$04
R5=$05
R6=$06
R7=$07
R8=$08
R9=$09

; Ram table
*=$7000


*=$0810
; GAMECODE START
	; default video data just incase
	JSR KERNAL_CINT

	; init video controller
	LDA #%00000000
	STA VERA_CONTROL
	
	; Display composer setup
	LDA #$1f
	LDY #$00
	LDX #$00
	JSR setVERAHml
	; Set screen scale factor
	;     F----COO	; C chroma disable, OO output mode
	LDA #%00000001
	STA VERA_DATA1
	LDA #$64			; 128 = 1x, 64 = 2x
	STA VERA_DATA1
	LDA #$64	 		; 128 = 1x, 64 = 2x
	STA VERA_DATA1
	
	
	; palette setup
	LDA #$1f
	LDY #$10
	LDX #$00
	JSR setVERAHml
	
	
	; Layer Setup
	LDA $1f
	LDY $20
	LDX $00
	JSR setVERAHml
	;     MMM----E ; Mode, Enable
	LDA #%11100001
	STA VERA_DATA1		;$40001	L0_CTRL1		--HWhhww	H=tile Height / W=tile Width / m=map height / w=map width
	LDA #$00
	STA VERA_DATA1		;$40002	L0_MAP_BASE_L 	LLLLLLLL	Map Base (9:2)
	LDA #%00000000
	STA VERA_DATA1		;$40003	L0_MAP_BASE_H	HHHHHHHH	Map Base (17:10)
	LDA #$00
	STA VERA_DATA1		;$40004	L0_TILE_BASE_L 	LLLLLLLL	Tile Base (9:2)
	LDA #%00100000
	LDA #$00
	STA VERA_DATA1		;$40005	L0_TILE_BASE_H  HHHHHHHH	Tile Base (17:10)
	; scroll data init
	LDA #$00
	LDA VERA_DATA1		;$40006	L0_HSCROLL_L	LLLLLLLL	Hscroll (7:0)
	LDA #$00
	LDA VERA_DATA1		;$40007	L0_HSCROLL_H 	----HHHH	Vscroll (11:8)
	LDA #$00
	LDA VERA_DATA1		;$40008	L0_VSCROLL_L  	LLLLLLLL	Hscroll (7:0)
	LDA #$00
	LDA VERA_DATA1		;$40009	L0_VSCROLL_H 	----HHHH	Vscroll (11:8)
	
	; change bank to 0
	LDA	#$00
	STA+1 R0
	JSR changeBank
	
	; Load level0 to bank0
	LDA+1 #(FILE_00_END - FILE_00) ; string length
	STA R2
	LDA #<FILE_00 ; string address startLO
	STA R0
	LDA #>FILE_00 ; string address startHI
	STA R1
	LDA #<HIMEM ; goalLO
	STA R3
	LDA #>HIMEM ; goalHI
	STA R4
	JSR loadFile
	
	; Test time!
	LDA #$10
	LDY #$00
	LDX #$00
	JSR setVERAHml
	; 
	LDX #$10 + 1
	LDY #$2C + 1
	LDA #$00 + 1
TestLoop:
	STA VERA_DATA1
	SEC
	SBC #$01
	BNE TestLoop
	DEY
	BNE TestLoop
	DEX
	BNE TestLoop
	
	
	; Output test text
	; LDA	#<HIMEM
	; STA+1 R0
	; LDA	#>HIMEM
	; STA+1 R1
	; JSR outputString

	; end game
	jsr	KERNAL_CHRIN	; Wait for input
	RTS

	
outputString: ; ========================= outputString( string address LO, string address HI)
	LDY	#$00			; Y register is used to index the string
outputString_loop:
	LDA (R0),Y 		; Load character from string into A reg with X offset
	BEQ	outputString_end				; If the character was 0, jump to end label
	JSR	KERNAL_CHROUT	; Output character stored in A register
	INY					; Increment X register
	JMP	outputString_loop			; Jump back to loop label to print next char
outputString_end:
	RTS

	
changeBank:	; ============================= ChangeBank(index)
	LDA R0
	; begin
	STA HIMEMCONTROLLER				; Store A to HIMEM bank selector
	RTS
	
	
loadFile: ; =============================== LoadFile( string address startLO, string address startHI, string length, goalLO, goalHI) into work ram
	CLC
	CLV
	; SET FILE NAME
	LDX R0 					; LO file name string start address
    LDY R1 					; HI file name string start address
	LDA R2					; string length
	JSR KERNAL_SETNAM
	; SET FILE SYSTEM
	LDA #$01				; logical file number is 1
	LDX $BA       			; last used device number
    BNE loadFile_skip
    LDX #$08      			; device 8 default
loadFile_skip:	
	LDY #$00      			; $00 means: load to new address
	JSR KERNAL_SETLFS
	; LOAD DATA
	LDX R3					; LO goal address
    LDY R4 					; HI goal address
	LDA #$00 				; LOAD = 0, VERIFY = 1
	JSR KERNAL_LOAD
	BCS outputErrorText    	; ($0879) if carry set, a load error has happened
    RTS
	
setVERAHml: ; ==================================== setVERAHml(HI A, MI Y, LO X)
	; top half of A is the incrimenter for vera!
	STA VERA_ADD_HI
	STY VERA_ADD_MI
	STX VERA_ADD_LO
	RTS
	
loadGraphicsSet: ; =============================== loadGraphicsSet() run after a level file is loaded, assumes format
	; tile color is in NES interleaved first 8 bytes are bit 0 of color index, second set controls bit 1
	
	RTS
	

outputErrorText: ; =============================== outputErrorText( reads from A)
	CLC
	CLV
	CMP #$05
	BEQ outputErrorText_ERRA
	CMP #$04
	BEQ outputErrorText_ERRB
	CMP #$1D
	BEQ outputErrorText_ERRC
	CMP #$00
	BEQ outputErrorText_ERRD
	CMP #$08
	BEQ outputErrorText_ERRE
	RTS
	; each error gives a different output string
outputErrorText_ERRA:
	LDA	#<STRING_DEVICEERROR
	STA+1 R0
	LDA	#>STRING_DEVICEERROR
	STA+1 R1
	jmp outputErrorText_ERREND;
outputErrorText_ERRB:
	LDA	#<STRING_NOFILE
	STA+1 R0
	LDA	#>STRING_NOFILE
	STA+1 R1
	jmp outputErrorText_ERREND;
outputErrorText_ERRC:
	LDA	#<STRING_LOADERROR
	STA+1 R0
	LDA	#>STRING_LOADERROR
	STA+1 R1
	jmp outputErrorText_ERREND;
outputErrorText_ERRD:
	LDA	#<STRING_BREAKERROR
	STA+1 R0
	LDA	#>STRING_BREAKERROR
	STA+1 R1
	jmp outputErrorText_ERREND;
outputErrorText_ERRE:
	LDA	#<STRING_MISSINGERROR
	STA+1 R0
	LDA	#>STRING_MISSINGERROR
	STA+1 R1
outputErrorText_ERREND:
	; Output error text
	JSR outputString
	jsr	KERNAL_CHRIN	; Wait for input
	; force quit on error
	JSR forceExitProgram
    RTS


forceExitProgram: ; ==================== forceExitProgram() kill game
	SEI
	LDA #$37
	STA $01
	CLI
	JSR     $ff8a           ; RESTOR: Initialize vector table $0314-$0333
	JSR     $ff81           ; SCINIT: Initialize VIC++
	JSR     $ff84           ; IOINIT: Initialize CIAs++ 
	RTS


*=$8000
; File Table
FILE_00:					!text "level1.prg"
FILE_00_END:  				; String end
FILE_01:					!text "FILE_01"
FILE_01_END: 				; String end
FILE_02:					!text "FILE_02"
FILE_02_END: 				; String end
FILE_03:					!text "FILE_03"
FILE_03_END: 				; String end
	

; String Table
STRING_00:					!text "ATTEMPTING TO LOAD DATA ON DISK...", $0D, 0
STRING_00_END: 				; String end
STRING_DEVICEERROR: 		!text "A PROBLEM WITH THE DEVICE OCCURRED", $0D, 0
STRING_DEVICEERROR_END: 	; String end
STRING_NOFILE:				!text "FILE WAS NOT FOUND", $0D, 0
STRING_NOFILE_END: 			; String end
STRING_LOADERROR:			!text "A error occured during program LOAD", $0D, 0
STRING_LOADERROR_END:		; String end
STRING_BREAKERROR:			!text "LOADING INTERRUPTED", $0D, 0
STRING_BREAKERROR_END: 		; String end
STRING_MISSINGERROR:		!text "MISSING FILE NAME", $0D, 0
STRING_MISSINGERROR_END:	; String end