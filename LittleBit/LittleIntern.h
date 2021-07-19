#pragma once

#include <iostream>
#include <utility>
#include <functional>

#define LILBIT_MAXPARAMS 16 //Number of parameters supported

namespace LilBit
{
	typedef unsigned char Byte;
	typedef void* Func(void**);

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
		I_LFUNC = 10 //Calls any function with any parameters
	};

	//Edge (end) case component of unpacking a parameter selection into static type references
	template<typename T> void unpackElements(void** pack, size_t i, T& subject)
	{
		subject = *reinterpret_cast<T*>(pack[i]);
	}

	//Recursive component of unpacking a parameter selection into static type references
	template<typename T, typename... P> void unpackElements(void** pack, size_t i, T& subject, P&... rest)
	{
		subject = *reinterpret_cast<T*>(pack[i]);
		unpackElements(pack, i + 1, rest...);
	}

	//Unpacks a parameter selection into static type references
	//USED TO DECIPHER LITTLEBIT INTERPRETER CALLS!
	template<typename... T> void unpack(void** pack, T&... into)
	{
		unpackElements(pack, 0, into...);
	}

	class Intern //Interpreter class
	{
		public:
			Intern();
			~Intern();

			void initiate(size_t funcCount, size_t staticCount);

			void run(std::istream& in);

			//Registers a function with an id and number of parameters
			void registerFunc(size_t id, const std::function<Func>& function, size_t params);
			//Registers the address to an existing variable by id
			void registerVariable(size_t id, void* var);

		private:

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

	};
}

