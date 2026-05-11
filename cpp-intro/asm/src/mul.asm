MAX_QWORDS:     equ             128

                section         .text

                global          _start
_start:

                sub             rsp, 2 * MAX_QWORDS * 8                 ;store place for ans
                ;PLACEMENT IN MEMORY
                ;[rsp]                           - register rdi (first operand)
                ;[rsp + MAX_QWORDS * 8]          - register rsi (second operand)
                ;[rsp + 2 * MAX_QWORDS * 8]      - begin of ans (needs 2*MAX_QWORDS*8 to store it)
                sub             rsp, 2 * MAX_QWORDS * 8
                lea             rdi, [rsp + MAX_QWORDS * 8]
                mov             rcx, MAX_QWORDS
                call            read_long
                mov             rdi, rsp
                call            read_long
                lea             rsi, [rsp + MAX_QWORDS * 8]
                call            multiply_long_long

                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit

; multiply two long numbers
;    rdi -- address of operand #1 (long number)
;    rsi -- address of operand #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    multiply is written to rdi
multiply_long_long:
                lea             r12, [rsp + 4 * MAX_QWORDS * 8 + 8]     ;register for check end of data
                ;(it is used in clear data)
                lea             r11, [rsp + 2 * MAX_QWORDS * 8 + 8]     ;set cur

                ;why add 8? if we check edb we will see that on top of stack
                ;is return address of call, not rdi,
                ;so shift need to calculate with one additional qword

                push            r11                                     ;push on stack address of begin of ans
                ;and on end of multiply I will
                ;pop rdi to save begin address of ans to rdi register
                jmp             .clear_data                             ;clear place for ans

.init_cycle:
                xor             r13, r13
                mov             r10, MAX_QWORDS                         ;iterations left for first number
                lea             r11, [rsp + 2 * MAX_QWORDS * 8 + 16]    ;cur pointer on ans
                ;16 add because on the top of stack is two elements before rdi

                push            rcx
                push            rdi
                jmp             .mul_first_digits

.mul_second_digits:
                mov             r8, [rsi]                               ;cur qword
                xor             r14, r14                                ;prepare register to save new carry bit
                push            rax                                     ;cur first number qword
                mul             r8                                      ; RDX:RAX=RAX*r8

                add             [r11], rax                              ;save low half new
                adc             [r11+8], rdx                            ;save high half new
                adc             r14, 0
                adc             [r11+8], r13                            ;add carry from last high half
                pop             rax                                     ;recover cur qword of first number
                adc             r14, 0                                  ;save carry
                mov             r13, r14

                add             r11, 8                                  ;move pointer on ans
                add             rsi, 8                                  ;get next qword of second number


                cmp             rsi, rcx                                ;left number of iterations for second number
                jb              .mul_second_digits

                add             rdi, 8                                  ;get next qword of first number

                pop             r11                                     ;recover global pointer on ans from begin
                add             r11, 8                                  ;move global pointer on ans
                pop             rsi                                     ;recover pointer on begin of second number
                dec             r10                                     ;left number of iterations for first number
                jz              .outro

.mul_first_digits:
                push            rsi                                     ;save pointer on begin of second number
                push            r11                                     ;save pointer on cur index of ans
                mov             rax, [rdi]                              ;save mul
                lea             rcx, [rsp + 2 * MAX_QWORDS * 8 +48]     ;iterations left for second number
                jmp             .mul_second_digits

.outro:
                pop             rdi                                     ;recover begin pointer on first number
                pop             rcx                                     ;recover rcx to write a number len
                mov             rcx, 2*MAX_QWORDS                       ;write number len
                pop             rdi
                ret

.clear_data:

                mov             qword [r11], 0                          ;set [r11] value to 0
                lea             r11, [r11 + 8]                          ;get next byte

                cmp             r11, r12                                ;end of ans
                jb              .clear_data
                jmp             .init_cycle



; adds a short number to a long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi



add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx, rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies a long number by a short number
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rdi
                pop             rax
                ret

; divides a long number by a short number
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    remainder is written to rdx
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns zero to a long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; reads a long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; writes a long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; reads one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; writes one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; prints a string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: \
                equ             $ - invalid_char_msg
