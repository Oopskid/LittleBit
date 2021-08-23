#pragma once

#include <utility>
#include <vector>
#include <set>

#include "JumpMark.h"
#include "LittleCode.h"
namespace LilBit
{
	//Works out what jump commands to use
	class Frogger
	{
		public:
		Frogger(std::vector<LilBit::Code>* codeChunks, size_t virtualLocationsCount);

		void registerJump(size_t virtualLocation, JumpMark marker);
		void registerDestination(size_t virtualLocation);

		//Works out suitable optimised instructions
		void resolveInstructions();

		private:
		//Calculates the amount of guaranteed bytes to each virtual location
		void calcMinGuarantees();

		//Approximately determines the efficient size of a jump/branch instruction relative to other instructions
		//Permanently remembers the 'solution'. Recursive algorithm. Returns the determined or already determined size
		size_t determineSize(ID jmpVirtLoc, std::set<ID>& visited);

		//Virtual locations paired with guaranteed minimum bytes to that point, jump details and if needs to be resolved
		std::vector<std::tuple<JumpMark, size_t, bool>> virtuals;
		std::vector<LilBit::Code>* runs;
	};
}

