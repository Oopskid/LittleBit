#include "LittleCompiler.h"

using namespace LilBit;

LilBit::Compiler::Compiler()
{
	typeProg = 0;

	virtualLocation = 0;
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

void LilBit::Compiler::newScope()
{
	scopes.push_back(std::make_pair(promiseJumpDest(), promiseJumpDest()));
}

bool LilBit::Compiler::declareLabel(std::string name)
{
	auto lblIt = jumpLabels.find(name);
	if (lblIt == jumpLabels.end())
	{
		lblIt = jumpLabels.insert(std::make_pair(name, jumpIndexer.size())).first;
		
		jumpIndexer.push_back(virtualLocation);

		return false;
	}

	jumpIndexer[lblIt->second] = virtualLocation;
	return true;
}

ID LilBit::Compiler::declareType(std::string name)
{
	//Try to insert
	auto typeAt = typeDec.insert(std::make_pair(name, std::make_pair(typeProg, scopes.size())));
	//Check if successful
	if (typeAt.second)
	{
		typeProg++;
		return typeAt.first->second.first;
	}

	log.push(std::string("The type \"").append(name).append("\" already exists!"));
	return typeAt.first->second.first;
}

bool LilBit::Compiler::declareTypeAlias(ID knownId, std::string name)
{
	//Try to insert
	auto typeAt = typeDec.insert(std::make_pair(name, std::make_pair(knownId, scopes.size())));

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

		paramLink.push_back(typeIt->second.first);
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

		jumpIndexer.push_back(0);
	}

	return lblIt->second;
}

ID LilBit::Compiler::promiseJumpDest()
{
	ID id = jumpIndexer.size();
	jumpIndexer.push_back(virtualLocation); //Assume jump to here for the now!
	return id;
}
