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
    void get(QString s);
    void print(QString s, QString s2);

    void getProgram();
    void getLine();

    void getNewBlock(QString pattern, Block block);
    void getNewFunctionBlock(QString pattern, Block block);

    void getFunction();
    void getIdentifier();
    void getOperator();
    void getLiteral();
    void getArrayIndex();
    void getExpression();

    void getString();
    void getParameter();
    void getParameters();

    void getConstantLine();

    void getConstantArray();
    void getConstantArrayItem();

    void getConstantAssignment();
    void getConstantExpression();
    void getConstantAdd();
    void getConstantSubtract();
    void getConstantTerm();
    void getConstantMultiply();
    void getConstantDivide();
    void getConstantFactor();
    void getConstantIdentifier();
    void getConstantPrimaryExpression();


    void getObjectString();
    void getObjectLine();

    void error(Buffer & in, QString text);
    void error(Match m, QString text);

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



