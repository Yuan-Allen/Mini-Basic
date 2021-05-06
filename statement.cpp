#include "statement.h"
#include "parser.h"
#include "mainwindow.h"

statement::statement(evalstate* st)
{
    state = st;
}

bool statement::isLegalVarName(QString name)
{
    if(name[0].isDigit())
        return false;
    QRegExp reg("^[a-zA-Z_][a-zA-Z0-9_]*$");
    int index;
    if((index=reg.indexIn(name))==0)
        return true;
    else return false;
}

stmtType statement::getType()
{
    return type;
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

bool remStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]=="REM")
        return true;
    return false;
}
//-------------------------------------------REM--------------------------------------------end

//-------------------------------------------LET-----------------------------------------------
letStmt::letStmt(QList<QString> tokens, evalstate* st):statement (st)
{
//    if(tokens.length()!=4) {
//        QString error("Illegal LET Statement! The Length Of Tokens Is Not 4.");
//        throw error;
//    }

//    if(tokens[2]!="=") {
//        QString error("Illegal LET Statement! Missing \"=\".");
//        throw error;
//    }

    varible = tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除变量名内所有空格;
    parser myParser(state);
    if((tokens[3].front()=='\"'&&tokens[3].back()=='\"')||(tokens[3].front()=='\''&&tokens[3].back()=='\'')) {  //如果是被单或双引号括起来的
        sValue = tokens[3].mid(1, tokens[3].length()-2);
        exp = nullptr;
    }
    else
        exp = myParser.parseExp(tokens[3]);

    type = LET;
}

letStmt::~letStmt()
{
    if(exp)
        delete exp;
}

int letStmt::excute()
{
    if(exp)
        state->setValue(varible, exp->eval(*state));
    else state->setValue(varible, sValue);
    return 0;
}

QString letStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("LET =\n")+QString("    ")+varible+QString("\n");
    if(exp)
        tmpString.append(exp->toSyntaxTreeString(2));
    else tmpString+="    "+sValue;
    return tmpString;
}

bool letStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="LET")
        return false;
    if(tokens.length()!=4) {
        return false;
    }

    if(tokens[2]!="=") {
        return false;
    }
    if(!isLegalVarName(tokens[1].remove(QRegExp("\\s"))))   //判断是否为合法的变量名
        return false;
    if((tokens[3].front()=='\"'&&tokens[3].back()=='\"')||(tokens[3].front()=='\''&&tokens[3].back()=='\'')) {  //如果是被单或双引号括起来的
        tokens[3]=tokens[3].mid(1, tokens[3].length()-2);    //去掉头尾引号
        if(tokens[3].contains('\"')||tokens[3].contains('\''))    //如果里面包含双或单引号，不合法
            return false;
    }

    return true;
}
//-------------------------------------------LET--------------------------------------------end

//-------------------------------------------PRINT-----------------------------------------------
printStmt::printStmt(QList<QString> tokens, evalstate *st):statement (st)
{
//    if(tokens.length()!=2) {
//        QString error("Illegal PRINT Statement! The Length Of Tokens Is Not 2.");
//        throw error;
//    }

    parser myParser(state);
    exp = myParser.parseExp(tokens[1]);

    type = PRINT;
}

printStmt::~printStmt()
{
    if(exp)
        delete exp;
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

bool printStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="PRINT")
        return false;
    if(tokens.length()!=2) {
        return false;
    }
    return true;
}
//-------------------------------------------PRINT--------------------------------------------end

//-------------------------------------------INPUT-----------------------------------------------
inputStmt::inputStmt(QList<QString> tokens, evalstate *st):statement (st)
{
//    if(tokens.length()!=2) {
//        QString error("Illegal GOTO Statement! The Length Of Tokens Is Not 2.");
//        throw error;
//    }

    varible=tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除变量名内所有空格
    type = INPUT;
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

bool inputStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="INPUT")
        return false;
    if(tokens.length()!=2) {
        return false;
    }
    if(!isLegalVarName(tokens[1].remove(QRegExp("\\s"))))   //判断是否为合法的变量名
        return false;
    return true;
}
//-------------------------------------------INPUT--------------------------------------------end

