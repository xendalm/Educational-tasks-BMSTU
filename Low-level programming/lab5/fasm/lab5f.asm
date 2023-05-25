include 'lib.inc'
use16
org 100h
    jmp start

msg1 db "Source string: $"
msg2 db "Symbol to find: $"
msg3 db "Result: $"
symbol db 0
foundflag db 0
infile db 'source.txt',0
outfile db 'result.txt',0
s_error db 'Error!',13,10,'$'
buffer rb 101
handle rw 1
size db 88

start:
    mov ah, 3dh
    xor al, al
    mov dx, infile
    xor cx, cx
    int 21h
    jnc @F
    call error_msg
    jmp exit
@@: mov [handle], ax

    mov bx, ax
    mov ah, 3fh
    mov dx, buffer
    mov cx, 100
    int 21h
    jnc @F
    call error_msg
    jmp close_file

@@: mov bx, buffer
    add bx, ax
    mov byte[bx], '$'

close_file:
    mov ah, 3eh
    mov bx, [handle]
    int 21h
    jnc begin
    call error_msg
    
begin:
    printstr msg1
    newline
    printstr buffer
    newline
    printstr msg2
    mov ah, 01h
    int 21h
    mov [symbol], al
    newline
    
    xor bx, bx
    mov si, buffer
    cycle:
        mov dl, [si]
        mov al, [si+1]
        cmp al, 36
        je breakl
        checkflag [foundflag]
        jne checksymbol
        replaceby [si+1]
        jmp skip
        checksymbol:
        cmp dl, [symbol]
        jne skip
        mov [foundflag], 1
        replaceby [si+1]
        skip: 
        inc bl
        inc si
        jmp cycle
    breakl:
    inc bl
    mov [size], bl
    checkflag [foundflag]
    jne skipreplace
    replaceby [symbol]
    skipreplace:
    printstr msg3
    newline
    printstr buffer

    mov ah, 3ch
    mov dx, outfile
    xor cx, cx
    int 21h
    jnc @F
    call error_msg
    jmp exit
@@: mov [handle], ax

    mov bx, ax
    mov ah, 40h
    mov dx, buffer
    movzx cx, [size]
    int 21h
    jnc close_file2
    call error_msg
 
close_file2:
    mov ah, 3eh
    mov bx, [handle]
    int 21h
    jnc exit
    call error_msg

exit:
    mov ax, 4C00h
    int 21h

error_msg:
    mov ah, 09h
    mov dx, s_error
    int 21h
    ret
    
printdec:
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