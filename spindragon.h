#pragma once

#include <QString>
#include <QRegularExpression>

#include <exception>

class ParseError : public std::exception
{
public:
    const char* what() const throw();
};

class ExpectedError : public ParseError
{
public:
    const char* what() const throw();
};



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
    QString _pattern;

private:
    void eatToken(QString t);
    bool match(QString pattern);
    bool look(QString pattern);
    void expected(QString pattern);
    void reset();
    bool atEnd();

    QString allowUnderscore(QString s);

    void getIndent();
    void print(QString s);
    int eatSpace();
    void getIdentifier();
    void getOperator();
    void getNewLine();
    void getNumber();
    void getExpression();

    void getString();
    void getParameter();
    void getParameters();

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



