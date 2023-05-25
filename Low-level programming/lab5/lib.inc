printstr macro str
    mov dx, offset str
    mov ah, 09h
    int 21h
endm
replaceby macro x
    mov al, x
    mov byte ptr [si], al
endm
checkflag macro f
    mov al, f
    cmp al, 1
endm
newline macro
    push ax
    push dx
    mov ah, 02h
    mov dl, 10
    int 21h
    mov dl, 13
    int 21h
    pop dx
    pop ax
endm
    