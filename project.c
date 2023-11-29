#include "spimcore.h"

#define MEMSIZE (65536 >> 2)

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    printf("%c \n", ALUControl);
    
    switch (ALUControl) {
        case 0:
            *ALUresult = A + B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 1:
            *ALUresult = A - B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 2:
            *ALUresult = (A < B) ? 1 : 0;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 3:
            *ALUresult = ((unsigned)A < (unsigned)B) ? 1 : 0;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 4:
            *ALUresult = A & B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 5:
            *ALUresult = A | B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 6:
            *ALUresult = B << 16;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 7:
            *ALUresult = ~A;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
    }

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0) {
        return 1;
    }

    *instruction = Mem[PC >> 2];

    if (*instruction == 0 || (PC / 4) >= MEMSIZE) {
        return 1;
    }

    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    *op = (instruction >> 26) & 0x3F; 
    *r1 = (instruction >> 21) & 0x1F;
    *r2 = (instruction >> 16) & 0x1F;
    *r3 = (instruction >> 11) & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    switch (op) {
        case 0x0: // R-type
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0b111;
            controls->RegDst = 1;
            break;
        case 0x8: // addi
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0b000;
            controls->RegDst = 0;
            break;
        case 0x23: // lw
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemtoReg = 1;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0b000;
            controls->RegDst = 0;
            break;
        case 0x2B: // sw
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0b000;
            controls->RegDst = 0;
            break;
        case 0x4: // beq
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 1;
            controls->Jump = 0;
            controls->ALUOp = 0b010;
            controls->RegDst = 0;
            break;
        case 0x2: // j
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 1;
            controls->ALUOp = 0b000;
            controls->RegDst = 0;
            break;
        default: // Halt for illegal instruction
            return 1;
    }
    return 0;

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    *extended_value = (offset & 0x8000) ? (offset | 0xFFFF0000) : offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    unsigned A, B;
    char ALUControl;

    if (ALUSrc == 0) {
        A = data1;
        B = data2;
    } else {
        A = data1;
        B = extended_value;
    }

    if (ALUOp == 0b000) {
        ALUControl = 0b000;
    } else if (ALUOp == 0b001) { 
        ALUControl = 0b001;
    } else if (ALUOp == 0b010) { 
        ALUControl = 0b010;
    } else if (ALUOp == 0b011) {
        ALUControl = 0b011;
    } else if (ALUOp == 0b100) {
        ALUControl = 0b100;
    } else if (ALUOp == 0b101) {
        ALUControl = 0b101;
    } else if (ALUOp == 0b110) {
        ALUControl = 0b110;
    } else if (ALUOp == 0b111) {
        if (funct == 0b000000) {
            ALUControl = 0b000;
        } else if (funct == 0b000010) {
            ALUControl = 0b010;
        } else {
            ALUControl = 0b000; 
        }
    } else {
        ALUControl = 0b000; 
    }

    ALU(A, B, ALUControl, ALUresult, Zero);

    printf("ALU Operation: A=%u, B=%u, ALUControl=%u, ALUResult=%u, Zero=%d\n", A, B, ALUControl, *ALUresult, *Zero);

    if(*ALUresult >> 2 >= MEMSIZE){
        printf("triggered \n");
        return 1;
    }

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(ALUresult / 4 >= MEMSIZE){
        return 1;
    }
    
    if(MemWrite == 1){
        Mem[ALUresult / 4] = data2;
    }
    else if(MemRead == 1){
        *memdata = Mem[ALUresult / 4];
    }

    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    unsigned write_data;

    if (RegWrite) {
        write_data = (MemtoReg) ? memdata : ALUresult;
        unsigned write_register = (RegDst) ? r3 : r2;
        Reg[write_register] = write_data;
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC = *PC + 4;

    if(Branch && Zero){
        *PC = *PC + extended_value;
    }
    else if(Jump){
        *PC = (jsec << 2) | (*PC & 0xF0000000);
    }
}


