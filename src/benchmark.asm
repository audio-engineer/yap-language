.autoimport on
.importzp sp
.import _printf

.export _StartTimerA
.export _StopTimerA

.segment "RODATA"

ticks_string:
    .byte "%U ticks",$0d,$00    ; %u ticks\r\0

.segment "CODE"

; ---------------------------------------------------------------
; void __near__ StartTimerA (void)
; ---------------------------------------------------------------

.proc _StartTimerA: near
    lda #$00
    sta $dc0e   ; Stop Timer A
    lda #$ff
    sta $dc04   ; Set Timer A low byte
    sta $dc05   ; Set Timer A high byte
    lda #$81
    sta $dc0e   ; Start Timer A
    rts
.endproc

; ---------------------------------------------------------------
; void __near__ StopTimerA (void)
; ---------------------------------------------------------------

.proc _StopTimerA: near
    lda $dc0e
    jsr pusha
    ldy #$00
    lda (sp),y
    and #$7f
    sta $dc0e
    lda $dc04
    sta tmp1
    lda $dc05
    sta tmp2
    lda tmp1
    sta tmp3
    lda tmp2
    sta tmp3+1
    lda #<(ticks_string)
    ldx #>(ticks_string)
    jsr pushax
    lda tmp3
    ldx tmp3+1
    jsr pushax
    ldy #$04
    jsr _printf
    jmp incsp1

.segment "BSS"

tmp1:
    .res 1,$00
tmp2:
    .res 1,$00
tmp3:
    .res 2,$00

.endproc
