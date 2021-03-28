#include "tokenizer.h"

tokenizer::tokenizer()
{

}

QList<QString> tokenizer::toTokens(QString line)
{
    tokens.clear();
    int pos1 = line.indexOf(' ');
    int pos2 = 0;
    QString token = line.left(pos1);
    tokens.push_back(token);
    pos1++;
    for (pos2 = pos1;pos2<line.length();++pos2) {
        if(line[pos2]=='='||line[pos2]=='<'||line[pos2]=='>') {
            if(pos2>pos1)
                tokens.push_back(line.mid(pos1, pos2-pos1));
            tokens.push_back(QString(line[pos2]));
            pos1 = pos2+1;
        }
        else if (pos2<line.length()-3&&line.mid(pos2,4)=="THEN") {
            if(pos2>pos1)
                tokens.push_back(line.mid(pos1, pos2-pos1));
            tokens.push_back("THEN");
            pos1 = pos2 + 4;
            pos2 += 3;
        }
    }
    if(pos1<line.length()) {
        tokens.push_back(line.mid(pos1));
    }
    return tokens;
}

QList<QString> tokenizer::toExpTokens(QString expString)
{
    QList<QString> expTokens;
    expString.remove(QRegExp("\\s"));   //用正则表达式移除expString内所有空格
    int pos1, pos2;
    for (pos1=pos2=0;pos2<expString.length();++pos2) {
        if(expString.mid(pos2, 2)=="**") {
            if(pos2>pos1)
                expTokens.push_back(expString.mid(pos1, pos2-pos1));
            expTokens.push_back("**");
            pos1 = pos2+2;
            pos2 +=1;
        }
        else if(isOperator(expString[pos2])) {
            if(pos2>pos1)
                expTokens.push_back(expString.mid(pos1, pos2-pos1));
            expTokens.push_back(QString(expString[pos2]));
            pos1 = pos2+1;
        }
    }
    if(pos1<expString.length()) {
        expTokens.push_back(expString.mid(pos1));
    }
    return expTokens;
}

QString tokenizer::getToken(int i)
{
    return tokens[i];
}

QList<QString> tokenizer::getTokens()
{
    return tokens;
}

bool tokenizer::isOperator(QChar obj)
{
    if(obj=='+'||obj=='-'||obj=='*'||obj=='/'||obj=='('||obj==')')
        return true;
    return false;
}

int tokenizer::size()
{
    return tokens.size();
}
