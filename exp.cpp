#include "exp.h"

ConstantExp::ConstantExp(int val)
{
    value = val;
}

int ConstantExp::eval(evalstate & state) {
    return value;
}

QString ConstantExp::toString()
{
    return QString::number(value);
}

ExpressionType ConstantExp::type()
{
    return CONSTANT;
}

int ConstantExp::getConstantValue()
{
    return value;
}

QString ConstantExp::toSyntaxTreeString(int level)
{
    QString tmpString=QString::number(value);
    for (int i=1;i<level;++i) {
        tmpString.prepend("    ");
    }
    tmpString.append("\n");
    return tmpString;
}

IdentifierExp::IdentifierExp(QString name)
{
    this->name = name;
}

int IdentifierExp::eval(evalstate & state) {
   return state.getValue(name);
}

QString IdentifierExp::toString()
{
    return name;
}

ExpressionType IdentifierExp::type()
{
    return IDENTIFIER;
}

QString IdentifierExp::getIdentifierName()
{
    return name;
}

QString IdentifierExp::toSyntaxTreeString(int level)
{
    QString tmpString = name;
    for (int i=1;i<level;++i) {
        tmpString.prepend("    ");
    }
    tmpString.append("\n");
    return tmpString;
}

CompoundExp::CompoundExp(QString op, Expression *lhs, Expression *rhs)
{
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

CompoundExp::~CompoundExp()
{
    if(lhs) delete lhs;
    if(rhs) delete rhs;
}

int CompoundExp::eval(evalstate & state) {
   int right = rhs->eval(state);
   if (op == "=") {
      state.setValue(lhs->getIdentifierName(), right);
      return right;
   }
   int left = lhs->eval(state);
   if (op == "**") return pow(left, right);
   if (op == "+") return left + right;
   if (op == "-") return left - right;
   if (op == "*") return left * right;
   if (op == "/") {
      if (right == 0) {
        QString error("Division by 0");
        throw error;
      }
      return left / right;
   }
   QString error("Illegal operator in expression");
   throw error;
   //return 0;
}

QString CompoundExp::toString()
{
    return op;
}

ExpressionType CompoundExp::type()
{

}

QString CompoundExp::getOperator()
{

}

Expression *CompoundExp::getLHS()
{
    return lhs;
}

Expression *CompoundExp::getRHS()
{
    return rhs;
}

QString CompoundExp::toSyntaxTreeString(int level)
{
    QString tmpString;
    QQueue<Expression*> queue;
    Expression *tmp=nullptr;
    queue.enqueue(this);
    queue.enqueue(nullptr);     //?????????????????????????????????????????????
    while (true) {
        tmp=queue.dequeue();
        if(tmp==nullptr) {      //?????????????????????????????????????????????????????????????????????????????????????????????????????????
            if(queue.isEmpty()) break;      //????????????????????????????????????????????????????????????
            ++level;
            queue.enqueue(nullptr);
            continue;
        }
        for (int i=1;i<level;++i) {
            tmpString.append("    ");
        }
        tmpString+=tmp->toString();
        tmpString.append("\n");
        if(tmp->getLHS()) queue.enqueue(tmp->getLHS());
        if(tmp->getRHS()) queue.enqueue(tmp->getRHS());
    }
    return  tmpString;
}

