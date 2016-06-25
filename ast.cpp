#include "ast.h"

#include <stdio.h>


Node::Node(Buffer & in)
{
    Q_UNUSED(in);
    _line = in.line();
    _col = in.col();
}

Node::~Node()
{
}

void Node::print()
{
    printf("%s '%s' ", qPrintable(_name), qPrintable(_token));
}

QString Node::token()
{
    return _token;
}

// identifier

Identifier::Identifier(Buffer & in)
    : Node(in)
{
    _name = "IDENT";

    try
    {
        _token = in.match("[_a-zA-Z][a-zA-Z_0-9]*");
    }
    catch (Error & e)
    {
        throw Error("Expected an identifer (e.g. 'foobar', 'foo_bar')");
    }

    _value = _token;

    print();
}

QString Identifier::value()
{
    return _value;
}


// numbers

Number::Number(Buffer & in)
    : Node(in)
{
    Q_UNUSED(in);
}

void Number::parse(Buffer & in)
{
    try
    {
        _token = in.match(_pattern);
    }
    catch (Error & e)
    {
        throw Error(_error);
    }

    if (in.isIdentifier()) throw Error(_error);
    print();
}

qint32 Number::value()
{
    return _value;
}

QString Number::allowUnderscore(QString s)
{
    return  "["+s+"]+(["+s+"_]+["+s+"]|["+s+"]*)";
}



Decimal::Decimal(Buffer & in)
    : Number(in)
{
    _name = "DEC";
    _pattern = allowUnderscore("0-9");
    _error = "Not a valid decimal number (0-9)";

    parse(in);
}

Float::Float(Buffer & in)
    : Number(in)
{
    _name = "FLOAT";
    _pattern = allowUnderscore("0-9")+"\\."+allowUnderscore("0-9");
    _error = "Not a valid floating point number (0.0)";

    parse(in);
    if (in.look("\\."))
        throw Error("Floating point numbers may only contain one decimal point.");
}

Hexadecimal::Hexadecimal(Buffer & in)
    : Number(in)
{
    _name = "HEX";
    _pattern = allowUnderscore("0-9a-fA-F");
    _error = "Not a valid hexadecimal number (0-F)";

    in.match("\\$");
    parse(in);
}

Binary::Binary(Buffer & in)
    : Number(in)
{
    _name = "BIN";
    _pattern = allowUnderscore("0-1");
    _error = "Not a valid binary number (0-1)";

    in.match("%");
    parse(in);
}

Quaternary::Quaternary(Buffer & in)
    : Number(in)
{
    _name = "QUAT";
    _pattern = allowUnderscore("0-3");
    _error = "Not a valid quaternary number (0-3)";

    in.match("%%");
    parse(in);
}

