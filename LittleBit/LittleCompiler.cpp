#include "LittleCompiler.h"

using namespace LilBit;

LilBit::Compiler::Compiler()
{
	typeProg = 0;

	virtualLocation = 0;

	exitLayer = 1;
	lastFreeStatic = 1;
	statics = nullptr;
	smCount = 0;

	runs.push_back(LilBit::Code());
}

LilBit::Compiler::~Compiler()
{
	delete statics;
	statics = nullptr;

}

bool LilBit::Compiler::hasLog()
{
	return !log.empty();
}

void LilBit::Compiler::nextLog()
{
	log.pop();
}

std::string& LilBit::Compiler::getLog()
{
	return log.top();
}

void LilBit::Compiler::postLog(std::string message)
{
	log.push(message);
}

void LilBit::Compiler::prepareSM(size_t smSize)
{
	//New memory
	delete statics;
	statics = new std::pair<bool, ID>[smSize];
	smCount = smSize;
	staticVariables.clear();

	//Make all memory available
	for (size_t i = 1; i < smSize; i++) { statics[i].first = false; }

	//Reserve null
	lastFreeStatic = 1;
	statics[0].first = true;
}

bool LilBit::Compiler::finalCheck()
{
	bool checkResult = true;

	if (virtualLocation + 1 != runs.size())
	{
		log.push("There is an inconsistency between virtual locations and runs, compilation is impossible!");
		checkResult = false;
	}

	//Find all name-able jump labels that inevitably weren't declared
	for (auto label : jumpLabels)
	{
		if (!jumpIndexer[label.second].second)
		{
			log.push(std::string("The promised label \"").append(label.first).append("\" does not exist!"));
			checkResult = false;
		}
	}

	//Count how many jumps are invalid
	{
		size_t countDeadLabels = 0;
		for (auto i : jumps)
		{
			if (!jumpIndexer[i.second.getJumpID()].second)
			{
				countDeadLabels++;
			}
		}
		if (countDeadLabels) { log.push(std::string("A total of ").append(std::to_string(countDeadLabels)).append(" jump(s) are invalid due to broken promises!")); checkResult = false; }
	}

	return checkResult;
}

Code LilBit::Compiler::compileAll()
{
	doEnd();

	Frogger jumpResolv(&runs, virtualLocation + 1);

	//Hand over jumps (with traded ID)
	for (size_t i = 0; i < jumps.size(); i++)
	{
		JumpMark newMark = jumps[i].second;
		newMark.setJumpID(jumpIndexer[newMark.getJumpID()].first); //Trade ID for virtual location

		jumpResolv.registerJump(jumps[i].first, newMark);
	}

	//Hand over destinations
	for (size_t i = 0; i < jumpIndexer.size(); i++)
	{
		if (jumpIndexer[i].second)
		{
			jumpResolv.registerDestination(jumpIndexer[i].first);
		}
	}

	jumpResolv.resolveInstructions();

	return jumpResolv.build();
}

void LilBit::Compiler::doEnd()
{
	runs.back().addInstruction(I_END, "");
}

bool LilBit::Compiler::callFunction(std::string funcName, std::vector<std::string> argumNames)
{
	//Get the available functions by name
	auto funcFIt = functionDec.find(funcName);
	if (funcFIt == functionDec.end()) { log.push(std::string("Could not find a function with name \"").append(funcName).append("\"!")); return false; }

	//Check arguments and break into structures
	std::vector<ID> paramSMs(argumNames.size());
	std::vector<ID> paramTypes(argumNames.size());
	for (size_t i = 0; i < argumNames.size(); i++)
	{
		auto smIt = staticVariables.find(argumNames[i]);
		if (smIt == staticVariables.end()) { log.push(std::string("The variable \"").append(argumNames[i]).append("\" does not exist!")); return false; }

		paramSMs[i] = smIt->second;
		paramTypes[i] = statics[smIt->second].second;
	}

	//Use arguments/parameters match to get the function
	auto funcSIt = funcFIt->second.find(paramTypes);
	if (funcSIt == funcFIt->second.end()) { log.push(std::string("Attempted call to \"").append(funcName).append("\" impossible as arguments don't match the parameters!")); return false; }

	//Successful! Call the function!
	callFunc(funcSIt->second, paramSMs);

	return true;
}

