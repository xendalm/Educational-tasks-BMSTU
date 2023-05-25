assume cs: code, ds: data
data segment
    res db      0ah,0dh,'result: ','$'
    string1     db 100, 99 dup ('$')
    string2    db 50, 49 dup ('$')
    dummy db 0Ah, '$'
data ends
code segment
strcspn proc
    push bp
    mov bp, sp
    
    mov ah, 09h
    
    
    mov si, [bp+6] ;string1 len
    xor bx, bx
    mov bl, byte ptr [si] ;default ans
    sub bx, 2
    mov si, [bp+4] ;string2 len
    xor cx, cx
    mov cl, byte ptr [si]
    
    mov si, [bp+4]
    add si, 2
    xor ax, ax
    
    cycle:
        xor ax, ax
        mov al, byte ptr [si]
        cmp al, 13
        je breakl
        cmp al, 36
        je breakl
        push cx
        cld
        mov di, [bp+6]
        add di, 2
        mov cx, bx
        repne scasb
        jne notfound
            sub di, [bp+6]
            sub di, 3
            mov bx, di
        notfound:
        pop cx
        inc si
        loop cycle
breakl:
    pop bp
    
    pop ax
    push bx
    push ax
    ret
strcspn endp    

start:  
    mov     ax, data
    mov     ds, ax
    mov     es, ax
    
    
    mov dx, offset string1
    mov ah, 0Ah
    int 21h
    mov dx, offset dummy
    mov ah, 09h
    int 21h
    mov dx, offset string2
    mov ah, 0Ah
    int 21h
    mov dx, offset dummy
    mov ah, 09h
    int 21h
    
    mov dx, offset string1
    push dx
    mov dx, offset string2
    push dx
    call strcspn
    
    mov dx, offset res
    mov ah, 09h
    int 21h
    pop ax
    call printdec
    
    mov     ax,4c00h
    int     21h

printdec proc
    push ax
    push bx
    push cx
    push dx
    mov bx, 10
    xor cx, cx
    cmp ax, 0
    jnl deccycle
    ;negative
        neg ax
        push ax
        mov ah, 02h
        mov dl, '-'
        int 21h
        pop ax
    deccycle:
        xor dx, dx
        div bx
        push dx
        inc cx
        or ax, ax
        jnz deccycle
    mov ah, 02h
    print:
        pop dx
        add dl, '0'
        int 21h
    loop print
    mov dl, 32
    int 21h
    pop dx
    pop cx
    pop bx
    pop ax
    ret
printdec endp

code ends
end start