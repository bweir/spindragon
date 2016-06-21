#include "spindragon.h"

#include <QDebug>

const char* ParseError::what() const throw() {
    return "a parsing error occured";
}

const char* ExpectedError::what() const throw() {
    return "Expected:";
}


SpinDragon::SpinDragon()
{
    reset();
}

SpinDragon::~SpinDragon()
{
    
}

void SpinDragon::reset()
{
    _index = 0;
    _lineindex = 0;
    _text = "";
    _linenum = 1;
    _colnum = 1;
}

void SpinDragon::eatToken(QString t)
{
    _text = t;
    _index += t.size();
    _colnum = _index - _lineindex;
}

bool SpinDragon::atEnd()
{
    return (_index >= _in.size());
}

bool SpinDragon::match(QString pattern)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch m = re.match(_in, _index);

    if (m.hasMatch() && m.capturedStart() == _index)
    {
        eatToken(m.captured());
        return true;
    }
    else
    {
        expected(pattern);
        return false;
    }
}

bool SpinDragon::look(QString pattern)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch m = re.match(_in, _index);

    return (m.hasMatch() && m.capturedStart() == _index);
}

void SpinDragon::expected(QString pattern)
{
    _pattern = pattern;
    throw ExpectedError();
}

void SpinDragon::print(QString s)
{
    printf(" %s '%s'", qPrintable(s), qPrintable(_text));
}

void SpinDragon::label(QString s)
{
    print(s);
    eatSpace();
}


void SpinDragon::getQuaternary()
{
    match("%%"+allowUnderscore("0-3"));
    label("QUAD");
}

void SpinDragon::getBinary()
{
    match("%"+allowUnderscore("0-1"));
    label("BIN");
}

void SpinDragon::getHexadecimal()
{
    match("\\$"+allowUnderscore("0-9a-fA-F"));
    label("HEX");
}

void SpinDragon::getFloat()
{
    match(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9"));
    label("FLOAT");
}

void SpinDragon::getDecimal()
{
    match(allowUnderscore("0-9"));
    label("DEC");
}

void SpinDragon::getIdentifier()
{
    match("[_a-zA-Z][a-zA-Z_0-9]*");
    print("IDENT");

    if (look("\\."))
    {
        match("\\.");
        print("DOT");

        if (look("[_a-zA-Z]"))
            getIdentifier();
    }
    else
    {
        eatSpace();
    }

    if (look("\\("))
    {
        getParameters();
    }
}

void SpinDragon::getParameter()
{
    printf(" P");
    getExpression();

    if (look(","))
    {
        match(",");
        eatSpace();

        getParameter();
    }
}

void SpinDragon::getParameters()
{
    printf(" PARAMS ");

    match("\\(");
    getParameter();
    match("\\)");
}



void SpinDragon::getOperator()
{
    match(":|=| |\\+");
    label("OP");
}

void SpinDragon::getNewLine()
{
    match("\n");

    _linenum++;
    _lineindex = _index;

    printf("\n%4i ", _linenum);

    getIndent();
}

void SpinDragon::getIndent()
{
    int indent = eatSpace();
    for (int i = 0; i < indent; i++)
        printf(".");
}

int SpinDragon::eatSpace()
{
    match("[ \t]*");
    return _text.size();
}

void SpinDragon::getString()
{
    match("\"");

    QString s; 

    while (!atEnd())
    {
        if (look("\""))
        {
            match("\"");
            break;
        }
        else if (look("\n") || atEnd())
        {
            expected("string");
        }
        else if (look("."))
        {
            match(".");
            s += _text;
        }
        else
        {
            expected("string");
        }
    }

    _text = s;

    label("STRING");
}

QString SpinDragon::allowUnderscore(QString s)
{
    return  "["+s+"]+(["+s+"_]+["+s+"]|["+s+"]*)";
}

void SpinDragon::getNumber()
{
    if (look(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9")))
        getFloat();

    else if (look(allowUnderscore("0-9")))
        getDecimal();

    else if (look("\\$"+allowUnderscore("0-9a-fA-F")))
        getHexadecimal();

    else if (look("%%"+allowUnderscore("0-3")))
        getQuaternary();

    else if (look("%"+allowUnderscore("0-1")))
        getBinary();

    else expected("number");
}

void SpinDragon::getExpression()
{
    if (look("[_a-zA-Z]"))
        getIdentifier();

    else if (look("[0-9]|\\$|%|%%"))
        getNumber();

    else if (look(":|=| |\\+"))
        getOperator();

    else if (look("\""))
        getString();

    else if (look("\n"))
        getNewLine();

    else expected("expression");
}

bool SpinDragon::parse(QString text)
{
    reset();
    _in = text;


    while (!atEnd())
    {
        try
        {
            getExpression();
        }
        catch (ParseError & e)
        {
            fflush(stdout);
            qDebug() << "\nError (" << _linenum << "," << _colnum << ")" << e.what() << _pattern;

            printf("%s\n", qPrintable(_in.mid(_lineindex, _in.indexOf("\n",_lineindex) - _lineindex)));
            printf("%s^\n", qPrintable(QString(_colnum, '-')));
            qDebug() << "TEXT" << _text << _text.size();
            return false;
        }
    }

    return true;
}
