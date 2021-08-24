#pragma once

#include "LittleIntern.h"
namespace LilBit
{
	class Compiler; //Forward declaration

	//Lightweight class for optimising jumps
	class JumpMark
	{
		public:
		JumpMark();
		JumpMark(ID jumpTo);
		JumpMark(Byte branchBase, size_t branchParam, ID jumpTo);

		Byte getInstruction();
		ID getJumpID();
		size_t getParam();

		void setJumpID(ID location);

		//Returns code representing the instruction given the global location and destination (bytes)
		std::string makeCode(size_t realLocation, size_t realDestination);

		//Optimises this instruction. Requires [if backwards jump] and the size of the distance from the instruction to destination
		void optimiseInstruction(bool polarity, size_t span);

		//Returns if the current base command has a better counterpart
		bool couldOptimise();

		//Returns the amount of bytes that this instruction would currently take
		size_t getInstructionSize();

		private:
		Byte baseInstruction;
		ID jumpID;
		size_t param;
	};
}

