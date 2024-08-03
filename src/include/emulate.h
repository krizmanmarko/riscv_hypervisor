#ifndef EMULATE_H
#define EMULATE_H

#include "types.h"

// htinst transformed instructions:
//	load : 0[12] addr_offset[5] funct3[3] rd[5] opcode[7]
//	store: 0[7] rs2[5] addr_offset[5] funct3[3] 0[5] opcode[7]

struct instr {
	uint8 opcode;
	uint8 rd;
	uint8 funct3;
	uint8 addr_offset;
	uint8 rs2;
	uint64 addr;
};

#endif // EMULATE_H
