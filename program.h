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
    ~program();
    void addNewStmtString(int lineNum, QString line);    //增加新statement（以字符串形式存储）
    void addNewStatement(int lineNum, statement* newStatement);    //增加新statement
    void removeStatement(int lineNum);
    void clear();
    void clearStmts();  //清空statements，但是不清空字符串
};

#endif // PROGRAM_H
