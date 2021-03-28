#include "evalstate.h"
#include <qdebug.h>

evalstate::evalstate()
{

}

void evalstate::setValue(QString var, int value)
{
    symbolTable[var] = value;
}

int evalstate::getValue(QString var)
{
    if(!isDefined(var)) {
        QString error = var + " is undefined";
        throw error;
    }
    return symbolTable.value(var);
}

bool evalstate::isDefined(QString var)
{
    return symbolTable.contains(var);
}

void evalstate::clear()
{
    symbolTable.clear();
}