//-------------------------------------------GOTO-----------------------------------------------
gotoStmt::gotoStmt(QList<QString> tokens, evalstate *st):statement (st)
{
//    if(tokens.length()!=2) {
//        QString error("Illegal GOTO Statement! The Length Of Tokens Is Not 2.");
//        throw error;
//    }
    tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除line number内所有空格
    bool ok;
    nextLineNum = tokens[1].toInt(&ok);
//    if(!ok) {
//        QString error("Illegal GOTO Statement! The Second Token Should Be A Number.");
//        throw error;
//    }
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

bool gotoStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="GOTO")
        return false;
    if(tokens.length()!=2) {
        return false;
    }
    tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除line number内所有空格
    bool ok;
    tokens[1].toInt(&ok);
    if(!ok) {
        return false;
    }
    return true;
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

ifStmt::~ifStmt()
{
    if(exp1)
        delete exp1;
    if(exp2)
        delete exp2;
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

bool ifStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="IF")
        return false;
    if(tokens.length()!=6) {
        return false;
    }
    bool ok;
    tokens[5].remove(QRegExp("\\s"));   //用正则表达式移除number内所有空格
    tokens[5].toInt(&ok);
    if(!isOp(tokens[2])||tokens[4]!="THEN"||!ok) {
        return false;
    }
    return true;
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

bool endStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="END")
        return false;
    return true;
}
//-------------------------------------------END--------------------------------------------end

//-------------------------------------------INPUTS--------------------------------------------
inputsStmt::inputsStmt(QList<QString> tokens, evalstate *st):statement (st)
{
//    if(tokens.length()!=2) {
//        QString error("Illegal GOTO Statement! The Length Of Tokens Is Not 2.");
//        throw error;
//    }

    varible=tokens[1].remove(QRegExp("\\s"));   //用正则表达式移除变量名内所有空格
    type = INPUTS;
}

int inputsStmt::excute()
{
    return -2;
}

QString inputsStmt::getVarName()
{
    return varible;
}

QString inputsStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("INPUTS\n")+QString("    ")+varible+QString("\n");
    return tmpString;
}

bool inputsStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="INPUTS")
        return false;
    if(tokens.length()!=2) {
        return false;
    }
    if(!isLegalVarName(tokens[1].remove(QRegExp("\\s"))))   //判断是否为合法的变量名
        return false;
    return true;
}
//-------------------------------------------INPUTS-----------------------------------------end

//-------------------------------------------PRINTF--------------------------------------------
printfStmt::printfStmt(QList<QString> tokens, evalstate *st):statement (st)
{
    tokenizer myTokenizer;
    QString tmpString;
    for (int i=1;i<tokens.size();++i) {
        tmpString+=tokens[i];
    }
    bool flag=false;
    argv = myTokenizer.toStringTokens(tmpString, flag);

    type = PRINTF;
}

QString printfStmt::getText()
{
    QString text = argv[0], tmpS;
    for (int i=0, j=1, k=0;i<argv[0].size();++i, ++k) {
        if(argv[0].mid(i, 2)=="{}") {
            if((argv[j].front()=='\"'&&argv[j].back()=='\"')||(argv[j].front()=='\''&&argv[j].back()=='\'')) {
                tmpS = argv[j].mid(1, argv[j].size()-2);
            }
            else {
                bool ok;
                int num = argv[j].toInt(&ok);
                if(ok)
                    tmpS = QString::number(num);
                else {
                    if(state->isDefinedInt(argv[j]))
                        tmpS = QString::number(state->getValue(argv[j]));
                    else tmpS = state->getString(argv[j]);
                }
            }
            text.replace(k, 2, tmpS);
            k+=tmpS.size()-1;    //for循环里还要++k，所以这里少加一个
            ++j;
            ++i;    //多加一个i，跳过右大括号
        }
    }
    text = text.mid(1, text.size()-2);
    return text;
}

int printfStmt::excute()
{
    return -1;
}

QString printfStmt::toSyntaxTree()
{
    QString tmpString;
    tmpString=QString("PRINTF\n");
    for (int i=0;i<argv.size();++i) {
        tmpString.append("    "+argv[i]+"\n");
    }

    return tmpString;
}

bool printfStmt::isLegal(QList<QString> tokens)
{
    if(tokens[0]!="PRINTF")
        return false;
    QString tmpString;
    for (int i=1;i<tokens.size();++i) {
        tmpString+=tokens[i];
    }
    bool flag=false;
    tokenizer myTokenizer;
    QList<QString> lists;
    lists = myTokenizer.toStringTokens(tmpString, flag);
    if(!flag)
        return false;
    if(lists[0].count("{}")>lists.size()) //"{}"的数量大于后续参数的数量？
        return false;
    return true;
}
//-------------------------------------------PRINTF-----------------------------------------end
