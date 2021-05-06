#include "evalstate.h"
#include <qdebug.h>

evalstate::evalstate()
{

}

void evalstate::setValue(QString var, int value)
{
    if(isDefinedString(var))
        stringSymTable.remove(var);
    symbolTable[var] = value;
}

void evalstate::setValue(QString var, QString sValue)
{
    if(isDefinedInt(var))
        symbolTable.remove(var);
    stringSymTable[var] = sValue;
}

int evalstate::getValue(QString var)
{
    if(!isDefinedInt(var)) {
        QString error = var + " is undefined";
        throw error;
    }
    return symbolTable.value(var);
}

QString evalstate::getString(QString var)
{
    if(!isDefinedString(var)) {
        QString error = var + " is undefined";
        throw error;
    }
    return stringSymTable.value(var);
}

bool evalstate::isDefinedString(QString var)
{
    return stringSymTable.contains(var);
}

bool evalstate::isDefinedInt(QString var)
{
    return symbolTable.contains(var);
}

void evalstate::clear()
{
    symbolTable.clear();
}

