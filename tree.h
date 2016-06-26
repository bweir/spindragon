#pragma once

#include <QString>
#include <stdio.h>
#include "errors.h"

class Node
{
    Match _m;
    QString _name;

public:
    Node(Match m, QString name)
    {
        _m = m;
        _name = name;
    }

    QString name()
    {
        return _name;
    }

    virtual ~Node() {}

    virtual void print() = 0;

    void print(QString s2)
    {
        printf("%s '%s' ", qPrintable(_name), qPrintable(s2));
        fflush(stdout);
    }
};

class DecimalNode : public Node
{
    quint32 _value;

public:
    DecimalNode(Match m)
        : Node(m, "DEC")
    {
        _value = m.text().toInt();
    }

    void print()
    {
        Node::print(QString::number(_value));
    }

    quint32 value()
    {
        return _value;
    }
};


class FloatNode : public Node
{
    float _value;

public:
    FloatNode(Match m)
        : Node(m, "FLOAT")
    {
        _value = m.text().toFloat();
    }

    void print()
    {
        Node::print(QString::number(_value));
    }

    float value()
    {
        return _value;
    }
};



class HexadecimalNode : public Node
{
    quint32 _value;

public:
    HexadecimalNode(Match m)
        : Node(m, "HEX")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 8)
            throw Error("Hexadecimal numbers may not exceed 8 characters!");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 16);

        if (!ok)
            throw Error("Failed to read hexadecimal number!");
    }

    void print()
    {
        Node::print(QString::number(_value, 16));
    }

    quint32 value()
    {
        return _value;
    }
};


class BinaryNode : public Node
{
    quint32 _value;

public:
    BinaryNode(Match m)
        : Node(m, "BIN")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 32)
            throw Error("Binary numbers may not exceed 32 characters.");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 2);

        if (!ok)
            throw Error("Failed to read binary number!");
    }

    void print()
    {
        QString s = QString::number(_value, 2);
        printf("%s '", qPrintable(name()));
        for (int i = 0; i < s.size(); i++)
        {
            printf("%s",qPrintable(s[i]));
            if (i < s.size()-1 && i % 8 == 7)
                printf("_");
        }
        printf("'");
        fflush(stdout);
    }

    quint32 value()
    {
        return _value;
    }
};



class QuaternaryNode : public Node
{
    quint32 _value;

public:
    QuaternaryNode(Match m)
        : Node(m, "QUAT")
    {
        QString s = m.text();
        s = s.replace("_","");
        if (s.size() > 32)
            throw Error("Quaternary numbers may not exceed 16 characters!");

        QByteArray ba = s.toLocal8Bit();
        bool ok;

        _value = ba.toUInt(&ok, 4);

        if (!ok)
            throw Error("Failed to read quaternary number!");
    }

    void print()
    {
        Node::print(QString::number(_value, 4));
    }

    quint32 value()
    {
        return _value;
    }
};
