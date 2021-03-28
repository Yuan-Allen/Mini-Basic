#ifndef STATEMENT_H
#define STATEMENT_H

#include <QObject>
#include <QWidget>
#include "exp.h"

class MainWindow;

enum stmtType{REM, LET, PRINT, INPUT, GOTO, IF, END};

class statement
{

    friend class program;
    friend class MainWindow;
protected:
    stmtType type;
    evalstate *state;
public:
    statement(evalstate* st);
    virtual int excute() = 0;
    virtual QString toSyntaxTree(){return QString("");}
};

class remStmt: public statement
{
private:
    QString text;
public:
    remStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
};

class letStmt: public statement
{
private:
    QString varible;
    Expression* exp;
public:
    letStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
};

class printStmt: public statement
{
private:
    Expression* exp;
public:
    printStmt(QList<QString> tokens, evalstate* st);
    int getValue();
    int excute() override;
    QString toSyntaxTree() override;
};

class inputStmt: public statement
{
    QString varible;
public:
    inputStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString getVarName();
    QString toSyntaxTree() override;
};

class gotoStmt: public statement
{
    int nextLineNum;
public:
    gotoStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
};

class ifStmt: public statement
{
    Expression *exp1, *exp2;
    int thenNum;
    QString op;
public:
    ifStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    bool isOp(QString op);
    bool condition();
    QString toSyntaxTree() override;
};

class endStmt: public statement
{
public:
    endStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
};

#endif // STATEMENT_H
