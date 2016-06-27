#pragma once

#include <QString>

#include "tokens.h"
#include "paths.h"
#include "buffer.h"
#include "tree.h"

class SpinDragon : public QObject
{
    Q_OBJECT

    QString _filename;

    Block   _block;

    SpinDragonPaths paths;
    Buffer tok;

private:

    QString allowUnderscore(QString s);

    void getIndent();
    void print(QString s, QString s2);

    void getProgram();
    void getLine();

    void getNewBlock(QString pattern, Block block);
    void getNewFunctionBlock(QString pattern, Block block);

    void getFunction();
    Node * getIdentifier();
    void getArrayIndex();

    void getString();
    void getParameter();
    void getParameters();

    void getConstantLine();

    void getConstantArray();
    void getArrayItem();

    Node * getAssignment();
    Node * getExpression();
    Node * getTerm();
    Node * getFactor();
    Node * getPrimaryExpression();

    void getObjectString();
    void getObjectLine();

    void error(Buffer & in, QString text);
    void error(Match m, QString text);

    Node * getLiteral();
    Node * getNumber();
    Node * getDecimal();
    Node * getFloat();
    Node * getHexadecimal();
    Node * getBinary();
    Node * getQuaternary();

public:
    explicit SpinDragon();
    ~SpinDragon();
    bool parse(QString text, QString filename = QString(), SpinDragonPaths paths = SpinDragonPaths());
    void reset();
};



