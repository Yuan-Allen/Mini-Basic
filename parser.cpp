#include "parser.h"
#include "mainwindow.h"

int precedence(QString op)
{
    if(op=="**") return 3;
    if(op=="*"||op=="/") return 2;
    if(op=="+"||op=="-") return 1;
    if(op=="(") return 0;
    return -1;
}

parser::parser(evalstate *state)
{
    this->state = state;
}

statement* parser::parse(QString line)
{
    myTokenizer.toTokens(line);
    statement * newStatement = nullptr;
    if (myTokenizer.getToken(0)=="REM") {
        newStatement = new remStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="LET") {
        newStatement = new letStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="PRINT") {
        newStatement = new printStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="INPUT") {
        newStatement = new inputStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="GOTO") {
        newStatement = new gotoStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="IF") {
        newStatement = new ifStmt(myTokenizer.getTokens(), state);
    }
    else if (myTokenizer.getToken(0)=="END") {
        newStatement = new endStmt(myTokenizer.getTokens(), state);
    }
    if(!newStatement) {
        QString error("Illegal Statement!");
        throw error;
    }
    return newStatement;
}

Expression* parser::parseExp(QString expString)
{   
    QList<QString> expTokens = myTokenizer.toExpTokens(expString);
    expTokens = preprocessForNeg(expTokens);
    QStack<QString> operators;
    QStack<Expression*> operands;
    Expression *newExpression = nullptr, *lexp, *rexp;

    int i=0;
    while (i<expTokens.length()) {
        if(expTokens[i]==")") {
            while (true) {
                if (operators.isEmpty()) {      //没找到左括号，抛出错误
                    QString error("Illegal Expression! Missing \"(\".");
                    throw error;
                }
                if (operators.top()=="(") {
                    operators.pop();
                    break;
                }
                if (operands.length()<2) {       //是operator，但栈里操作数不够2个了，抛出错误
                    QString error("Illegal Expression! Missing Operands.");
                    throw error;
                }
                QString op = operators.pop();
                rexp = operands.pop();
                lexp = operands.pop();
                newExpression = new CompoundExp(op, lexp, rexp);
                operands.push(newExpression);
            }
            ++i;
            continue;
        }

        if(precedence(expTokens[i])==-1) {
            bool ok;
            int value = expTokens[i].toInt(&ok);
            if(ok) {
                newExpression = new ConstantExp(value);
            }
            else {
                newExpression = new IdentifierExp(expTokens[i]);
            }
            operands.push(newExpression);
            ++i;
            continue;
        }

        while (true) {
            if(precedence(expTokens[i])==3) {
                operators.push(expTokens[i]);
                ++i;
                break;
            }
            if(operators.isEmpty()||precedence(expTokens[i])==0) {
                operators.push(expTokens[i]);
                ++i;
                break;
            }
            if(precedence(operators.top())>=precedence(expTokens[i])) {
                if (operands.length()<2) {       //需要出栈但栈里操作数不够2个了，抛出错误
                    QString error("Illegal Expression! Missing Operands.");
                    throw error;
                }
                QString op = operators.pop();
                rexp = operands.pop();
                lexp = operands.pop();
                newExpression = new CompoundExp(op, lexp, rexp);
                operands.push(newExpression);
            }
            else {
                operators.push(expTokens[i]);
                ++i;
                break;
            }
        }
    }

    while (!operators.isEmpty()) {
        if (operands.length()<2) {       //需要出栈但栈里操作数不够2个了，抛出错误
            QString error("Illegal Expression! Missing Operands.");
            throw error;
        }
        QString op = operators.pop();
        rexp = operands.pop();
        lexp = operands.pop();
        newExpression = new CompoundExp(op, lexp, rexp);
        operands.push(newExpression);
    }

    if(operands.length()==1) {
        return operands.top();
    }
    else {
        QString error("Illegal Expression! Missing Operators.");
        throw error;
    }
}

QList<QString> parser::preprocessForNeg(QList<QString> tokens)
{
    for (int i=0;i<tokens.size();++i) {
        if(tokens[i]=="-") {
            if(i==0||(precedence(tokens[i-1])!=-1)) {
                tokens.insert(i, "0");
                tokens.insert(i, "(");
                i+=2;
                for (int j=i+1, count1=0, count2=0;j<tokens.size();++j) {
                    if(tokens[j]=="(") {
                        count1++;
                        continue;
                    }
                    if(tokens[j]==")") {
                        count2++;
                        if(count1==count2) {
                            tokens.insert(j+1, ")");
                            break;
                        }
                        else continue;
                    }
                    if(isVar(tokens[j])&&count1==count2&&count1==0) {
                        tokens.insert(j+1, ")");
                        break;
                    }
                }
            }
        }
    }
    return tokens;
}

bool parser::isVar(QString var)
{
    return (precedence(var)==-1&&var!=")");
}
