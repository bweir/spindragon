#pragma once

#include <QString>
#include <QList>
#include <QVariant>

#include <stdio.h>
#include "errors.h"

class Node
{
    Match _m;
    QString _name;

protected:
    QVariant _value;

public:
    Node(Match m, QString name)
    {
        _m = m;
        _name = name;
        _value = m.text();
    }

    QString name()
    {
        return _name;
    }

    virtual ~Node() {}

    virtual void print() = 0;

    virtual QVariant value()
    {
        return _value;
    }

    void print(QString s2)
    {
        printf("%s '%s' ", qPrintable(_name), qPrintable(s2));
        fflush(stdout);
    }
};

class DecimalNode : public Node
{
public:
    DecimalNode(Match m)
        : Node(m, "DEC")
    {
        _value = m.text().toInt();
    }

    void print()
    {
        Node::print(QString::number(_value.toInt()));
    }
};


class FloatNode : public Node
{

public:
    FloatNode(Match m)
        : Node(m, "FLOAT")
    {
        _value = m.text().toFloat();
    }

    void print()
    {
        Node::print(QString::number(_value.toFloat()));
    }
};



class HexadecimalNode : public Node
{
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
        Node::print(QString::number(_value.toUInt(), 16));
    }
};


class BinaryNode : public Node
{
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
        QString s = QString::number(_value.toUInt(), 2);
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
};



class QuaternaryNode : public Node
{
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
        Node::print(QString::number(_value.toUInt(), 4));
    }
};


class IdentifierNode : public Node
{
public:
    IdentifierNode(Match m)
        : Node(m, "IDENT")
    {
        _value = m.text();
    }

    void print()
    {
        Node::print(_value.toString());
    }
};

class BinaryOpNode : public Node
{
    QString _op;
    Node * _lhs;
    Node * _rhs;

public:
    BinaryOpNode(Node * lhs, Match op, Node * rhs)
        : Node(op, "OP")
    {
        _lhs = lhs;
        _rhs = rhs;
        _op = op.text();
    }

    void print()
    {
        _lhs->print();
        printf(" %s ", qPrintable(_op));
        _rhs->print();
    }
};


class ExpressionNode : public Node
{
    Node * _term;
    QList<QString> _ops;
    QList<Node *> _terms;

public:
    ExpressionNode(Node * term)
        : Node(Match(), "EXPR")
    {
        _term = term;
    }

    void add(QString op, Node * term)
    {
        _ops.append(op);
        _terms.append(term);
    }

    void print()
    {
        _term->print();
        for (int i = 0; i < _terms.size(); i++)
        {
            printf(" %s ",qPrintable(_ops[i]));
            _terms[i]->print();
        }
    }

    QVariant value()
    {

        if (_terms.isEmpty())
            return _term->value();

        quint32 v1 = _term->value().toInt();
        printf("%i", v1);

        for (int i = 0; i < _terms.size(); i++)
        {
            quint32 v2 = _terms[i]->value().toInt();
            printf(" %s %i", qPrintable(_ops[i]), v2);

            if (_ops[i] == "\\+")
                v1 = v1 + v2;
            else if (_ops[i] == "-")
                v1 = v1 - v2;
            else if (_ops[i] == "\\*")
                v1 = v1 * v2;
            else if (_ops[i] == "/")
                v1 = v1 / v2;
            else if (_ops[i] == "//")
                v1 = v1 % v2;

            // shift operators
            else if (_ops[i] == ">>")
                v1 = v1 >> v2;
            else if (_ops[i] == "~>")
                v1 = ((qint32) v1) >> v2;
            else if (_ops[i] == "<<")
                v1 = v1 << v2;

            // bitwise operators
            else if (_ops[i] == "&")
                v1 = v1 & v2;
            else if (_ops[i] == "\\|")
                v1 = v1 | v2;
            else if (_ops[i] == "\\^")
                v1 = v1 ^ v2;

            // relational operators
            else if (_ops[i] == ">")
                v1 = (v1 > v2 ? -1 : 0);
            else if (_ops[i] == "<")
                v1 = (v1 < v2 ? -1 : 0);
            else if (_ops[i] == ">=")
                v1 = (v1 >= v2 ? -1 : 0);
            else if (_ops[i] == "<=")
                v1 = (v1 <= v2 ? -1 : 0);

            // equality operators
            else if (_ops[i] == "==")
                v1 = (v1 == v2 ? -1 : 0);
            else if (_ops[i] == "!=")
                v1 = (v1 != v2 ? -1 : 0);



        }

        return QVariant(v1);
    }
};




class FactorNode : public Node
{
    Node * _expr;

public:
    FactorNode(Node * expr)
        : Node(Match(), "FACTOR")
    {
        _expr = expr;
    }

    void print()
    {
        printf("(");
        _expr->print();
        printf(")");
        fflush(stdout);
    }

    QVariant value()
    {
        return _expr->value();
    }
};



/*
class ConstantArrayNode : public Node
{
    quint32 _start;
    QList<Node *> _exprs;
    QList<quint32> _offsets;

public:
    FactorNode(Match m)
        : Node(m, "CONST_ARRAY")
    {
        _expr = expr;
    }

    void add(Match op, Node * term)
    {
        _exprs.append(op);
        _terms.append(term);
    }

    void print()
    {
        printf("(");
        _expr->print();
        printf(")");
        fflush(stdout);
    }

    Node * value()
    {
        return _expr;
    }
};

*/
