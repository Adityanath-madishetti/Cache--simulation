.data
.dword 10, 20, 30, 40, 50
.text
lui x3, 0x10
lb x4, 0(x3)
lb x4, 8(x3)
lb x4, 16(x3)
lb x4, 24(x3)
lb x4, 32(x3)