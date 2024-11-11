.data
.dword 10, 20, 30, 40, 50
.text
lui x3, 0x20
addi x4, x4, 4
sw x4, 0(x3)
lw x5, 0(x3)
sw x4, 0(x3)
addi x4, x4, 4
sw x4, 4(x3)
addi x4, x4, 4
sw x4, 8(x3)
addi x4, x4, 4
sw x4, 12(x3)
addi x4, x4, 4
sw x4, 16(x3)
addi x4, x4, 4
sw x4, 20(x3)
addi x4, x4, 4
sw x4, 24(x3)
addi x4, x4, 4
sw x4, 28(x3)
addi x4, x4, 4
sw x4, 32(x3)
addi x4, x4, 4
sw x4, 36(x3)
addi x4, x4, 4
sw x4, 40(x3)
addi x4, x4, 4
sw x4, 44(x3)
addi x4, x4, 4
