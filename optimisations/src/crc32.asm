section .text
global asm_get_hash_crc32

asm_get_hash_crc32:
    mov eax, 0
    mov r11, 0 
    .loop:
        crc32 eax, byte [rdi+r11]
        inc r11
        cmp r11, rsi
        jne .loop
    ret