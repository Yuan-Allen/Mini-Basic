#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QString>

class evalstate {

public:

   evalstate();
   void setValue(QString var, int value);
   int getValue(QString var);
   bool isDefined(QString var);
   void clear();

private:

   QMap<QString,int> symbolTable;

};

#endif // EVALSTATE_H
