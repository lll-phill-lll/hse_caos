
.intel_syntax noprefix
.global compute_hypots

compute_hypots:
    push rbp
    mov  rbp, rsp

    // rdi — адрес первого элемента массива
    // rsi — размер массива (количество элементов)

    test rsi, rsi
    jz   done

compute:
    mov eax, [rdi]
    imul eax, eax

    mov ecx, [rdi + 4]
    imul ecx, ecx

    // сумма квадратов
    add eax, ecx
    // записываем в поле hypot
    mov [rdi + 8], eax     

    // переходим к следующей структуре
    add rdi, 12            
    dec rsi
    jnz compute


done:
    mov rsp, rbp
    pop rbp
    ret