bool LilBit::Compiler::ifNull(ID smVar, ID endJumpID)
{
	return jumpTo(JumpMark(Instructions::I_BNZ, smVar, endJumpID));
}

bool LilBit::Compiler::ifValue(ID smVar, ID endJumpID)
{
	return jumpTo(JumpMark(Instructions::I_BOZ, smVar, endJumpID));
}

bool LilBit::Compiler::ifPositive(ID smVar, ID endJumpID)
{
	return jumpTo(JumpMark(Instructions::I_BNE, smVar, endJumpID));
}

bool LilBit::Compiler::ifNegative(ID smVar, ID endJumpID)
{
	return jumpTo(JumpMark(Instructions::I_BPO, smVar, endJumpID));
}

void LilBit::Compiler::jumpTo(std::string labelName)
{
	jumpTo(promiseJumpDest(labelName));
}

bool LilBit::Compiler::jumpTo(ID jumpID)
{
	return jumpTo(JumpMark(jumpID));
}

bool LilBit::Compiler::jumpTo(LilBit::JumpMark mark)
{
	if (mark.getJumpID() >= jumpIndexer.size()) { log.push("This jump location does not exist!"); return false; }

	newRun();

	jumps.push_back(std::make_pair(virtualLocation, mark));
	return true;
}

bool LilBit::Compiler::doContinue(size_t scopeDif)
{
	if (scopeDif <= 0) { log.push("Cannot continue when the scope difference is <= 0"); return false; }
	if (scopeDif > scopes.size()) { log.push("Cannot continue when the destination scope doesn't exist"); return false; }

	return jumpTo(scopes[scopes.size() - scopeDif].continueJumpID);
}

bool LilBit::Compiler::doBreak(size_t scopeDif)
{
	if (scopeDif <= 0) { log.push("Cannot break when the scope difference is <= 0"); return false; }
	if (scopeDif > scopes.size()) { log.push("Cannot break when the destination scope doesn't exist"); return false; }

	return jumpTo(scopes[scopes.size() - scopeDif].breakJumpID);
}

ID LilBit::Compiler::declareVariable(std::string type, std::string name)
{
	//Check type
	auto typeIt = typeDec.find(type);
	if (typeIt == typeDec.end()) { log.push(std::string("The type \"").append(type).append("\" does not exist")); return 0; }

	//Check variable does not already exist
	if (staticVariables.find(name) != staticVariables.end()) { log.push(std::string("The SM variable \"").append(name).append("\" is already declared!")); return 0; }

	ID varLoc = newVar(typeIt->second);
	if (varLoc >= smCount) { log.push(std::string("SM overrun with new variable \"").append(name).append("\"!")); return 0; }
	
	//Success! Make SM variable official
	staticVariables.insert(std::make_pair(name, varLoc));
	scopes.back().staticVariables.push_back(varLoc);
	scopes.back().staticVarDecs.push_back(name);

	return varLoc;
}

void LilBit::Compiler::newScope(ID tag)
{
	scopes.push_back(Scope());
	scopes.back().scopeTag = tag;

	scopes.back().breakJumpID = promiseJumpDest();
	scopes.back().continueJumpID = promiseJumpDest();
}

bool LilBit::Compiler::endScope()
{
	Scope& scope = scopes.back();

	//Implement break
	newRun();
	jumpIndexer[scope.breakJumpID] = std::make_pair(virtualLocation, true);

	//Remove all scope dependant definitions
	//Type defs
	for (auto itTy : scope.typeDecs)
	{
		typeDec.erase(itTy);
	}
	//SM vars
	for (auto itSM : scope.staticVariables)
	{
		statics[itSM].first = false;
		if (itSM < lastFreeStatic) { lastFreeStatic = itSM; }
	}
	//SM defs
	for (auto itSM : scope.staticVarDecs)
	{
		staticVariables.erase(itSM);
	}

	//Finalise
	scopes.pop_back();
	return scopes.size() < exitLayer;
}

void LilBit::Compiler::scopeHop(ID tag)
{
	//Force a jump within the current scope to link to the end of the next (same level) scope!
	ID breakOverride = promiseJumpDest();
	jumpTo(breakOverride);

	endScope();
	newScope(tag);
	scopes.back().breakJumpID = breakOverride;
	//Currently leaves an unused jump ID in the abyss but oh well!
}

