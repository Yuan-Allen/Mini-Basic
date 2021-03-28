#include "program.h"

program::program()
{

}

void program::addNewStmtString(int lineNum, QString line)
{
    stmtStrings[lineNum]=line;
}

void program::addNewStatement(int lineNum, statement* newStatement)
{
    stmts[lineNum]=newStatement;
}

void program::removeStatement(int lineNum)
{
    if(stmts.contains(lineNum))
        stmts.remove(lineNum);
    if(stmtStrings.contains(lineNum))
        stmtStrings.remove(lineNum);
}

void program::clear()
{
    stmts.clear();
    stmtStrings.clear();
}
