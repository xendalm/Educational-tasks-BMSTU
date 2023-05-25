macro printstr str {
    mov dx, str
    mov ah, 09h
    int 21h
}
macro replaceby x {
    mov al, x
    mov [si], al
}
macro checkflag f {
    mov al, f
    cmp al, 1
}
macro newline {
    push ax
    push dx
    mov ah, 02h
    mov dl, 10
    int 21h
    mov dl, 13
    int 21h
    mov dl, 10
    int 21h
    mov dl, 13
    int 21h
    pop dx
    pop ax
}
    