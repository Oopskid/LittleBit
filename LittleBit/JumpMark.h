#pragma once

#include "LittleIntern.h"
namespace LilBit
{
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

		private:
		Byte baseInstruction;
		ID jumpID;
		size_t param;
	};
}

