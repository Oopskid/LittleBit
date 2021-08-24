#include "JumpMark.h"
#include "LittleCompiler.h"

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

void LilBit::JumpMark::setJumpID(ID location)
{
	jumpID = location;
}

std::string LilBit::JumpMark::makeCode(size_t realLocation, size_t realDestination)
{
	std::string raw;

	raw.append(LilBit::raw(baseInstruction), sizeof(Small)); //Instruction itself
	switch (baseInstruction)
	{
		default: return std::string(); //undefined
		
		case I_JUMP:
			raw.append(LilBit::raw(realDestination), sizeof(Large)); //Global destination
		return raw;

		case I_HOPBACK:
			realDestination = realLocation - realDestination; //Backwards offset
			raw.append(LilBit::raw(realDestination), sizeof(Small));
		return raw;

		case I_HOPFORWARD:
			realDestination -= realLocation; //Forwards offset
			raw.append(LilBit::raw(realDestination), sizeof(Small));
		return raw;

		case I_BNZ:
		case I_BOZ:
		case I_BNE:
		case I_BPO:
			raw.append(LilBit::raw(param), sizeof(Small)); //Param
			raw.append(LilBit::raw(realDestination), sizeof(Large)); //Global destination
		return raw;
	}

	return raw;
}

void LilBit::JumpMark::optimiseInstruction(bool polarity, size_t span)
{
	Byte fittingType = LilBit::Compiler::getSizeRequirement(span);

	switch (baseInstruction)
	{
		default: return;
		case I_JUMP:
		if(fittingType == 0)
		{
			baseInstruction = polarity ? I_HOPBACK : I_HOPFORWARD;
		}
		return;
	}
}

bool LilBit::JumpMark::couldOptimise()
{
	switch (baseInstruction)
	{
		case I_JUMP: return true;
	}

	return false;
}

size_t LilBit::JumpMark::getInstructionSize()
{
	switch (baseInstruction)
	{
		default: return 0;

		case I_JUMP: return sizeof(Small) + sizeof(Large);
		case I_HOPFORWARD: return 2 * sizeof(Small);
		case I_HOPBACK: return 2 * sizeof(Small);

		case I_BNZ:
		case I_BOZ:
		case I_BNE:
		case I_BPO: return 2 * sizeof(Small) + sizeof(Large);
	}

	return 0;
}