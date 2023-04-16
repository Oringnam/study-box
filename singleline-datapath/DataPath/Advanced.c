#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

/**************************************/
struct Control
{
	unsigned char RegDst;
	unsigned char Jump;
	unsigned char Branch;
	unsigned char MemRead;
	unsigned char MemtoReg;
	unsigned char ALUOp;
	unsigned char MemWrite;
	unsigned char ALUSrc;
	unsigned char RegWrite;
};

struct Reg_Data
{
	unsigned int Read_data_1;
	unsigned int Read_data_2;

	unsigned int Write_location;
};

struct Mem_Data
{
	unsigned int Read_data;
	unsigned int Write_data;
	unsigned int Move_data;
};

struct ALU
{
	unsigned char zero; // 1: enable, 0: disable
	unsigned int result;
};

struct Control control;
struct Reg_Data reg_data;
struct Mem_Data mem_data;
struct ALU alu;
unsigned int mem[64] = { 0 };
unsigned int reg[32] = { 0 };

/**************************************/

unsigned int Sign_Extend(unsigned int);					// 부호 확장
unsigned int Shift_Left_2(unsigned int);				// 비트 2 증가
unsigned int Inst_Fetch(char [], unsigned int);			// IF
void print_reg_mem();									// 출력
void Register_Read(unsigned int, unsigned int);			// 레지스터 읽기
void ALU_func(unsigned char, unsigned int, unsigned int);		// ALU 컨트롤
void Mux(char [], unsigned char, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);		// Mux
unsigned int Add(unsigned int, unsigned int );			// Add

/**************************************/

unsigned int pc = 0;		// 현재 주소
unsigned int pc_add_4 = 0;	// 현재 주소 + 4

