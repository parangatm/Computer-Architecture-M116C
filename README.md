# Computer-Architecture-M116C
This repository contains my implementation of the Coding Assignments from EC M116C Fall 23 course at UCLA taught by Prof. Sehatbaksh

## Simulator for a RISC-V CPU

### Overview
This project involves designing and implementing a RISC-V single-cycle processor in C/C++. The processor supports a range of instructions, including arithmetic, logical, memory, and control flow operations. The goal is to create a functional processor capable of executing a list of provided instructions.

### Supported Instructions
1. Arithmetic and Logical Operations: `ADD`, `SUB`, `ADDI`, `XOR`, `ANDI`, `SRA`
2. Memory Operations: `LW`, `SW`
3. Control Flow Operations: `BLT`, `JALR`

### Files
- **cpusim.cpp**: Entry point of the project. Run the program using `./cpusim <inputfile.txt>` and print the final values of registers a0 and a1 in the terminal in the format "(a0,a1)".
- **23instMem-X files**: Binary files containing instructions in unsigned decimal format, stored in little-endian format. Use these files to develop and test your project.
- **23all.txt, 23sw.txt, 23r-type.txt**: Assembly programs corresponding to the instructions in the "23instMem-X" files. Each line includes the memory address (hex), the instruction (hex), and the actual assembly representation.
- **.s files**: Assembly program files for each trace, providing the code with no other details.

### Testing
Use the provided test files in the "trace" folder, starting with "r-type.txt" and gradually progressing to "sw.txt" and "all.txt" for comprehensive testing. Debug your code using the correct final values provided in the files.

### Deliverables
1. **Well-documented code**:  Code should be compiled with the following command: `g++ *.cpp -o cpusim`, and then executed with `./cpusim <inputfile.txt>`.
2. **Report**: A short report (a PDF file) that shows your design (datapath with control signals), a table that shows the values for each control signal for all instructions.


## Memory Controller

### Overview
The objective of this project is to design a memory hierarchy with a memory controller. The memory controller is responsible for loading data from memory (LW) or storing values at the correct address (SW). The memory hierarchy consists of two levels of cache (L1 and L2) and main memory. Additionally, there is a victim cache associated with L1.

### Memory Hierarchy Levels
| Memory Level | Specifications                                                                                                               |
|--------------|------------------------------------------------------------------------------------------------------------------------------|
| **L1 Cache**     | - Direct-mapped cache<br>- 1 block per line<br>- 16 lines/sets<br>- Each block: 4 bytes (int)                                |
| **L2 Cache**     | - 8-way set-associative cache<br>- 1 block per line<br>- 16 sets<br>- Each block: 4 bytes (int)<br>- Replacement policy: LRU |
| **Victim Cache** | - Fully-associative<br>- 1 block per line<br>- 4 entries<br>- Each block: 4 bytes (int)<br>- Replacement policy: LRU         |
| **Main Memory**  | - 4096 lines<br>- Each line: 1 byte                                                                                          |

Note: Each cache level is:
  - exclusive/Write-No-Allocate/Write-Through
  - byte-addressable
  - initially randomly initialized with all valid bits equal to zero

### Controller Operations
1. **Load Operation**
- Search L1, Victim cache, and then L2
- Update stats for each cache level
- Update data, tag, and LRU positions if needed
- Evicted L1 data goes to Victim cache, evicted Victim cache data goes to L2

2. **Store Operation**
- Search L1 and Victim cache
- Update stats in both cases
- Update LRU for store
- If data is in both cache and memory, update both
- If not in the cache, only update memory

### Testing
The entry point is `memory_driver.cpp`. Run the program using `./memory_driver <inputfile.txt>`.
Print the miss-rate for L1 and L2, and the average access time (AAT) in the terminal.
(Miss-rate reported only for LW)

AAT assumptions: L1 hit time: 1 cycle, Victim cache hit time: 1 cycle, L2 hit time: 8 cycles, Main memory hit time: 100 cycles

**Output format**: "(L1,L2,AAT)"

### Deliverables
**All code files**: Compiled with the command `g++ *.cpp -o memory_driver` and run with `./memory_driver <inputfile.txt>`.
