#include "LittleCode.h"

using namespace LilBit;

Code::Code()
{

}

void Code::run(Intern& interpreter)
{
	std::istringstream codeStream(raw);
	interpreter.run(codeStream);
}

void Code::addInstruction(Byte inst, std::string additional)
{
	raw.push_back(inst);
	raw.append(additional);
}

void LilBit::Code::addRaw(std::string byteCode)
{
	raw.append(byteCode);
}

void LilBit::Code::mergeWith(const LilBit::Code& from)
{
	raw.append(from.raw);
}

size_t LilBit::Code::getSize()
{
	return raw.size();
}
