assume cs: code, ds: data
data segment
    n equ 100 ;max len
    base equ 10 ;10 / 16
    string1 db n, n+1 dup ('$')
    string2 db n, n+1 dup ('$')
    sumres db n*2, n*2 dup ('0')
    subres db n, n dup ('0')
    mulres db n*2, n*2 dup (0)
    sumresmsg db "Sum res: $"
    mulresmsg db "Mul res: $"
    dummy db 0Ah, '$'
    errormsg db "invalid character$"
    len1 dw ?
    len2 dw ?
    string1neg db 1
    string2neg db 1
    negsign db ''
    swapflag db 0
data ends
code segment
start:  
    mov ax, data
    mov ds, ax
    mov es, ax
    mov dx, offset string1
    mov ah, 0Ah
    int 21h
    call newline
    mov dx, offset string2
    mov ah, 0Ah
    int 21h
    call newline
;neg check
    cmp string1[2], '-'
    jne skipnegcheck1
    mov string1neg, byte ptr 0
    mov string1[2], '0'
    skipnegcheck1:
    cmp string2[2], '-'
    jne skipnegcheck2
    mov string2neg, byte ptr 0
    mov string2[2], '0'
    skipnegcheck2:
;check
    mov dx, offset string1
    inc dx
    push dx
    call check
    mov dx, offset string2
    inc dx
    push dx
    call check
    xor ax, ax
    mov al, string1[1]
    mov len1, ax
    mov al, string2[1]
    mov len2, ax
;lencmp and swap
    xor ax, ax     
    xor bx, bx
    mov ax, len1
    mov bx, len2
    cmp ax, bx
    jg skipswap
        jne skipcmp
        mov si, 2
        mov cx, len1
        cmpcycle:
            mov al, string1[si]
            cmp al, string2[si]
            jl skipcmp
            jg skipswap
            inc si
        loop cmpcycle
        jmp skipswap
        skipcmp:
        mov si, 1
        mov cx, n+1
        swap:
            mov al, string1[si]
            mov bl, string2[si]
            mov string1[si], bl
            mov string2[si], al
            inc si
        loop swap
        mov swapflag, byte ptr 1
    skipswap:
;shift
    xor ax, ax
    mov al, string1[1]
    mov len1, ax
    mov al, string2[1]
    mov len2, ax
    mov ax, len1
    mov bx, len2
    cmp ax, bx
    je skipshift
        std
        mov si, offset string2
        add si, word ptr len2
        inc si
        mov di, offset string2
        add di, word ptr len1
        inc di
        mov cx, len2
        shiftcycle1:
            lodsb
            stosb 
            loop shiftcycle1
        mov si, offset string2
        add si, 2
        mov cx, len1
        sub cx, len2
        shiftcycle2:
            mov byte ptr [si], 0
            inc si
            loop shiftcycle2
    skipshift:
;sum/sub
    mov dx, offset string1
    inc dx
    push dx
    mov dx, offset string2
    inc dx
    push dx
    mov ah, 09h
    mov dx, offset sumresmsg
    int 21h
    mov al, string1neg
    mov bl, string2neg
    cmp al, bl
    je samesign1
    call subfunc
    mov dx, offset subres
        mov al, string1neg
        cmp al, 1
        je positive1
            mov al, swapflag
            cmp al, 1
            je positive
            mov negsign, byte ptr '-'
            jmp show1
        positive1:
            mov al, swapflag
            cmp al, 1
            jne positive
            mov negsign, byte ptr '-'
            jmp show1
    samesign1:
    call sumfunc
    mov dx, offset sumres
        mov al, string1neg
        cmp al, 1
        je positive
        mov negsign, byte ptr '-'
        jmp show1
    positive:
    mov negsign, byte ptr ''
    show1:
    inc dx
    push dx
    call showans
;mul
    mov dx, offset string1
    inc dx
    push dx
    mov dx, offset string2
    inc dx
    push dx
    call mulfunc
    call newline
    mov ah, 09h
    mov dx, offset mulresmsg
    int 21h
    mov dx, offset mulres
    inc dx
    push dx
    mov al, string1neg
    mov bl, string2neg
    cmp al, bl
    je samesign2
    mov negsign, byte ptr '-'
    jmp show2
    samesign2:
    mov negsign, byte ptr ''
    show2:
    call showans
;end
    mov ax,4c00h
    int 21h
;sum pcoc
sumfunc proc
    push bp
    mov bp, sp
    ;reset
    mov sumres[0], n*2
    mov si, 1
    mov cx, n*2
    resetsumres:
        mov sumres[si], '0'
        inc si
        loop resetsumres
    xor ax, ax
    mov si, [bp+6]
    mov al, byte ptr [si]
    mov len1, ax
    mov si, [bp+4]
    mov al, byte ptr [si]
    mov len2, ax
    xor ax, ax
    xor bx, bx
    xor dx, dx
    mov si, [bp+6]
    add si, len1
    mov di, [bp+4]
    add di, len1
    mov cx, len1
    mov ax, cx
    sumcycle:
        xor ax, ax
        mov al, byte ptr [si]
        mov bl, byte ptr [di]
        add al, bl
        add al, dl
        mov bl, base
        div bl
        push si
            sub si, [bp+6]
            add sumres[si+1], ah
        pop si
        mov dl, al
        dec si
        dec di
    loop sumcycle
    sub si, [bp+6]
    add sumres[si+1], dl
    mov si, len1
    mov sumres[si+2], 36
    pop bp
    ret
