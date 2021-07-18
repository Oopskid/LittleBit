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
