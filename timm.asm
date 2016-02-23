.486
.model flat
extrn __fltused:near
extrn __ftol:near
_TEXT segment
align 4
L2 label byte
db 00H
db 1 dup (0)
dw 0
dd 0
dd 0
dd 00H
dd 00H
dd 00H
dd 00H
db 116
db 101
db 115
db 116
db 3 dup (0)
db 1 dup (0)
db 4 dup (0)
public _misc
_TEXT ends
_TEXT segment
_misc:
push ebx
push esi
push edi
push ebp
mov ebp,esp
sub esp,44
mov edi,dword ptr (24)[ebp]
mov ebx,edi
mov byte ptr (24)[ebp],bl
;;unsigned long misc(const char *a, const char b) {
;;	Foo st = { 0, 0, 0, 0, 0, 0, NULL, { 't', 'e', 's', 't' } };
lea edi,(-40)[ebp]
lea esi,(L2)
mov ecx,40
rep movsb
;;	switch (st.c) {
mov bl,byte ptr (-40)[ebp]
movzx edi,bl
mov dword ptr (-44)[ebp],edi
mov edi,dword ptr (-44)[ebp]
mov dword ptr (?)[ebp],edi
cmp dword ptr (?)[ebp],0
jl L3
cmp dword ptr (?)[ebp],5
jg L3
mov edi,dword ptr (?)[ebp]
jmp dword ptr (L13)[edi*4]
_TEXT ends
_TEXT segment
align 4
L13 label byte
dd L6
dd L9
dd L6
dd L9
dd L6
dd L9
_TEXT ends
_TEXT segment
L6:
;;		memcpy(st.a, "even", 5);
push 5
lea edi,(L8)
push edi
lea edi,(-12)[ebp]
push edi
call _memcpy
add esp,12
;;		break;
jmp L4
L9:
;;		memcpy(st.a, "odd", 4);
push 4
lea edi,(L11)
push edi
lea edi,(-12)[ebp]
push edi
call _memcpy
add esp,12
;;		break;
jmp L4
L3:
;;		st.bit = false;
and dword ptr (-4)[ebp],0fffffffeH
;;		break;
L4:
;;	return -64;
mov edi,-64
mov eax,edi
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
;;int main(int argc, const char *argv[]) {
;;	return 0;
mov eax,0
L14:
mov esp,ebp
pop ebp
pop edi
pop esi
pop ebx
ret
_TEXT ends
extrn _memcpy:near
_TEXT segment
_TEXT ends
_TEXT segment
align 1
L11 label byte
db 111
db 100
db 100
db 0
align 1
L8 label byte
db 101
db 118
db 101
db 110
db 0
_TEXT ends
end
