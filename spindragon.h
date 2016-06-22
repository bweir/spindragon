#pragma once

#include <QString>
#include <QRegularExpression>

#include "tokens.h"

class Production;

class Production
{
public:
    virtual ~Production();
};


class SpinDragon
{
    int     _linenum;
    int     _colnum;
    int     _lineindex;
    int     _index;
    QString _in;
    QString _text;

    Block   _block;



private:
    void eatToken(QString t);
    bool match(QString pattern, bool casesensitive = true);
    bool look(QString pattern, bool casesensitive = true);
    void reset();
    bool atEnd();

    QString allowUnderscore(QString s);

    bool isEmptyLine();
    bool isIdentifier();
    bool isNumber();
    bool isString();

    void getIndent();
    void get(QString s);
    void print(QString s);
    int eatSpace();

    void getProgram();
    void getLine();

    void getFunction();
    void getIdentifier();
    void getOperator();
    void getNewLine();
    void getNumber();
    void getLiteral();
    void getExpression();

    void getString();
    void getParameter();
    void getParameters();

    void getConstant();
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


    void getFloat();
    void getDecimal();
    void getHexadecimal();
    void getBinary();
    void getQuaternary();

    void label(QString s);


public:
    explicit SpinDragon();
    ~SpinDragon();
    bool parse(QString text);
};



