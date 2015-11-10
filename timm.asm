.486
.model flat
extrn __fltused:near
extrn __ftol:near
public _hello
_TEXT segment
_hello:
push ebx
push esi
push edi
push ebp
mov ebp,esp
sub esp,4
lea edi,(L2)
mov dword ptr (-4)[ebp],edi
L1:
mov esp,ebp
pop ebp
pop edi
pop esi
pop ebx
ret
public _main
_main:
push ebx
push esi
push edi
push ebp
mov ebp,esp
mov eax,0
L3:
mov esp,ebp
pop ebp
pop edi
pop esi
pop ebx
ret
_TEXT ends
_TEXT segment
align 1
L2 label byte
db 104
db 101
db 108
db 108
db 111
db 0
_TEXT ends
end
