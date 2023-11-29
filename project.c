#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0b000:
            *ALUresult = A + B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b001:
            *ALUresult = A - B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b010:
            *ALUresult = (A < B) ? 1 : 0;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b011:
            *ALUresult = ((unsigned)A < (unsigned)B) ? 1 : 0;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b100:
            *ALUresult = A & B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b101:
            *ALUresult = A | B;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b110:
            *ALUresult = B << 16;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
        case 0b111:
            *ALUresult = ~A;
            *Zero = (*ALUresult == 0) ? 1 : 0;
            break;
    }

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    *instruction = Mem[PC / 4];

    if (*instruction == 0 || (PC % 4 != 0) || (PC >= 0xFFFF) || ((PC / 4) >= MEMORY_SIZE)) {
        Halt = true;
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
    
    
    if (*instruction == 0 || (PC % 4 != 0) || (PC >= 0xFFFF) || ((PC / 4) >= MEMORY_SIZE)) {
        Halt = true;
        return 1;
    }
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
    //This could be wrong.
    *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