sumfunc endp
;sub pcoc
subfunc proc
    push bp
    mov bp, sp
    ;reset
    mov subres[0], n*2
    mov si, 1
    mov cx, n
    resetsubres:
        mov subres[si], '0'
        inc si
        loop resetsubres
    xor ax, ax
    mov si, [bp+6]
    mov al, byte ptr [si]
    mov len1, ax
    mov si, [bp+4]
    mov al, byte ptr [si]
    mov len2, ax
    xor ax, ax
    xor bx, bx
    xor dx, dx
    mov si, [bp+6]
    add si, len1
    mov di, [bp+4]
    add di, len1
    mov cx, len1
    subcycle:
        xor ax, ax
        mov al, byte ptr [si]
        mov bl, byte ptr [di]
        sub al, dl 
        mov dl, 0  
        cmp al, bl
        jge greater
                mov dl, 1
                add al, base
        greater:
        sub al, bl
        add subres[si-1], al
        dec si
        dec di
    loop subcycle
    mov si, len1
    mov subres[si+1], 36
    pop bp
    ret
subfunc endp
;mul pcoc
mulfunc proc
    push bp
    mov bp, sp
    ;reset
    mov mulres[0], n*2
    mov si, 1
    mov cx, n*2
    resetmulres:
        mov mulres[si], 0
        inc si
    loop resetmulres
    ;update len
    xor ax, ax
    mov si, [bp+6]
    mov al, byte ptr [si]
    mov len1, ax
    add si, len1
    mov di, [bp+4]
    mov al, byte ptr [di]
    mov len2, ax
    add di, len1
    mov ax, len1
    shl ax, 1
    mov mulres[0], al
    mov cx, len2
    mulcycle1:
        push cx
        xor ax, ax
        xor bx, bx
        xor dx, dx
        mov si, [bp+6]
        add si, len1
        mov cx, len1
        mulcycle2:
            xor ax, ax
            mov al, byte ptr [si]
            mov bl, byte ptr [di]
            mul bl
            add al, dl
            mov bl, base
            div bl
            push si
            push di
                sub si, [bp+6]
                add si, di
                sub si, [bp+4]
                add mulres[si], ah
            pop di
            pop si
            mov dl, al
            dec si
        loop mulcycle2
        push di
            sub si, [bp+6]
            add si, di
            sub si, [bp+4]
            add mulres[si], dl
        pop di
        pop cx
        dec di
        loop mulcycle1
    mov ax, len1
    shl ax, 1
    dec ax
    mov cx, ax
    mov si, 1
    add si, ax
    mov mulres[si+1], 36
    xor ax, ax
    xor bx, bx
    xor dx, dx
    mulcycle:
        xor ax, ax
        mov al, mulres[si]
        add al, dl
        mov bl, base
        div bl
        mov mulres[si], ah
        add mulres[si], '0'
        mov dl, al
        dec si
    loop mulcycle
    add mulres[1], dl
    add mulres[1], '0'
    pop bp
    ret
mulfunc endp
;showans
showans proc
    push bp
    mov bp, sp
    mov si, [bp+4]
    mov ax, base
    cmp ax, 16
    jne @base10
    mov cx, n*2
    makeanscycle:
        cmp byte ptr [si], 36
        je @base10
        cmp byte ptr [si], '9'
        jle notletter
        add byte ptr [si], 7
        notletter:
        inc si
    loop makeanscycle
    @base10:
    mov si, [bp+4]
    mov cx, n*2
    shiftanscycle:
        cmp byte ptr [si], 36
        jne skip
        dec si
        mov negsign, byte ptr ''
        jmp breakshiftans
        skip:
        cmp byte ptr [si], '0'
        jne breakshiftans
        inc si
        loop shiftanscycle
    breakshiftans:
;sign
    mov ah, 02h
    mov dl, negsign
    int 21h
;ans
    mov ah, 09h
    mov dx, si
    int 21h
    pop bp
    ret
showans endp
;check string
check proc
    push bp
    mov bp, sp
    mov si, [bp+4]
    xor cx, cx
    mov cl, byte ptr [si]
    inc si
    cycle:
        cmp byte ptr [si], '0'
        jl @errorl
        mov ax, base
        cmp ax, 16
        jne @check10
        cmp byte ptr [si], 'F'
        jg @errorl
        cmp byte ptr [si], '9'
        jle @check10
        cmp byte ptr [si], 'A'
        jl @errorl
        sub byte ptr [si], 55
        jmp @skip
        @check10:
        cmp byte ptr [si], '9'
        jg @errorl
        sub byte ptr [si], '0'
        @skip:
        inc si
    loop cycle
    jmp @label
    @errorl:
    call error
    @label:
;shift
    mov si, [bp+4]
    xor cx, cx
    mov cl, byte ptr [si]
    xor bx, bx
    xor ax, ax
    inc si
    countzero:
        mov al, byte ptr [si]
        cmp byte ptr [si], 0
        jne @breakl
        inc bx
        inc si
    loop countzero
    @breakl:
    mov ax, bx
    mov si, [bp+4]
    cmp byte ptr [si], bl
    jne noteq
    dec bl
    noteq:
    sub byte ptr [si], bl
    xor cx, cx
    mov cl, byte ptr [si]
    inc si
    mov di, [bp+4]
    inc di
    add si, bx
    cld
    shiftzero:
        lodsb
        stosb
    loop shiftzero
    pop bp
    ret
check endp
;error msg
error proc
    mov ah, 09h
    mov dx, offset dummy
    int 21h
    mov dx, offset errormsg
    int 21h
    mov ax,4c00h
    int 21h
error endp
;newline
newline proc
    push ax
    push dx
    mov ah, 02h
    mov dl, 10
    int 21h
    mov dl, 13
    int 21h
    pop dx
    pop ax
    ret
newline endp
;print
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