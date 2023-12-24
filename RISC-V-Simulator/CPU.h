#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


class instruction {
public:
	bitset<32> instr;//instruction
	instruction(bitset<32> fetch); // constructor

};

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 

	int32_t reg_file[32];

	// Variables for evaluating performance
	int clock_cycles;
	int r_instr_count;
	int instr_count;
	double ipc;

	//collection of variables for each stage, that are run independently in each stage, so that they can be modified later on for pipelining and forwarding
	struct DECODE_STAGE
	{
		int32_t rs1 = 0;
		int32_t rs2 = 0;
		uint32_t rd = 0;
		int32_t imm = 0;
		string operation;
	} decode_stage;

	struct EXECUTE_STAGE
	{
		int32_t alu_result = 0;
		int32_t rs2 = 0;
		uint32_t rd = 0;
		string operation;
	} execute_stage;

	struct MEMORY_STAGE
	{
		uint32_t rd = 0;
		int32_t alu_result = 0;
		int32_t data_mem = 0;
		string operation;
	} memory_stage;

public:
	CPU();
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction* instr);

	//bool decode_instr(instruction *instr);
	//functions for each of the stages
	void execute_instr();
	void memory_instr();
	void write_back_instr();
	void print_results();
	
};

// add other functions and objects here

// Opcodes
#define R_TYPE 0x33		//0110011
#define I_TYPE 0x13		//0010011
#define LW_TYPE 0x3		//0000011
#define SW_TYPE 0x23	//0100011
#define B_TYPE 0x63		//1100011
#define J_TYPE 0x67		//1100111

//HALT the execution when ZERO encountered
#define ZERO 0x0		//0000000
