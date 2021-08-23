#include "Frogger.h"

LilBit::Frogger::Frogger(std::vector<LilBit::Code>* codeChunks, size_t virtualLocationsCount)
{
	runs = codeChunks;
	virtuals.resize(virtualLocationsCount);
}

void LilBit::Frogger::registerJump(size_t virtualLocation, JumpMark marker)
{
	virtuals[virtualLocation] = std::make_tuple(marker, 0, false);
}

void LilBit::Frogger::registerDestination(size_t virtualLocation)
{
	virtuals[virtualLocation] = std::make_tuple(JumpMark(I_END, 0, 0), 0, true);
}

void LilBit::Frogger::resolveInstructions()
{
	//One full pass at a high level
	for (size_t i = 0; i < virtuals.size(); i++)
	{
		std::set<ID> check;
		determineSize(i, check);
	}
}

void LilBit::Frogger::calcMinGuarantees()
{
	size_t progress = 0;
	for (size_t i = 0; i < virtuals.size(); i++)
	{
		std::get<1>(virtuals[i]) = progress;

		progress += (*runs)[i].getSize(); //Next chunk
	}
}

size_t LilBit::Frogger::determineSize(ID jmpVirtLoc, std::set<ID>& visited)
{
	JumpMark& mark = std::get<0>(virtuals[jmpVirtLoc]);

	//Return answer immediately if already known or can't be optimised
	if (std::get<2>(virtuals[jmpVirtLoc]) || !mark.couldOptimise()) { return mark.getInstructionSize(); }

	//Remember this has been visited
	if (!visited.insert(jmpVirtLoc).second)
	{
		//Was already visited but answer not known...
		//Implies a circular dependency. Instruction hence will be the largest available (which it already is)
		std::get<2>(virtuals[jmpVirtLoc]) = true;
		return mark.getInstructionSize();
	}

	//Normalise the range of the instruction
	ID startRange = jmpVirtLoc;
	ID endRange = mark.getJumpID();
	if (endRange < startRange) { startRange = endRange; endRange = jmpVirtLoc; }

	//The relative size the instruction spans (approx bytes between jump and destination)
	size_t approxSpan = std::get<1>(virtuals[endRange]) - std::get<1>(virtuals[startRange]);

	//Figure out the span
	for (ID i = startRange + 1; i < endRange; i++)
	{
		approxSpan += determineSize(i, visited);
	}

	//Figure out optimised instruction based on range


	std::get<2>(virtuals[jmpVirtLoc]) = true;
	return mark.getInstructionSize();
}
