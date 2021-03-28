#include "statement.h"
#include "parser.h"
#include "mainwindow.h"

statement::statement(evalstate* st)
{
    state = st;
}

//-------------------------------------------REM-----------------------------------------------
remStmt::remStmt(QList<QString> tokens, evalstate* st):statement (st)
{
    for (int i=1;i<tokens.size();++i) {
        text += tokens[i];
    }
    type = REM;
}

int remStmt::excute()
{
    return 0;
}

QString remStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("REM\n")+QString("    ")+text+QString("\n");
    return tmpString;
}
//-------------------------------------------REM--------------------------------------------end

//-------------------------------------------LET-----------------------------------------------
letStmt::letStmt(QList<QString> tokens, evalstate* st):statement (st)
{
    if(tokens.length()!=4) {
        QString error("Illegal LET Statement! The Length Of Tokens Is Not 4.");
        throw error;
    }

    if(tokens[2]!="=") {
        QString error("Illegal LET Statement! Missing \"=\".");
        throw error;
    }

    varible = tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除变量名内所有空格;
    parser myParser(state);
    exp = myParser.parseExp(tokens[3]);

    type = LET;
}

int letStmt::excute()
{
    state->setValue(varible, exp->eval(*state));
    return 0;
}

QString letStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("LET =\n")+QString("    ")+varible+QString("\n");
    tmpString.append(exp->toSyntaxTreeString(2));
    return tmpString;
}
//-------------------------------------------LET--------------------------------------------end

//-------------------------------------------PRINT-----------------------------------------------
printStmt::printStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    if(tokens.length()!=2) {
        QString error("Illegal PRINT Statement! The Length Of Tokens Is Not 2.");
        throw error;
    }

    parser myParser(state);
    exp = myParser.parseExp(tokens[1]);

    type = PRINT;
}

int printStmt::getValue()
{
    return exp->eval(*state);
}

int printStmt::excute()
{
    return -1;
}

QString printStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("PRINT\n");
    tmpString.append(exp->toSyntaxTreeString(2));
    return tmpString;
}
//-------------------------------------------PRINT--------------------------------------------end

//-------------------------------------------INPUT-----------------------------------------------
inputStmt::inputStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    if(tokens.length()!=2) {
        QString error("Illegal GOTO Statement! The Length Of Tokens Is Not 2.");
        throw error;
    }

    varible=tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除变量名内所有空格
}

int inputStmt::excute()
{
    return -2;
}

QString inputStmt::getVarName()
{
    return varible;
}

QString inputStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("INPUT\n")+QString("    ")+varible+QString("\n");
    return tmpString;
}
//-------------------------------------------INPUT--------------------------------------------end

//-------------------------------------------GOTO-----------------------------------------------
gotoStmt::gotoStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    if(tokens.length()!=2) {
        QString error("Illegal GOTO Statement! The Length Of Tokens Is Not 2.");
        throw error;
    }
    tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除line number内所有空格
    bool ok;
    nextLineNum = tokens[1].toInt(&ok);
    if(!ok) {
        QString error("Illegal GOTO Statement! The Second Token Should Be A Number.");
        throw error;
    }
    type = GOTO;
}

int gotoStmt::excute()
{
    return nextLineNum;
}

QString gotoStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("INPUT\n")+QString("    ")+QString::number(nextLineNum)+QString("\n");
    return tmpString;
}
//-------------------------------------------GOTO--------------------------------------------end

//-------------------------------------------IF THEN-----------------------------------------------
ifStmt::ifStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    if(tokens.length()!=6) {
        QString error("Illegal IF Statement! The Length Of Tokens Is Not 6.\nThe Form Should Be \"IF exp1 op exp2 THEN n\"");
        throw error;
    }
    bool ok;
    tokens[5].remove(QRegExp("\\s"));   //用正则表达式移除number内所有空格
    thenNum = tokens[5].toInt(&ok);
    if(!isOp(tokens[2])||tokens[4]!="THEN"||!ok) {
        QString error("Illegal IF Statement!\nThe Form Should Be \"IF exp1 op exp2 THEN n\"");
        throw error;
    }

    parser myParser(st);
    exp1 = myParser.parseExp(tokens[1]);
    exp2 = myParser.parseExp(tokens[3]);
    op=tokens[2];
    type = IF;
}

int ifStmt::excute()
{
    if(condition())
        return thenNum;
    else return 0;
}

bool ifStmt::isOp(QString op)
{
    if(op==">"||op=="<"||op=="=")
        return true;
    return false;
}

bool ifStmt::condition()
{
    int value1=exp1->eval(*state);
    int value2=exp2->eval(*state);
    if(op==">") {
        return value1>value2;
    }
    else if (op=="=") {
        return value1==value2;
    }
    else if (op=="<") {
        return value1<value2;
    }
    else return false;
}

QString ifStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("IF THEN\n");
    tmpString.append(exp1->toSyntaxTreeString(2));
    tmpString+=(QString("    ")+op+QString("\n"));
    tmpString.append(exp2->toSyntaxTreeString(2));
    tmpString+=(QString("    ")+QString::number(thenNum)+QString("\n"));
    return tmpString;
}
//-------------------------------------------IF THEN--------------------------------------------end

//-------------------------------------------END-----------------------------------------------
endStmt::endStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    type = END;
}

int endStmt::excute()
{
    return -3;
}

QString endStmt::toSyntaxTree()
{
    return QString("END\n");
}
//-------------------------------------------END--------------------------------------------end
