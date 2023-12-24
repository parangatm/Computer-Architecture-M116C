#include "CPU.h"

instruction::instruction(bitset<32> fetch)
{
	//cout << fetch << endl;
	instr = fetch;
	//cout << instr << endl;
}

CPU::CPU()
{
	PC = 0; //set PC to 0
	
	// Initializing all the performance variables to 0
	clock_cycles = 0;
	r_instr_count = 0;
	instr_count = 0;

	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}

	for (int j = 0; j < 32; j++)
	{
		reg_file[j] = 0; 
	}
}

bitset<32> CPU::Fetch(bitset<8> *instmem) {
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong()));  //get 32 bit instruction
	PC += 4;//increment PC
	return instr;
}


bool CPU::Decode(instruction* curr)
{
//cout<<curr->instr<<endl;
	uint32_t curr_instr;
	curr_instr = (uint32_t)(curr->instr.to_ulong());

	//parse bits from the instructions, each variable is made 32 bits for uniformity
	uint32_t opcode = (curr_instr >> 0) & 0x7F;    // Bits 0-6
	uint32_t rd = (curr_instr >> 7) & 0x1F;		  // Bits 7-11
	uint32_t funct3 = (curr_instr >> 12) & 0x7;    // Bits 12-14
	uint32_t rs1 = (curr_instr >> 15) & 0x1F;      // Bits 15-19
	uint32_t rs2 = (curr_instr >> 20) & 0x1F;      // Bits 20-24
	uint32_t funct7 = (curr_instr >> 25) & 0x7F;   // Bits 25-31

	// Accessing register file to read the registers specified
	decode_stage.rs1 = reg_file[rs1];
	decode_stage.rs2 = reg_file[rs2];
	decode_stage.rd = rd;

	// sign extension or Immediate generation
	decode_stage.imm = (int32_t)curr->instr.to_ulong() >> 20; 

	// Decoding the operation based on opcode, funct3 and funct7 values
	if (opcode == R_TYPE)
	{
		if (funct3 == 0x0)
		{
			if (funct7 == 0x00)
			{
				decode_stage.operation = "ADD";
			}
			else if (funct7 == 0x20)
			{
				decode_stage.operation = "SUB";
			}
			else
			{
				decode_stage.operation = "ERROR";	
			}
		}

		else if ((funct7 == 0x0) && (funct3 == 0x4))
		{
			decode_stage.operation = "XOR";
		}

		else if ((funct7 == 0x20) && (funct3 == 0x5))
		{
			decode_stage.operation = "SRA";
		}

		else
		{
			decode_stage.operation = "ERROR";
		}

		r_instr_count++;
		instr_count++;
	}

	else if (opcode == I_TYPE)
	{
		if (funct3 == 0x0)
		{
			decode_stage.operation = "ADDI";
		}

		else if (funct3 == 0x7)
		{
			decode_stage.operation = "ANDI";
		}	

		else
		{
			decode_stage.operation = "ERROR";
		}

		instr_count++;
	}

	else if ((opcode == LW_TYPE) && (funct3 == 0x2))
	{
		decode_stage.operation = "LW";
		instr_count++;
	}

	else if ((opcode == SW_TYPE) && (funct3 == 0x2))
	{
		//extracting and ordering bits for the immediate operand
		auto imm11_5 = (int32_t)(curr_instr & 0xfe000000);
		auto imm4_0 = (int32_t)((curr_instr & 0xf80) << 13);
		int32_t immediate = (imm11_5 + imm4_0) >> 20;
		decode_stage.imm = immediate;
		decode_stage.operation = "SW";
		instr_count++;
	}

	else if ((opcode == B_TYPE) && (funct3 == 0x4))
	{
		//extracting and ordering bits for the immediate operand
		auto imm0 = (int32_t)(0);  
		auto imm4_1 = (int32_t)((curr_instr & 0xf00) >> 7);
		auto imm10_5 = (int32_t)((curr_instr & 0x7e000000) >> 20);
		auto imm11 = (int32_t)((curr_instr & 0x80) << 4);
		auto imm12 = (int32_t)((curr_instr & 0x80000000) >> 19);
		decode_stage.imm = (int32_t)((imm12 + imm11 + imm10_5 + imm4_1 + imm0)); 
		
		//additional check when the immediate or the offset is negative to make the operand uniform for execute stage ALU
		if(imm12>0)
		{
			decode_stage.imm = (int32_t)(decode_stage.imm | 0xFFFFF000);
		}

		decode_stage.operation = "BLT";
		instr_count++;
	}

	else if ((opcode == J_TYPE) && (funct3 == 0x0))
	{
		decode_stage.operation = "JALR";
		instr_count++;
	}

	//Stop the execution when a ZERO opcode is encountered
	else if (opcode == ZERO)
	{
		decode_stage.operation = "END";
		instr_count++;
	}

	// All other cases not supported (yet) by the processor
	else
	{
		decode_stage.operation = "ERROR";
	}

	//cout << "Instruction #" << instr_count;
	//cout << " Decoded Operation: " << decode_stage.operation << endl;
	return true;
}

