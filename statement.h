#ifndef STATEMENT_H
#define STATEMENT_H

#include <QObject>
#include <QWidget>
#include "exp.h"

class MainWindow;

enum stmtType{REM, LET, PRINT, INPUT, GOTO, IF, END, INPUTS, PRINTF};

class statement
{

    friend class program;
    friend class MainWindow;
protected:
    stmtType type;
    evalstate *state;
public:
    statement(evalstate* st);
    virtual ~statement(){}
    virtual int excute() = 0;
    virtual QString toSyntaxTree(){return QString("");}
    static bool isLegalVarName(QString name);
    stmtType getType();
};

class remStmt: public statement
{
private:
    QString text;
public:
    remStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class letStmt: public statement
{
private:
    QString varible;
    Expression* exp;
    QString sValue; //字符串类型时字符串值
public:
    letStmt(QList<QString> tokens, evalstate* st);
    ~letStmt() override;
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class printStmt: public statement
{
private:
    Expression* exp;
public:
    printStmt(QList<QString> tokens, evalstate* st);
    ~printStmt() override;
    int getValue();
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class inputStmt: public statement
{
    QString varible;
public:
    inputStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString getVarName();
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class gotoStmt: public statement
{
    int nextLineNum;
public:
    gotoStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class ifStmt: public statement
{
    Expression *exp1, *exp2;
    int thenNum;
    QString op;
public:
    ifStmt(QList<QString> tokens, evalstate* st);
    ~ifStmt() override;
    int excute() override;
    static bool isOp(QString op);
    bool condition();
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class endStmt: public statement
{
public:
    endStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class inputsStmt: public statement
{
    QString varible;
public:
    inputsStmt(QList<QString> tokens, evalstate* st);
    int excute() override;
    QString getVarName();
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

class printfStmt: public statement
{
private:
    QList<QString> argv;
public:
    printfStmt(QList<QString> tokens, evalstate* st);
    QString getText();
    int excute() override;
    QString toSyntaxTree() override;
    static bool isLegal(QList<QString> tokens);
};

#endif // STATEMENT_H
