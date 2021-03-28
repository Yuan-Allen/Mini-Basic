#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <QWidget>
#include "statement.h" 
#include <QMap>

class program
{
    friend class MainWindow;
private:
    QMap<int, QString> stmtStrings;
    QMap<int, statement*> stmts;
public:
    program();
    void addNewStmtString(int lineNum, QString line);    //增加新statement（以字符串形式存储）
    void addNewStatement(int lineNum, statement* newStatement);    //增加新statement
    void removeStatement(int lineNum);
    void clear();
};

#endif // PROGRAM_H
