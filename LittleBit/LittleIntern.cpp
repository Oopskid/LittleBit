#include "LittleIntern.h"

LittleIntern::LittleIntern()
{
	functions = nullptr;
	staticMemory = nullptr;
}

LittleIntern::~LittleIntern()
{
	delete functions;
	delete staticMemory;
}

void LittleIntern::initiate(size_t funcCount, size_t staticCount)
{
	functions = new std::pair<void*, size_t>[funcCount];
	staticMemory = new void* [staticCount];
}

void LittleIntern::run(std::istream& in)
{
	char ins;
	while (in.get(ins).good())
	{

	}
}

void LittleIntern::registerFunc(size_t id, LittleIntern::Func function, size_t params)
{
	functions[id] = std::make_pair(function, params);
}

void LittleIntern::registerVariable(size_t id, void* var)
{
	staticMemory[id] = var;
}

void LittleIntern::execute(std::istream& in, Byte instruction)
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

			if (staticMemory[miniValLoc]) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BOZ:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (!staticMemory[miniValLoc]) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BNE:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (intptr_t(staticMemory[miniValLoc]) < 0) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
		case I_BPO:
		{
			Byte miniValLoc = get<Byte>(in);
			size_t jumpPos = get<size_t>(in);

			if (intptr_t(staticMemory[miniValLoc]) >= 0) { in.seekg(jumpPos, std::ios_base::_Seekbeg); }
		}
		return;
	}
}