bool LilBit::Compiler::declareLabel(std::string name)
{
	newRun(); //No matter the outcome, a new run is mandatory

	auto lblIt = jumpLabels.find(name);
	if (lblIt == jumpLabels.end())
	{
		lblIt = jumpLabels.insert(std::make_pair(name, jumpIndexer.size())).first;
		
		jumpIndexer.push_back(std::make_pair(virtualLocation, true));

		return false;
	}

	jumpIndexer[lblIt->second] = std::make_pair(virtualLocation, true);
	return true;
}

ID LilBit::Compiler::declareType(std::string name)
{
	//Try to insert
	auto typeAt = typeDec.insert(std::make_pair(name, typeProg));
	//Check if successful
	if (typeAt.second)
	{
		typeProg++;
		return typeAt.first->second;
	}

	log.push(std::string("The type \"").append(name).append("\" already exists!"));
	return typeAt.first->second;
}

bool LilBit::Compiler::declareTypeAlias(ID knownId, std::string name)
{
	//Try to insert
	auto typeAt = typeDec.insert(std::make_pair(name, knownId));

	if (!typeAt.second)
	{
		log.push(std::string("The type \"").append(name).append("\" already exists!"));
	}

	return typeAt.second;
}

bool LilBit::Compiler::assertFunction(ID id, std::string name, const std::vector<std::string>& params)
{
	//Decipher parameters
	std::vector<ID> paramLink(params.size());
	//Get IDs from type names
	for (size_t i = 0; i < params.size(); i++)
	{
		auto typeIt = typeDec.find(params[i]);
		//Type validation
		if(typeIt == typeDec.end())
		{
			log.push(std::string("The parameter type \"").append(params[i]).append("\" does not exist! Could not declare function \"").append(name).append("\"!"));
			return false;
		}

		paramLink[i] = typeIt->second;
	}

	//Sort by name
	auto firstIt = functionDec.insert(std::make_pair(name, decltype(functionDec)::value_type::second_type()));
	//Sort by parameters
	auto secondIt = firstIt.first->second.insert(std::make_pair(paramLink, id));

	//Check if this was a duplicate
	if (!(firstIt.second || secondIt.second))
	{
		log.push(std::string("The function \"").append(name).append("\" with ").append(std::to_string(params.size())).append(" parameters already exists!"));
		return false;
	}

	//Success!
	return true;
}

ID LilBit::Compiler::newVar(ID type)
{
	//Find the next available SM location
	for (; lastFreeStatic < smCount; lastFreeStatic++) 
	{ 
		if (!statics[lastFreeStatic].first)
		{
			statics[lastFreeStatic].first = true;
			statics[lastFreeStatic].second = type;
			break;
		}
	}

	return lastFreeStatic;
}

Byte LilBit::Compiler::getSizeRequirement(ID value)
{
	if (value <= SmallMax) { return 0; }
	else if (value <= MediumMax) { return 1; }
	
	return 2;
}

void LilBit::Compiler::callFunc(ID function, const std::vector<ID>& params)
{
	//Find out what instruction is required to fit the arguments
	Byte required = getSizeRequirement(function);
	for (size_t i = 0; i < params.size(); i++) 
	{
		required = std::max(required, getSizeRequirement(params[i]));
	}

	//Used to fetch the fitting instruction
	const Byte callIns[3] = { I_SFUNC, I_MFUNC, I_LFUNC };
	
	//Represents the instruction params
	std::string rawParams;
	rawParams.append(raw(function), DYSize[required]); //Push value (using the minimum size)
	for (size_t i = 0; i < params.size(); i++) //Same for all SM params
	{
		rawParams.append(raw(params[i]), DYSize[required]);
	}

	//Finally, create the instruction
	runs.back().addInstruction(callIns[required], rawParams);
}

void LilBit::Compiler::newRun()
{
	runs.push_back(LilBit::Code());
	virtualLocation++;
}

ID LilBit::Compiler::promiseJumpDest(std::string destName)
{
	auto lblIt = jumpLabels.find(destName);
	if (lblIt == jumpLabels.end())
	{
		lblIt = jumpLabels.insert(std::make_pair(destName, jumpIndexer.size())).first;

		jumpIndexer.push_back(std::make_pair(0, false));
	}

	return lblIt->second;
}

ID LilBit::Compiler::promiseJumpDest()
{
	ID id = jumpIndexer.size();
	jumpIndexer.push_back(std::make_pair(virtualLocation, false));
	return id;
}