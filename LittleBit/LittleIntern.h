#pragma once

#include <iostream>
#include <utility>
#include <functional>

#define LILBIT_MAXPARAMS 16 //Number of parameters supported
class LittleIntern //Interpreter class
{
	public:
	typedef void* Func (void**);

	LittleIntern();
	~LittleIntern();

	void initiate(size_t funcCount, size_t staticCount);

	void run(std::istream& in);

	//Registers a function with an id and number of parameters
	void registerFunc(size_t id, const std::function<LittleIntern::Func>& function, size_t params);
	//Registers the address to an existing variable by id
	void registerVariable(size_t id, void* var);

	private:
	typedef unsigned char Byte;

	template<typename T> void read(std::istream& in, T& out)
	{
		in.read(reinterpret_cast<char*>(&out), sizeof(T));
	}

	template<typename T> T get(std::istream& in)
	{
		T out;
		in.read(reinterpret_cast<char*>(&out), sizeof(T));
		
		return out;
	}

	template<typename T> void runFunction(std::istream& in)
	{
		T funcID = get<T>(in);
		for (size_t i = 0; i < functions[funcID].second; i++)
		{
			parameterSel[i] = staticMemory[get<T>(in)];
		}

		if (!in.good()) { return; } //Safety

		functions[funcID].first(parameterSel);
	}

	void execute(std::istream& in, Byte instruction);

	std::pair<std::function<Func>, size_t>* functions; //Functions and their argument count
	void** staticMemory; //Predefined variables

	void* parameterSel[LILBIT_MAXPARAMS]; //Selected parameters for function calls

	enum Instructions : Byte
	{
		I_END = 0,

		I_HOPFORWARD = 1, //Small forward jump
		I_HOPBACK = 2, //Small backwards jump

		I_JUMP = 3, //Jump to position

		I_BNZ = 4, //Jump to position when small address *value* not zero
		I_BOZ = 5, //Jump to position when small address *value* is zero
		I_BNE = 6, //Jump to position when small address *value* is negative
		I_BPO = 7, //Jump to position when small address *value* is positive

		I_SFUNC = 8, //Calls a small id function with small id parameters
		I_MFUNC = 9, //Calls a medium id function with medium id parameters
		I_BFUNC = 10 //Calls any function with any parameters
	};

};