unsigned long CPU::readPC()
{
	return PC;
}

// Add other functions here ... 

void CPU::execute_instr()
{
	//Transfering variables from decode stage to execute stage, so dependency on decode stage is removed
	execute_stage.operation = decode_stage.operation;
	execute_stage.rs2 = decode_stage.rs2;
	execute_stage.rd = decode_stage.rd;

	//R-type instructions: ADD, SUB, XOR, SRA

	if (execute_stage.operation == "ADD")
	{
		execute_stage.alu_result = decode_stage.rs1 + decode_stage.rs2;
	}

	else if (execute_stage.operation == "SUB")
	{
		execute_stage.alu_result = decode_stage.rs1 - decode_stage.rs2;
	}

	else if (execute_stage.operation == "XOR")
	{
		execute_stage.alu_result = decode_stage.rs1 ^ decode_stage.rs2;
	}

	else if (execute_stage.operation == "SRA")
	{
		execute_stage.alu_result = (int32_t) decode_stage.rs1 >> (decode_stage.rs2 & (unsigned) 0x1F);
	}

	// I-type instructions: ADDI, ANDI

	else if (execute_stage.operation == "ADDI")
	{
		execute_stage.alu_result = decode_stage.rs1 + decode_stage.imm;
	}

	else if (execute_stage.operation == "ANDI")
	{
		execute_stage.alu_result = decode_stage.rs1 & decode_stage.imm;
	}
	
	//Memory instructions: LW, SW

	else if ((execute_stage.operation == "LW") || (execute_stage.operation == "SW"))
	{
		execute_stage.alu_result = decode_stage.rs1 + decode_stage.imm;
	}

	//Branch if less than instruction, modify PC only when rs1 < rs2
	else if (execute_stage.operation == "BLT")
	{
		if(decode_stage.rs1 < decode_stage.rs2)
		{
			PC = PC + (int32_t)(decode_stage.imm & ~1) - 4;
		}
	}

	//Jump instruction, modify PC and save return address
	else if (execute_stage.operation == "JALR")
	{
		execute_stage.alu_result =PC;
		PC = (decode_stage.rs1 + (int32_t)(decode_stage.imm) & ~1);
	}

	else if (execute_stage.operation == "END")
	{
		execute_stage.alu_result = ZERO;
	}

	else if (execute_stage.operation == "ERROR")
	{
		cout << "ERROR Operation encountered in execute_instr()" << endl;
	}

}

void CPU::memory_instr()
{
	memory_stage.rd = execute_stage.rd;
	memory_stage.alu_result = execute_stage.alu_result;
	memory_stage.operation = execute_stage.operation;

	if (execute_stage.operation == "LW")
	{
		memory_stage.data_mem = ((((dmemory[execute_stage.alu_result + 3]) << 24)) | ((dmemory[execute_stage.alu_result + 2]) << 16) | ((dmemory[execute_stage.alu_result + 1]) << 8) | (dmemory[execute_stage.alu_result]));
	}

	else if (execute_stage.operation == "SW")
	{
		//Since the memory is byte-addressable, so to maintain the continuity of data
		dmemory[execute_stage.alu_result] = (execute_stage.rs2 & 0xFF);
		dmemory[execute_stage.alu_result + 1] = ((execute_stage.rs2 & 0xFF00)>>8);
		dmemory[execute_stage.alu_result + 2] = ((execute_stage.rs2 & 0xFF0000)>>16);
		dmemory[execute_stage.alu_result + 3] = ((execute_stage.rs2 & 0xFF000000)>>24);
	}

}

void CPU::write_back_instr()
{
	//Incrementing clock cycles since after write back the instruction life is over
	clock_cycles++;

	if ((memory_stage.operation == "SW") || (memory_stage.rd == ZERO))
	{
		return;
	}

	else if (memory_stage.operation == "LW")
	{
		reg_file[memory_stage.rd] = memory_stage.data_mem;
	}

	else if (memory_stage.operation == "JALR")
	{
		reg_file[execute_stage.rd] = execute_stage.alu_result;
	}

	else if (memory_stage.operation == "ERROR")
	{
		cout << "ERROR Operation encountered in write_back_instr()" << endl;
	}

	else	//remainder instructions - ADD, SUB, XOR, SRA, ADDI, ANDI - write to register file
	{
		reg_file[memory_stage.rd] = memory_stage.alu_result;
	}

}

//Helper function to print performance metrics, and the output in the specified format
void CPU::print_results()
{
	int a0 = reg_file[10]; //a0 is x10
	int a1 = reg_file[11]; //a1 is x11

	cout << "(" << a0 << "," << a1 << ")" << endl;

	ipc = (double) instr_count / clock_cycles;

	//cout << "Total Clock Cycles elapsed: " << clock_cycles << endl;
	//cout << "Total Instructions " << instr_count << endl;
	//cout << "R-type Instructions " << r_instr_count << endl ;
	//cout << "Instructions per Cycle (IPC) " << ipc << endl;
}
