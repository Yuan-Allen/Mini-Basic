#include "program.h"

program::program()
{

}

program::~program()
{
    clear();
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
    for (QMap<int, statement*>::iterator it = stmts.begin();it!=stmts.end();++it) {
        delete it.value();
    }
    stmts.clear();
    stmtStrings.clear();
}

void program::clearStmts()
{
    stmts.clear();
}
