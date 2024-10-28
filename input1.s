.data
.dword 0x100 0x1 34 23


.text


lui x4, 0x10
ld x3, 32(x4)

.data

.dword 0x123


.text

addi x2, x3, 34
lb x5, 32(x4)