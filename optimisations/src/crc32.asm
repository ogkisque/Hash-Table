section .text
global asm_get_hash_crc32

asm_get_hash_crc32:
    xor eax, eax                    ; eax = 0
    xor r11, r11                    ; r11 = 0
    .loop:
        crc32 eax, byte [rdi+r11]
        inc r11                     ; r11++
        cmp r11, rsi
        jne .loop
    ret