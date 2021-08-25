#pragma once

#include <iostream>
#include <utility>
#include <functional>

#define LILBIT_MAXPARAMS 16 //Number of parameters supported
#define LILBIT_SMALLMAX 

namespace LilBit
{
	typedef unsigned char Byte;
	typedef Byte Small;
	typedef unsigned short Medium;
	typedef size_t Large;
	typedef void (*Func) (void**);
	typedef std::function<void (void**)> Functor;
	typedef size_t ID;

	const size_t SmallMax = (Small)~0;
	const size_t MediumMax = (Medium)~0;
	const size_t LargeMax = (Large)~0;

	//Allows selection of the required word size
	static constexpr size_t DYSize[3] = { sizeof(Small), sizeof(Medium), sizeof(Large) };

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
		I_LFUNC = 10, //Calls any function with any parameters

		I_CNE = 11, //Sets a small address *value* flag for if another small address *value* is negative
		I_CPO = 12 //Sets a small address *value* flag for if another small address *value* is positive
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
			void registerFunc(ID id, const Functor& function, size_t params);
			//Registers the address to an existing variable by id
			void registerVariable(ID id, void* var);

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

			std::pair<Functor, size_t>* functions; //Functions and their argument count
			void** staticMemory; //Predefined variables

			void* parameterSel[LILBIT_MAXPARAMS]; //Selected parameters for function calls

	};
}

