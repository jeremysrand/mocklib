;
;  speech.s
;  mocktest
;
;  Created by Jeremy Rand on 2016-09-29.
;  Copyright © 2016 Jeremy Rand. All rights reserved.
;


    .export _setupSpeech, _unsetupSpeech
    .export _speechData, _speechLen, _speechBusy


TMPPTR  := $FB           ; Temporary pointer used in interrupt handler
IRQL    := $03FE         ; Interrupt vector, low byte
IRQH    := $03FF         ; Interrupt vector, high byte
BASE    := $C440         ; First speech chip
DURPHON := BASE          ; Register 0 of speech chip
INFLECT := BASE+$01      ; Register 1 of speech chip
RATEINF := BASE+$02      ; Register 2 of speech chip
CTTRAMP := BASE+$03      ; Register 3 of speech chip
FILFREQ := BASE+$04      ; Register 4 of speech chip
DDRB    := $C402
DDRA    := $C403
PCR     := $C48C         ; Peripheral control register, 6522
IFR     := $C48D         ; Interrupt flag register, 6522
IER     := $C48E


.DATA
_speechData:    .byte   $00, $00
_speechLen:     .byte   $00, $00
_outptr:        .byte   $00, $00
_endptr:        .byte   $00, $00
_speechBusy:    .byte   $00
_irqLandingPad: .byte   $4C, $00, $00


.CODE

.proc _setupSpeech
    sei                 ; Disable interrupts

    lda IRQL            ; Store the old interrupt vector
    sta _irqLandingPad+1
    lda IRQH
    sta _irqLandingPad+2

    lda #<_interr       ; Set interrupt vector
    sta IRQL            ; to point to interrupt
    lda #>_interr       ; service routine
    sta IRQH
    lda #$00
    sta DDRA
    sta DDRB

    lda _speechData+1   ; Get high address of data
    sta _outptr+1       ; Store in work pointer
    lda _speechData     ; Get low address of data
    sta _outptr         ; Store low byte

    lda _speechLen+1    ; Get high length byte
    clc
    adc _speechData+1   ; And add to base address
    sta _endptr+1       ; Store end address
    lda _speechLen      ; Get low length byte
    clc
    adc _speechData     ; And add to base address
    bcc @L2             ; Check for page boundary
    inc _endptr+1
@L2:
    sta _endptr         ; Store end address

    lda #$FF            ; Set busy flag
    sta _speechBusy     ; And set peripheral control
    lda #$0C            ; Register to recognize
    sta PCR             ; Signal from speech chip
    lda #$80            ; Raise control bit in register 3
    sta CTTRAMP
    lda #$C0            ; Set transitioned inflection
    sta DURPHON         ; Mode in register 0
    lda #$70            ; Lower control bit
    sta CTTRAMP
    lda #$82            ; Enable 6522 interrupts
    sta IER
    cli                 ; Clear interrupt mask
    rts                 ; Return to caller
.endproc


.proc _unsetupSpeech
    sei                 ; Restore the old interrupt vector
    lda _irqLandingPad+1
    sta IRQL
    lda _irqLandingPad+2
    sta IRQH
    cli
    rts
.endproc


.proc _interr
    pha                 ; Save accumulator
    lda IFR
    bmi @L4             ; If we have 6522 interrupt jump to L4
    pla                 ; Otherwise restore the accumulator
    jmp _irqLandingPad  ; And jump to the old interrupt handler
@L4:
    txa                 ; Save other registers
    pha
    tya
    pha
    lda #$02            ; Clear interrupt flag
    sta IFR
    ldy #$00            ; Init registers
    ldx #$04
    lda _outptr+1
    cmp _endptr+1
    bcc @L1
    bne @L5
    lda _outptr         ; Check for end of data file
    cmp _endptr
    bcc @L1             ; If not, then continue
@L5:
    lda #$00            ; If end, turn everything off
    sta DURPHON         ; Store pause phoneme
    lda #$70            ; Zero amplitude
    sta CTTRAMP
    lda #$00            ; Clear busy flag
    sta _speechBusy
    lda #$02            ; Clear interrupt enable
    sta IER             ; In 6522
    lda #$FF
    sta DDRA
    lda #$07
    sta DDRB
@L2:
    pla                 ; Restore registers
    tay
    pla
    tax
    pla
    rti                 ; Return from interrupt

@L1:
    lda TMPPTR          ; Save the value in the tmp pointer
    pha
    lda TMPPTR+1
    pha
    lda _outptr         ; Move the _outptr into the tmp pointer
    sta TMPPTR
    lda _outptr+1
    sta TMPPTR+1
@L6:
    lda (TMPPTR),Y      ; Get data
    sta BASE,X          ; Store in speech chip
    inc TMPPTR          ; Next data
    bne @L3
    inc TMPPTR+1

@L3:
    dex                 ; Next register
    cpx #$FF            ; Last register?
    bne @L6             ; No, then continue
    lda TMPPTR          ; We are done, move tmp pointer to _outptr
    sta _outptr
    lda TMPPTR+1
    sta _outptr+1
    pla                 ; Restore the tmp pointer
    sta TMPPTR+1
    pla
    sta TMPPTR
    jmp @L2             ; Finish the interrupt handler
.endproc

