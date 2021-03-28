#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QWidget>
#include <QStack>
#include "statement.h"
#include "tokenizer.h"
#include "exp.h"

class MainWindow;

class parser
{
private:
    evalstate *state;
    tokenizer myTokenizer;
public:
    parser(evalstate *states);
    statement* parse(QString line);
    Expression* parseExp(QString expString);
    QList<QString> preprocessForNeg(QList<QString> tokens); //生成表达式树前的预处理，为输入的负数补0和括号。如-2 --> (0-2), -(1+1) --> (0-(1+1))
    bool isVar(QString var);
};

#endif // PARSER_H
