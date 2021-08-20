#include "Scope.h"

LilBit::ID LilBit::Scope::getBreak()
{
    return breakJumpID;
}

LilBit::ID LilBit::Scope::getContinue()
{
    return continueJumpID;
}

void LilBit::Scope::setBreak(ID breakJ)
{
    breakJumpID = breakJ;
}

void LilBit::Scope::setContinue(ID continueJ)
{
    continueJumpID = continueJ;
}
