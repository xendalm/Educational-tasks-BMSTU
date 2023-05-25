assume cs: code, ds: data
data segment
    string db "Hello world!$"
    symbol db 0
    foundflag db 0
    msg1 db "Source string: $"
    msg2 db "Symbol to find: $"
    msg3 db "Result: $"
data ends
code segment
start:  
    include lib.inc
    mov ax, data
    mov ds, ax
    printstr msg1
    printstr string
    newline
    printstr msg2
    mov ah, 01h
    int 21h
    mov symbol, al
    newline
    
    mov si, offset string
    cycle:
        mov dl, byte ptr [si]
        cmp byte ptr [si+1], 36
        je breakl
        checkflag foundflag
        jne checksymbol
        replaceby [si+1]
        jmp skip
        checksymbol:
        cmp dl, symbol
        jne skip
        mov foundflag, 1
        replaceby [si+1]
        skip: 
        inc si
        jmp cycle
    breakl:
    checkflag foundflag
    jne skipreplace
    replaceby symbol
    skipreplace:
    printstr msg3
    printstr string
;end
    mov ax,4c00h
    int 21h
code ends
end start