int main()
{
	FILE *fp;
	unsigned int inst = 0;
	unsigned int inst_31_26 = 0;
	unsigned int inst_25_21 = 0;
	unsigned int inst_20_16 = 0;
	unsigned int inst_15_11 = 0;
	unsigned int inst_15_0 = 0;
	unsigned int inst_25_0 = 0;
	int total_cycle = 0;
	char name[100] = { "input_*.txt" };
	char num;


	// file open
	printf("file name : input_*.txt\n");
	printf("number : ");
	scanf("%c", &num);
	name[6] = num;
	printf("입력받은 파일 : %s\n", name);

	// register initialization
	/**************************************/
	reg[8] = 41621;
	reg[9] = 41621;
	reg[16] = 40;
	/**************************************/

	// memory initialization
	/**************************************/
	mem[40] = 3578;

	if (!(fp = fopen(name, "r")))
	{
		printf("error: file open fail !!\n");
		exit(1);
	}

	while (feof(fp) == 0)
	{
		fscanf(fp, "%x", &inst);
		mem[pc] = inst;
		pc = pc + 4;
	}
	/**************************************/
	
	// control initialization
	/**************************************/
	control.RegDst = 0;
	control.Jump = 0;
	control.Branch = 0;
	control.MemRead = 0;
	control.ALUOp = 0;
	control.MemWrite = 0;
	control.ALUSrc = 0;
	control.RegWrite = 0;
	/**************************************/

	print_reg_mem();	// 초기 출력
	printf("\n ***** Processor START !!! ***** \n");

	pc = 0;

	while (pc < 64)
	{
		/*		instruction fetch		*/
		/********************************/
		// pc + 4
		pc = pc_add_4;
		pc_add_4 = pc + 4;

		inst = Inst_Fetch(name, pc);
		printf("Instruction = %08x \n", inst);
		/********************************/


		/*		 instruction decode			*/
		/********************************/
		inst_31_26 = inst >> 26;					//	OP code
		inst_25_21 = (inst & 0x03e00000) >> 21;		//	rs
		inst_20_16 = (inst & 0x001f0000) >> 16;		//	rt
		inst_15_11 = (inst & 0x0000f800) >> 11;		//	rd
		inst_15_0 = inst & 0x0000ffff;				//	I-format
		inst_25_0 = inst & 0x03ffffff;				//	J-format


		switch (inst_31_26)	// RegDst
		{
			case 0	:	printf(" >>> Add\n");	control.RegDst = 1; control.Jump = 0; break;			// Add
			case 2	:	printf(" >>> Jump\n");	control.RegDst = 0; control.Jump = 1; break;			// Jump
			case 4	:	printf(" >>> BEQ\n");	control.RegDst = 0; control.Jump = 0; break;			// BEQ
			case 35	:	printf(" >>> LW\n");	control.RegDst = 0;	control.Jump = 0; break;			// LW
			case 43	:	printf(" >>> SW\n");	control.RegDst = 0; control.Jump = 0; break;			// SW
		}

		// RegDst
		Mux("RegDst",control.RegDst,inst_25_21,inst_20_16,inst_15_11,inst_15_0,inst_25_0);
		// Jump
		Mux("Jump", control.Jump, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);

		/********************************/

		/*			 Execution			*/
		/********************************/
		Register_Read(inst_25_21, inst_20_16);
		switch (inst_31_26)	// RegDst
		{
			case 0	:	control.ALUOp = 10;	control.ALUSrc = 0; control.Branch = 0; break;			// Add
			case 2	:	control.ALUOp = 00;	control.ALUSrc = 0;	control.Branch = 0; break;			// Jump
			case 4	:	control.ALUOp = 01; control.ALUSrc = 0;	control.Branch = 1; break;			// BEQ
			case 35	:	control.ALUOp = 00; control.ALUSrc = 1;	control.Branch = 0; break;			// LW
			case 43	:	control.ALUOp = 00; control.ALUSrc = 1;	control.Branch = 0; break;			// SW
		}
		// ALUSrc 
		Mux("ALUSrc",control.ALUSrc, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);	
		// ALUOp
		Mux("ALUOp", control.ALUOp, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);
		// Branch
		Mux("Branch", control.ALUOp, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);

		/********************************/

		/*			 Memory			*/
		/********************************/
		switch (inst_31_26)	// RegDst
		{
			case 0	:	control.MemRead = 0; control.MemWrite = 0; break;		// Add
			case 2	:	control.MemRead = 0; control.MemWrite = 0; break;		// Jump
			case 4	:	control.MemRead = 0; control.MemWrite = 0; break;		// BEQ
			case 35	:	control.MemRead = 1; control.MemWrite = 0; break;		// LW
			case 43	:	control.MemRead = 0; control.MemWrite = 1; break;		// SW
		}

		// MemRead
		Mux("MemRead", control.MemRead, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);
		// MemWrite
		Mux("MemWrite", control.MemWrite, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);
		

		/********************************/



		/*			 Write Back		*/
		/********************************/
		switch (inst_31_26)	// RegDst
		{
			case 0	:	control.MemtoReg = 0; control.RegWrite = 1; break;		// Add
			case 2	:	control.MemtoReg = 0; control.RegWrite = 0; break;		// Jump
			case 4	:	control.MemtoReg = 0; control.RegWrite = 0; break;		// BEQ
			case 35	:	control.MemtoReg = 1; control.RegWrite = 1; break;		// LW
			case 43	:	control.MemtoReg = 0; control.RegWrite = 0; break;		// SW
		}

		// MemtoReg
		Mux("MemtoReg", control.MemtoReg, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);
		// RegWrite
		Mux("RegWrite", control.RegWrite, inst_25_21, inst_20_16, inst_15_11, inst_15_0, inst_25_0);

		/********************************/

		total_cycle++;

		// result
		/********************************/
		printf("PC : %d \n", pc);
		printf("Total cycle : %d \n", total_cycle);
		print_reg_mem();
		/********************************/
	} // while - main process

	printf("\n ***** Processor END !!! ***** \n");

	fclose(fp);
	return 0;
}

void Register_Read(unsigned int read_reg_1, unsigned int read_reg_2)
{
	reg_data.Read_data_1 = reg[read_reg_1];
	reg_data.Read_data_2 = reg[read_reg_2];
}

