/*
 * File: exp.h
 * -----------
 * This interface defines a class hierarchy for arithmetic expressions.
 */

#ifndef _exp_h
#define _exp_h

#include <QString>
#include <QMap>
#include <qmath.h>
#include <QQueue>
#include "tokenizer.h"
#include "evalstate.h"

using namespace std;
/* Forward reference */

//class EvaluationContext;

/*
 * Type: ExpressionType
 * --------------------
 * This enumerated type is used to differentiate the three different
 * expression types: CONSTANT, IDENTIFIER, and COMPOUND.
 */

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

/*
 * Class: Expression
 * -----------------
 * This class is used to represent a node in an expression tree.
 * Expression itself is an abstract class.  Every Expression object
 * is therefore created using one of the three concrete subclasses:
 * ConstantExp, IdentifierExp, or CompoundExp.
 */

class Expression {

public:

   Expression(){}
   virtual ~Expression(){}
   virtual int eval(evalstate & state){return 0;}
   virtual QString toString(){return QString("");}
   virtual ExpressionType type(){return CONSTANT;}
   virtual QString toSyntaxTreeString(int level){return QString("");}

/* Getter methods for convenience */

   virtual int getConstantValue(){return 0;}
   virtual QString getIdentifierName(){return QString("");}
   virtual QString getOperator(){return QString("");}
   virtual Expression *getLHS(){return nullptr;}
   virtual Expression *getRHS(){return nullptr;}

};

/*
 * Class: ConstantExp
 * ------------------
 * This subclass represents a constant integer expression.
 */

class ConstantExp: public Expression {

public:

   ConstantExp(int val);

   virtual int eval(evalstate & state);
   virtual QString toString();
   virtual ExpressionType type();

   virtual int getConstantValue();
   virtual QString toSyntaxTreeString(int level);

private:

   int value;

};

/*
 * Class: IdentifierExp
 * --------------------
 * This subclass represents a expression corresponding to a variable.
 */

class IdentifierExp: public Expression {

public:

   IdentifierExp(QString name);

   virtual int eval(evalstate & state);
   virtual QString toString();
   virtual ExpressionType type();

   virtual QString getIdentifierName();
   virtual QString toSyntaxTreeString(int level);

private:

   QString name;

};

/*
 * Class: CompoundExp
 * ------------------
 * This subclass represents a compound expression.
 */

class CompoundExp: public Expression {

public:

   CompoundExp(QString op, Expression *lhs, Expression *rhs);
   virtual ~CompoundExp();

   virtual int eval(evalstate & state);
   virtual QString toString();
   virtual ExpressionType type();

   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();

   virtual QString toSyntaxTreeString(int level);

private:

   QString op;
   Expression *lhs, *rhs;

};

/*
 * Class: EvaluationContext
 * ------------------------
 * This class encapsulates the information that the evaluator needs to
 * know in order to evaluate an expression.
 */

//class EvaluationContext {

//public:

//   void setValue(QString var, int value);
//   int getValue(QString var);
//   bool isDefined(QString var);

//private:

//   QMap<QString,int> symbolTable;

//};

#endif



