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
	functions = new std::pair<Functor, size_t>[funcCount];
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

void Intern::registerFunc(ID id, const Functor& function, size_t params)
{
	functions[id] = std::make_pair(function, params);
}

void Intern::registerVariable(ID id, void* var)
{
	staticMemory[id] = var;
}

void Intern::execute(std::istream& in, Byte instruction)
{
	switch (instruction)
	{
		default: in.setstate(std::ios_base::failbit); return;
		case I_END: in.setstate(std::ios_base::eofbit); return;
		case I_HOPFORWARD: in.seekg(get<Small>(in), std::ios_base::_Seekcur); return;
		case I_HOPBACK: in.seekg(-get<Small>(in), std::ios_base::_Seekcur); return;
		case I_JUMP: in.seekg(get<Large>(in), std::ios_base::_Seekbeg); return;
		case I_BNZ:
		{
			Small miniValLoc = get<Small>(in);
			Large jumpPos = get<Large>(in);

			if (staticMemory[miniValLoc] && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BOZ:
		{
			Small miniValLoc = get<Small>(in);
			Large jumpPos = get<Large>(in);

			if (!staticMemory[miniValLoc] && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BNE:
		{
			Small miniValLoc = get<Small>(in);
			Large jumpPos = get<Large>(in);

			if (intptr_t(staticMemory[miniValLoc]) < 0 && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BPO:
		{
			Small miniValLoc = get<Small>(in);
			Large jumpPos = get<Large>(in);

			if (intptr_t(staticMemory[miniValLoc]) >= 0 && in.good()) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_SFUNC: runFunction<Small>(in); return;
		case I_MFUNC: runFunction<Medium>(in); return;
		case I_LFUNC: runFunction<Large>(in); return;
		case I_CNE:
		{
			Small flagLoc = get<Small>(in);
			Small valLoc = get<Small>(in);

			staticMemory[flagLoc] = reinterpret_cast<void*>(intptr_t(staticMemory[valLoc]) < 0);
		}
		return;
		case I_CPO:
		{
			Small flagLoc = get<Small>(in);
			Small valLoc = get<Small>(in);

			staticMemory[flagLoc] = reinterpret_cast<void*>(intptr_t(staticMemory[valLoc]) >= 0);
		}
		return;
	}
}
