#include "spimcore.h"

#define MEMSIZE (65536 >> 2)
static unsigned savePC;
static int jumpHappen;

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {
        case 0x0:
            *ALUresult = A + B;
            break;

        case 0x1:
            *ALUresult = A - B;
            break;

        case 0x2:
            *ALUresult = ((int)A < (int)B) ? 1 : 0;
            break;

        case 0x3:
            *ALUresult = ((unsigned)A < (unsigned)B) ? 1 : 0;
            break;

        case 0x4:
            *ALUresult = A & B;
            break;

        case 0x5:
            *ALUresult = A | B;
            break;

        case 0x6:
            *ALUresult = B << 16;
            break;

        case 0x7:
            *ALUresult = ~A;
            break;
    }

    if(*ALUresult == 0)
        *Zero == 1;
    else
        *Zero == 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0){
        return 1;
    }
    
    *instruction = Mem[PC >> 2];

    if ((PC / 4) >= MEMSIZE || *instruction == 0) {
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
            controls->ALUOp = 0x7;
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
            controls->ALUOp = 0x0;
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
            controls->ALUOp = 0x0;
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
            controls->ALUOp = 0x0;
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
            controls->ALUOp = 0x2;
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
            controls->ALUOp = 0x0;
            controls->RegDst = 0;
            break;
        case 0xa:   // slti
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0x2;
            controls->RegDst = 0;
            break;
        case 0xB:   // sltiu
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0x3;
            controls->RegDst = 0;
            break;
        case 0x0F: // lui
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0x6;
            controls->RegDst = 0;
            break;
        default:
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
    } 
    else {
        A = data1;
        B = extended_value;
    }

    if (ALUOp == 0x0) {
        ALUControl = 0x0;
    } 
    else if (ALUOp == 0x1) { 
        ALUControl = 0x1;
    } 
    else if (ALUOp == 0x2) { 
        ALUControl = 0x2;
    } 
    else if (ALUOp == 0x3) {
        ALUControl = 0x3;
    } 
    else if (ALUOp == 0x4) {
        ALUControl = 0x4;
    } 
    else if (ALUOp == 0x5) {
        ALUControl = 0x5;
    } 
    else if (ALUOp == 0x6) {
        ALUControl = 0x6;
    } 
    else if (ALUOp == 0x7) {
        if (funct == 0x20) {
            ALUControl = 0x0;
        } 
        else if (funct == 0x22) {
            ALUControl = 0x1;
        }
        else if (funct == 0x24) {
            ALUControl = 0x4;
        }
        else if (funct == 0x25) {
            ALUControl = 0x5;
        }
        else if (funct == 0x2a) {
            ALUControl = 0x2;
        }
        else if(funct == 0x2b){
            ALUControl = 0x3;
        } 
        else {
            return 1; 
        }
    } 
    else {
        return 1; 
    }

    ALU(A, B, ALUControl, ALUresult, Zero);

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(MemWrite == 1){
        if (ALUresult % 4 != 0) {
            return 1;
        }
        Mem[ALUresult / 4] = data2;
    }
    else if(MemRead == 1){
        if (ALUresult % 4 != 0) {
            return 1;
        }
        *memdata = Mem[ALUresult / 4];
    }

    return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    unsigned write_data;

    //change

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

    if(jumpHappen == 1){
        *PC = savePC;
        jumpHappen = 0;
    }
    else if(Branch && Zero){
        *PC = *PC + extended_value;
    }
    else if(Jump){
        savePC = *PC;
        jumpHappen = 1;
        *PC = (jsec << 2) | (*PC & 0xF0000000);
    }
}
