#pragma once

#include <string>
#include <sstream>

#include "LittleIntern.h"

namespace LilBit
{
	//Container for LittleBit code
	class Code
	{
		public:
		Code();

		void run(Intern& interpreter);

		void addInstruction(Byte inst, std::string additional);

		void mergeWith(const LilBit::Code& from);

		private:
		std::string raw;
	};
}

