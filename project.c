#include "spimcore.h"

// Memory size.
#define MEMSIZE (65536 >> 2)
// Save location before performing jump.
static unsigned savePC;
// Tells us if jump happened or not.
static int jumpHappen;

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Switch statements throughout the whole code for 
    // a better organization when it comes to different 
    // cases

    // Performs operation.
    switch (ALUControl) {
        case 0x0:
            *ALUresult = A + B;
            break;

        case 0x1:
            *ALUresult = A - B;
            break;

        case 0x2:
            if((int)A < (int)B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;

        case 0x3:
            if((unsigned)A < (unsigned)B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
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

    // Sets the Zero value.
    if(*ALUresult == 0)
        *Zero == 1;
    else
        *Zero == 0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Checks if PC is going beyond memory and if it is unaligned.
    if ((PC / 4) >= MEMSIZE || PC % 4 != 0){
        return 1;
    }
    
    // Fetches and saves the instruction from Mem into "instruction."
    *instruction = Mem[PC >> 2];

    // Checks if instruction is invalid.
    if (*instruction == 0) {
        return 1;
    }

    return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    //  Gets each variable to the spot they need to be.
    *op = (instruction >> 26); 
    *r1 = (instruction >> 21);
    *r2 = (instruction >> 16);
    *r3 = (instruction >> 11);

    // Saves the necessary part. 
    *op = *op & 0x3F; 
    *r1 = *r1 & 0x1F;
    *r2 = *r2 & 0x1F;
    *r3 = *r3 & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{    
    // Finds the appropriate op code and saves the action
    // appropriate result in each part of the control. Also
    // halts if the op code is illegal.
    switch (op) {
        // R-type
        case 0x0: 
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
        
        // lw
        case 0x23: 
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
        
        // sw
        case 0x2B: 
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
        
        // beq
        case 0x4: 
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
        
        // j
        case 0x2: 
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
        
        // addi
        case 0x8: 
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
        
        // slti
        case 0xa:   
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
        
        // sltiu
        case 0xB:   
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
        
        // lui
        case 0x0F: 
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
        
        // Halt
        default:
            return 1;
    }

    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // Reads and saves register.
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}

/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // Extends offset into extended_value according to whether it is negative or not.
    if(offset & 0x8000)
        *extended_value = offset | 0xFFFF0000;
    else
        *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    unsigned A, B;
    char ALUControl;
    
    A = data1;
    
    // Saves different data in B depending on ALUSrc.
    if (ALUSrc == 0)
        B = data2;
    else       
        B = extended_value;

    // Saves ALUControl according to ALUOp and if ALUOp
    // is not found, it halts.    
    switch(ALUOp){
        case 0x0:
            ALUControl = 0x0; 
            break;
        
        case 0x1:
            ALUControl = 0x1; 
            break;
        
        case 0x2:
            ALUControl = 0x2; 
            break;
        
        case 0x3:
            ALUControl = 0x3; 
            break;
        
        case 0x4:
            ALUControl = 0x4; 
            break;
        
        case 0x5:
            ALUControl = 0x5; 
            break;
        
        case 0x6:
            ALUControl = 0x6; 
            break;
        
        case 0x7:
            switch(funct){
                case 0x20:
                    ALUControl = 0x0;
                    break;
                
                case 0x22:
                    ALUControl = 0x1;
                    break;
                
                case 0x24:
                    ALUControl = 0x4;
                    break;
                
                case 0x25:
                    ALUControl = 0x5;
                    break;
                
                case 0x2a:
                    ALUControl = 0x2;
                    break;
                
                case 0x2b:
                    ALUControl = 0x3;
                    break;
                
                default:
                    return 1;
            } 
            break;
        
        default:
            return 1;
    }

    // Calls ALU function.
    ALU(A, B, ALUControl, ALUresult, Zero);

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // Halts the function if both are active at the same time.
    if(MemWrite == 1 && MemRead == 1){
        return 1;
    }
    // Writes
    else if(MemWrite == 1){
        // Halts if it is unaligned. 
        if (ALUresult % 4 != 0) {
            return 1;
        }
        Mem[ALUresult / 4] = data2;
    }
    // Reads
    else if(MemRead == 1){
        // Halts if it is unaligned. 
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
    unsigned write_register;

    // Writes into register.
    if (RegWrite) {
        //Determines what data to write acording to MemtoReg.
        if(MemtoReg)
            write_data = memdata;
        else
            write_data = ALUresult;
        
        // Determines where to write the data according to RegDst.
        if(RegDst)
            write_register = r3;
        else
            write_register = r2;
        
        // Writes the data into the register.
        Reg[write_register] = write_data;
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{   
    *PC = *PC + 4;

    //Makes the code go back after the jump.
    if(jumpHappen == 1){
        *PC = savePC;
        jumpHappen = 0;
    }
    //Branch and Zero 
    else if(Branch && Zero){
        *PC = *PC + extended_value;
    }
    //Makes the code perform the jump and also saves where the 
    //code jumped from
    else if(Jump){
        savePC = *PC;
        jumpHappen = 1;
        *PC = (jsec << 2) | (*PC & 0xF0000000);
    }
}