// MUX
void Mux(char signal[], unsigned char control, unsigned int rs,unsigned int rt, unsigned int rd, unsigned int Ioffset,unsigned int Joffset)
{
	if (signal == "RegDst") {
		if (control == 0)		reg_data.Write_location = rt;		// RegDst = 0	rt에 저장
		else if(control == 1)	reg_data.Write_location = rd;		// RegDst = 1	rd에 저장
	}	// RegDst

	if (signal == "ALUSrc") {
		if (control == 0)		reg_data.Read_data_2 = reg_data.Read_data_2;		// Read data 2			-> ALU 두번째 피 연산자
		else if (control == 1)	reg_data.Read_data_2 = Sign_Extend(Ioffset);		// 부호 확장된 Ioffset	-> ALU 두번째 피 연산자
	}	// ALUSrc

	if (signal == "ALUOp") {
		ALU_func(control, reg_data.Read_data_1, reg_data.Read_data_2);		// read data 1 : rs , read data 2 : rt or Ioffset
	}	// ALUOp

	if (signal == "MemRead") {
		if (control == 0)		return;										// MemRead = 0	 아무 일도 없음
		else if (control == 1) 	mem_data.Read_data = mem[alu.result];		// MemRead = 1	 메모리를 불러온다
	}	// MemRead

	if (signal == "MemWrite") {
		if (control == 0)		return;												// MemWrite = 0  아무 일도 없음
		else if (control == 1)	mem[alu.result] = reg[reg_data.Write_location];		// MemWrite = 1  메모리에 저장한다
	}	// MemWrite

	if (signal == "MemtoReg") {
		if (control == 0)			mem_data.Move_data = alu.result;			// MemtoReg = 0  ALU 출력	 -> 레지스터 Write data 입력													
		else if (control == 1) 		mem_data.Move_data = mem_data.Read_data;	// MemtoReg = 1  메모리 출력 -> 레지스터 Write data 입력
	}	// MemtoReg

	if (signal == "RegWrite") {
		if (control == 0)		return;													// RegWrite = 0		아무 일도 없음
		else if (control == 1) 	reg[reg_data.Write_location] = mem_data.Move_data;		// RegWrite = 1		레지스터에 Write data 씀
	}	// RegWrite

	if (signal == "Branch") {
		if (control == 0)		return;							// Branch = 0		아무 일도 없음
		else if (control == 1) {								// Branch = 1		브랜치 작업에 들어감
			if (alu.result == 1) {
				pc = pc_add_4 + Shift_Left_2(Sign_Extend(Ioffset));		// Branch 조건이 갖추어 지면 (rs = rt)
				pc_add_4 = pc;
			}
		}
	}	// Branch

	if (signal == "Jump") {
		if (control == 0)		return;										// Jump = 0		아무 일도 없음
		else if (control == 1) {											// Jump = 1		점프 작업에 들어감			
			pc = (0xf0000000 & pc_add_4) | Shift_Left_2(Joffset);			// Branch 조건이 갖추어 지면 (rs = rt)
			pc_add_4 = pc;			
		}
	}	// Jump
}	// Mux0

// ALU
void ALU_func(unsigned char control, unsigned int a, unsigned int b)
{
	// ALUOp = 10	R-type (Add)
	if (control == 10) {	
		alu.result = Add(a, b);
	}

	// ALUOp = 00	I-type (lw, sw)
	else if (control == 00) {
		alu.result = Add(a ,Sign_Extend(b));
	}
	// ALUOp = 01	Branch
	else if (control == 01) {
		if(a==b)	alu.result = 1;			// rs == rt
		else		alu.result = 0;			// rs != rt
	}
}

unsigned int Sign_Extend(unsigned int inst_16)
{
	unsigned int inst_32 = 0;
	if ((inst_16 & 0x00008000)) // minus
	{
		inst_32 = inst_16 | 0xffff0000;
	}
	else // plus
	{
		inst_32 = inst_16;
	}

	return inst_32;
}

unsigned int Shift_Left_2(unsigned int inst)
{
	return inst << 2;
}

void print_reg_mem()
{
	int reg_index = 0;
	int mem_index = 0;

	printf("\n===================== REGISTER =====================\n");

	for (reg_index = 0; reg_index < 8; reg_index++)
	{
		printf("reg[%02d] = %08d        reg[%02d] = %08d        reg[%02d] = %08d        reg[%02d] = %08d \n",
			reg_index, reg[reg_index], reg_index + 8, reg[reg_index + 8], reg_index + 16, reg[reg_index + 16], reg_index + 24, reg[reg_index + 24]);
	}

	printf("===================== REGISTER =====================\n");

	printf("\n===================== MEMORY =====================\n");

	for (mem_index = 0; mem_index < 32; mem_index = mem_index + 4)
	{
		printf("mem[%02d] = %012d        mem[%02d] = %012d \n",
			mem_index, mem[mem_index], mem_index + 32, mem[mem_index + 32]);
	}
	printf("===================== MEMORY =====================\n");
}

unsigned int Inst_Fetch(char name[], unsigned int pc)
{
	FILE *fp;
	unsigned int tmp;
	int i = 0;

	fp = fopen(name, "r");
	while (i <= pc) {
		fscanf(fp, "%x", &tmp);
		i += 4;
	}

	fclose(fp);
	return tmp;
}

unsigned int Add(unsigned int a, unsigned int b)
{
	return a + b;
}
