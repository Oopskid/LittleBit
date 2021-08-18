#pragma once

#include "LittleIntern.h"
namespace LilBit
{
	//Lightweight class for optimising jumps
	class JumpMark
	{
		public:

		private:
		Byte baseInstruction;
		ID jumpID;
		size_t param;
	};
}

