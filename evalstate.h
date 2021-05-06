#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QString>

class evalstate {

    friend class MainWindow;
public:

   evalstate();
   void setValue(QString var, int value);
   void setValue(QString var, QString sValue);  //重载，用来存字符串型变量
   int getValue(QString var);
   QString getString(QString var);  //获取字符串型变量值
   bool isDefinedInt(QString var);
   bool isDefinedString(QString var);   //字符串型变量是否被定义
   void clear();

private:

   QMap<QString, int> symbolTable;  //存int型变量
   QMap<QString, QString> stringSymTable;   //存字符串型变量

};

#endif // EVALSTATE_H
