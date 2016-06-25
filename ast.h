#pragma once

#include <QObject>
#include <QString>

#include "buffer.h"
#include "errors.h"

class Node
{
    int _line, _col;

protected:
    QString _name, _pattern, _token;

public:
    Node(Buffer & in);
    virtual ~Node();

    void print();
    QString name();
    QString token();
};




class Identifier : public Node
{
    QString _value;

public:
    Identifier(Buffer & in);
    QString value();
};



class Number : public Node
{
protected:
    qint32 _value;
    QString _error;
    QString allowUnderscore(QString s);

public:
    Number(Buffer & in);
    void parse(Buffer & in);
    qint32 value();
};

class Decimal : public Number
{
public:
    Decimal(Buffer & in);
};

class Float: public Number
{
public:
    Float(Buffer & in);
};

class Hexadecimal : public Number
{
public:
    Hexadecimal(Buffer & in);
};

class Binary : public Number
{
public:
    Binary(Buffer & in);
};

class Quaternary : public Number
{
public:
    Quaternary(Buffer & in);
};


