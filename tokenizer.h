#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QRegExp>

class tokenizer
{
private:
    QList<QString> tokens;
public:
    tokenizer();
    QList<QString> toTokens(QString line);
    QList<QString> toExpTokens(QString expString);
    QList<QString> toStringTokens(QString line, bool &flag);
    QString getToken(int i);
    QList<QString> getTokens();
    bool isOperator(QChar obj);
    int size();
};

#endif // TOKENIZER_H
