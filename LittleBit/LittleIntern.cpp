#include "LittleIntern.h"

using namespace LilBit;

Intern::Intern()
{
	functions = nullptr;
	staticMemory = nullptr;
}

Intern::~Intern()
{
	delete functions;
	delete staticMemory;
}

void Intern::initiate(size_t funcCount, size_t staticCount)
{
	functions = new std::pair<std::function<Func>, size_t>[funcCount];
	staticMemory = new void* [staticCount];
}

void Intern::run(std::istream& in)
{
	char ins;
	while (in.get(ins).good())
	{
		execute(in, ins);
	}
}

void Intern::registerFunc(size_t id, const std::function<Func>& function, size_t params)
{
	functions[id] = std::make_pair(function, params);
}

void Intern::registerVariable(size_t id, void* var)
{
	staticMemory[id] = var;
}

void Intern::execute(std::istream& in, Byte instruction)
{
	switch (instruction)
	{
		default: in.setstate(std::ios_base::failbit); return;
		case I_END: in.setstate(std::ios_base::eofbit); return;
		case I_HOPFORWARD: in.seekg(get<Byte>(in), std::ios_base::_Seekcur); return;
		case I_HOPBACK: in.seekg(-get<Byte>(in), std::ios_base::_Seekcur); return;
		case I_JUMP: in.seekg(get<size_t>(in), std::ios_base::_Seekbeg); return;
		case I_BNZ:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (staticMemory[miniValLoc] && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BOZ:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (!staticMemory[miniValLoc] && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BNE:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (intptr_t(staticMemory[miniValLoc]) < 0 && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BPO:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (intptr_t(staticMemory[miniValLoc]) >= 0 && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_SFUNC: runFunction<Byte>(in); return;
		case I_MFUNC: runFunction<unsigned short>(in); return;
		case I_LFUNC: runFunction<size_t>(in); return;
	}
}
