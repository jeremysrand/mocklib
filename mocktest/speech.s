;
;  speech.s
;  mocktest
;
;  Created by Jeremy Rand on 2016-09-29.
;  Copyright © 2016 Jeremy Rand. All rights reserved.
;


    .export _setupSpeech, _numIRQs, _speechData, _speechLen, _busy
;    .interruptor speechIRQ


OUTPTR  := $FB           ;START OF DATA POINTER
IRQL    := $03FE         ;INTERRUPT VECTOR, LOW BYTE
IRQH    := $03FF         ;INTERRUPT VECTOR, HIGH BYTE
BASE    := $C440         ;FIRST SPEECH CHIP
DURPHON := BASE          ;REGISTER 0 OF SPEECH CHIP
INFLECT := BASE+$01      ;REGISTER 1 OF SPEECH CHIP
RATEINF := BASE+$02      ;REGISTER 2 OF SPEECH CHIP
CTTRAMP := BASE+$03      ;REGISTER 3 OF SPEECH CHIP
FILFREQ := BASE+$04      ;REGISTER 4 OF SPEECH CHIP
DDRB    := $C402
DDRA    := $C403
PCR     := $C48C         ;PERIPHERAL CONTROL REG-6522
IFR     := $C48D         ;INTERRUPT FLAG REG-6522
IER     := $C48E


.DATA
speechIRQ:      .byte   $60, <_interr, >_interr   ; RTS plus two dummy bytes
_numIRQs:       .byte   $00
_speechData:    .byte   $00, $00
_speechLen:     .byte   $00, $00
endptr:         .byte   $00, $00
_busy:          .byte   $00


.CODE

.proc _setupSpeech
    SEI             ;DISABLE INTERRUPTS
    LDA #<_interr   ;SET INTERRUPT VECTOR
    STA IRQL        ;TO POINT TO INTERRUPT
    LDA #>_interr   ;SERVICE ROUTINE
    STA IRQH
;    LDA #$4C        ; JMP opcode
;    STA speechIRQ
    LDA #$00
    STA DDRA
    STA DDRB

    LDA _speechData+1   ;GET HIGH ADDRESS OF DATA
    STA OUTPTR+1        ;STORE IN WORK POINTER
    LDA _speechData     ;GET LOW ADDRESS OF DATA
    STA OUTPTR          ;STORE LOW BYTE

    LDA _speechLen+1    ;GET HIGH LENGTH BYTE
    CLC
    ADC _speechData+1   ;AND ADD TO BASE ADDRESS
    STA endptr+1        ;STORE END ADDRESS
    LDA _speechLen      ;GET LOW LENGTH BYTE
    CLC
    ADC _speechData     ;AND ADD TO BASE ADDRESS
    BCC @L2         ;CHECK FOR PAGE BOUNDARY
    INC endptr+1
@L2:
    STA endptr      ;STORE END ADDRESS

    LDA #$FF        ;SET BUSY FLAG
    STA _busy       ;AND SET PERIPHERAL CONTROL
    LDA #$0C        ;REGISTER TO RECOGNIZE
    STA PCR         ;SIGNAL FROM SPEECH CHIP
    LDA #$80        ;RAISE CTRL BIT IN REGISTER 3
    STA CTTRAMP
    LDA #$C0        ;SET TRANSITIONED INFLECTION
    STA DURPHON     ;MODE IN REGISTER 0
    LDA #$70        ;LOWER CTRL BIT
    STA CTTRAMP
    LDA #$82        ;ENABLE 6522 INTERRUPTS
    STA IER
    CLI             ;CLEAR INTERRUPT MASK
    RTS             ;RETURN TO CALLER
.endproc


.proc _interr
    TXA             ;SAVE REGISTERS
    PHA
    TYA
    PHA
    LDA IFR
    BPL @L2
    LDA #$02        ;CLEAR INTERRUPT FLAG
    STA IFR
    LDY #$00        ;INIT REGISTERS
    LDX #$04
    LDA OUTPTR+1
    CMP endptr+1
    BCC @L1
    BNE @L5
    LDA OUTPTR      ;CHECK FOR END OF DATA FILE
    CMP endptr
    BCC @L1         ;IF NOT THEN CONTINUE
@L5:
    LDA #$00        ;IF END, TURN EVERYTHING OFF
    STA DURPHON     ;STORE PAUSE PHONEME
    LDA #$70        ;ZERO AMPLITUDE
    STA CTTRAMP
    LDA #$00        ;CLEAR BUSY FLAG
    STA _busy
    LDA #$02        ;CLEAR INTERRUPT ENABLE
    STA IER         ;IN 6522
    LDA #$FF
    STA DDRA
    LDA #$07
    STA DDRB
@L2:
    PLA             ;RESTORE REGISTERS
    TAY
    PLA
    TAX
    LDA $45
    RTI             ;RETURN FROM INTERRUPT

@L1:
    LDA (OUTPTR),Y  ;GET DATA
    STA BASE,X      ;STORE IN SPEECH CHIP
    INC OUTPTR      ;NEXT DATA
    BNE @L3
    INC OUTPTR+1

@L3:
    DEX             ;NEXT REGISTER
    CPX #$FF        ;LAST REGISTER?
    BNE @L1         ;NO, CONTINUE
    BEQ @L2         ;YES, RETURN
.endproc

