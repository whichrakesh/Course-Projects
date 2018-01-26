.data
A:    .word 4
B:    .word 8
C:    .word 0

.text
main:

ld r1,A(r0)
dadd r2,r1,r1   # Data hazard in EX

exit:
        daddi r1,r1,-1
        bne r1,r0,exit  # RAW stall in ID
daddi r1,r0,10  # control hazard due to branch not taken

lol:
        daddi r2,r2,-4   # INSTR18
        daddi r1,r1,-2   # INSTR19
        bne r2,r0,lol    # data forwarding from EX stage of INSTR18 to ID  stage of this instruction

daddi r2,r2,4
beq r1,r0,baklol         # data forwarding from MEM stage of INSTR19 to ID  stage of this instruction
        daddi r1,r1,-4
baklol:

ld r1,C(r0)
bne r1,r0,baklol         # 2 stalls between these pair of instructions       

ld r1,B(r0)              # INSTR30
daddi r2,r0,1
daddi r2,r2,2
bne r1,r0,lolol          # Data forwarding from MEM stage of INSTR30 to ID stage of this instruction
daddi r2,r2,4
lolol:  

ddiv r1,r2,r2
dadd r1,r2,r2           # structural and waw stall between the previous and this instrution 

halt          


