; Use CIA 1 as a timer to benchmark function calls
; See https://www.c64-wiki.com/wiki/CIA

.autoimport on
.importzp sp
.import _printf

.export _StartTimerA
.export _StopTimerA

.segment "RODATA"

ticks_string:
    .byte "%U ticks",$0d,$00    ; "%u ticks\r\0"

.segment "CODE"

; ---------------------------------------------------------------
; void __near__ StartTimerA (void)
; ---------------------------------------------------------------

.proc _StartTimerA: near
; Stop Timer A
    lda #$00    ; Load 0x00 into A register
    sta $dc0e   ; Stop Timer A by setting all control bits to 0

; Set Timer A start value
    lda #$ff    ; Load 0xff into A register
    sta $dc04   ; Set Timer A low byte to 0xff
    sta $dc05   ; Set Timer A high byte to 0xff

; Set Timer A configuration
    lda #$81    ; Load 1000 0001 into A register. Bit 7 high: Start Timer A, bit 0 high: 50 Hz clock speed
    sta $dc0e   ; Load configuration into Timer A
    rts
.endproc

; ---------------------------------------------------------------
; void __near__ StopTimerA (void)
; ---------------------------------------------------------------

.proc _StopTimerA: near
; Stop Timer A by setting control bit 7 to low while keeping the other control bits intact
    lda $dc0e               ; Load Timer A control register into A register
    and #$7f                ; AND control bits with 0111 1111 (stop Timer A)
    sta $dc0e               ; Stop Timer A

; Read Timer A 16-bit value from $dc04 and $dc05 and store it in tmp1 and tmp1+1
    lda $dc04               ; Load Timer A value low byte into A register
    sta tmp1                ; Store Timer A value low byte in tmp1
    lda $dc05               ; Read Timer A value high byte into A register
    sta tmp1+1              ; Store Timer A value high byte in tmp1+1

; Subtract Timer A 16-bit value from 0xffff to get the difference (result)
; We will use the one's complement plus one method: (0xffff - x) = (~x + 1)
    lda tmp1                ; Load Timer A value low byte into A register
    eor #$ff                ; Low byte XOR 0xff = low byte one's complement
    sta res1                ; Store Timer A value low byte one's complement in res1
    lda tmp1+1              ; Load Timer A value high byte into A register
    eor #$ff                ; High byte XOR 0xff = high byte one's complement
    sta res1+1              ; Store Timer A value high byte one's complement in res1+1

; Add 1 to result. If res1 is 0xff, the carry flag will be set and res1+1 needs to be incremented too
    clc                     ; Clear carry flag
    inc res1                ; Increment value low byte
    bne done                ; If carry is zero: Go to done, otherwise go to next line.
    inc res1+1              ; Increment value high byte

; Push format format string low and high bytes onto stack
done:
    lda #<(ticks_string)    ; Load format string low byte into A register
    ldx #>(ticks_string)    ; Load format string high byte into A register
    jsr pushax              ; Push format string low and high bytes onto stack

; Push Timer A value low and high bytes onto stack
    lda res1                ; Load Timer A value low byte into A register
    ldx res1+1              ; Load Timer A value high byte into X register
    jsr pushax              ; Push Timer A value low and high bytes onto stack

; Call printf, pop 4 bytes off stack and return
    ldy #$04                ; Load 0x04 into Y register: Tell printf there are 4 bytes of data to print (16-bit value, \r, \0)
    jsr _printf             ; Call printf
    jmp incsp1              ; Cleanup

.segment "BSS"

tmp1:
    .res 2,$00  ; Reserve 2 bytes of zeros

res1:
    .res 2,$00  ; Reserve 2 bytes of zeros

.endproc
