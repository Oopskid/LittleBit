#include "JumpMark.h"

LilBit::JumpMark::JumpMark()
{
	baseInstruction = Instructions::I_END;
	jumpID = 0;
	param = 0;
}

LilBit::JumpMark::JumpMark(ID jumpTo)
{
	baseInstruction = Instructions::I_JUMP;
	jumpID = jumpTo;
	param = 0;
}

LilBit::JumpMark::JumpMark(Byte branchBase, size_t branchParam, ID jumpTo)
{
	baseInstruction = branchBase;
	jumpID = jumpTo;
	param = branchParam;
}

LilBit::Byte LilBit::JumpMark::getInstruction()
{
	return baseInstruction;
}

LilBit::ID LilBit::JumpMark::getJumpID()
{
	return jumpID;
}

size_t LilBit::JumpMark::getParam()
{
	return param;
}
