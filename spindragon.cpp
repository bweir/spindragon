#include "spindragon.h"
#include "errors.h"

#include <QDebug>

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

    _block = NO_BLOCK;
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

bool SpinDragon::match(QString pattern, bool casesensitive)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    if (!casesensitive)
    {
        re.setPatternOptions(QRegularExpression::DotMatchesEverythingOption
                           | QRegularExpression::CaseInsensitiveOption);
    }

    QRegularExpressionMatch m = re.match(_in, _index);

    if (m.hasMatch() && m.capturedStart() == _index)
    {
        eatToken(m.captured());
        return true;
    }
    else
    {
        throw ExpectedError(pattern);
        return false;
    }
}

bool SpinDragon::look(QString pattern, bool casesensitive)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption);
    if (!casesensitive)
    {
        re.setPatternOptions(QRegularExpression::DotMatchesEverythingOption
                           | QRegularExpression::CaseInsensitiveOption);
    }

    QRegularExpressionMatch m = re.match(_in, _index);

    return (m.hasMatch() && m.capturedStart() == _index);
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
    match("%%");
    try
    {
        match(allowUnderscore("0-3"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid quaternary number (0-3)");
    }

    if (isIdentifier()) throw Error("Not a valid quaternary number (0-3)");
    print("QUAD");
}

void SpinDragon::getBinary()
{
    match("%");
    try
    {
        match(allowUnderscore("0-1"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid binary number (0-1)");
    }

    if (isIdentifier()) throw Error("Not a valid binary number (0-1)");
    print("BIN");
}

void SpinDragon::getHexadecimal()
{
    match("\\$");
    try
    {
        match(allowUnderscore("0-9a-fA-F"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid hexadecimal number (0-F)");
    }

    print("HEX");
    if (look("[g-zG-Z]")) throw Error("Not a valid hexadecimal number (0-F)");
}

void SpinDragon::getFloat()
{
    try
    {
        match(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid floating point number (0.0)");
    }

    print("FLOAT");
    if (isIdentifier()) throw Error("Not a valid floating point number (0.0)");
}

void SpinDragon::getDecimal()
{
    try
    {
        match(allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid decimal number (0-9)");
    }

    print("DEC");
    if (isIdentifier()) throw Error("Not a valid decimal number (0-9)");
}

void SpinDragon::getLiteral()
{
    match("#");
    getNumber();
    eatSpace();
}

void SpinDragon::getFunction()
{
    getIdentifier();

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

void SpinDragon::getIdentifier()
{
    match("[_a-zA-Z][a-zA-Z_0-9]*");
    print("IDENT");
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
    print("OP");
}

void SpinDragon::getNewLine()
{
    match("\n");

    _linenum++;
    _lineindex = _index;

    printf(" NL\n");
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
            throw ExpectedError("string");
        }
        else if (look("."))
        {
            match(".");
            s += _text;
        }
        else
        {
            throw ExpectedError("string");
        }
    }

    _text = s;

    print("STRING");
    eatSpace();
}

QString SpinDragon::allowUnderscore(QString s)
{
    return  "["+s+"]+(["+s+"_]+["+s+"]|["+s+"]*)";
}

void SpinDragon::getNumber()
{
    if (look(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9")))
        getFloat();

    else if (look("[0-9]"))
        getDecimal();

    else if (look("\\$"))
        getHexadecimal();

    else if (look("%%"))
        getQuaternary();

    else if (look("%"))
        getBinary();

    else throw ExpectedError("number");
}


bool SpinDragon::isIdentifier()
{
    return look("[_a-zA-Z]");
}

bool SpinDragon::isNumber()
{
    return look("[0-9]|\\$|%|%%");
}

bool SpinDragon::isString()
{
    return look("\"");
}

void SpinDragon::getExpression()
{
    if (isIdentifier())
        getFunction();

    else if (isNumber())
        getNumber();

    else if (look(":|=|\\+"))
        getOperator();

    else if (isString())
        getString();

    else if (look("\n"))
        getNewLine();

    else throw ExpectedError("expression");
}

void SpinDragon::get(QString s)
{
    match(s);
    print("OP");
    eatSpace();
}


void SpinDragon::getConstantFactor()
{
    if (look("\\("))
    {
        get("\\(");
        getConstantExpression();
        get("\\)");
    }
    else
    {
        getConstantPrimaryExpression();
    }
}

void SpinDragon::getConstantMultiply()
{
    get("\\*");
    getConstantFactor();
}

void SpinDragon::getConstantDivide()
{
    get("/");
    getConstantFactor();
}


void SpinDragon::getConstantTerm()
{
    getConstantFactor();

    while (look("\\*|/"))
    {
        if (look("\\*"))
            getConstantMultiply();

        else if (look("/"))
            getConstantDivide();
    }
}

void SpinDragon::getConstantSubtract()
{
    get("-");
    getConstantTerm();
}

void SpinDragon::getConstantAdd()
{
    get("\\+");
    getConstantTerm();
}

void SpinDragon::getConstantExpression()
{
    if (!look("\\+|-"))
        getConstantTerm();

    while (look("\\+|-"))
    {
        if (look("\\+"))
            getConstantAdd();

        else if (look("-"))
            getConstantSubtract();
    }
}

void SpinDragon::getConstantPrimaryExpression()
{
    if (isIdentifier())
        getIdentifier();

    else if (isNumber())
        getNumber();

    else throw ExpectedError("primary expression");

    eatSpace();
}

void SpinDragon::getConstantIdentifier()
{
    getIdentifier();
    eatSpace();
}

void SpinDragon::getConstantAssignment()
{
    getConstantIdentifier();
    get("=");
    getConstantExpression();
}

void SpinDragon::getConstantArrayItem()
{
    getIdentifier();
    eatSpace();

    if (look("\\["))
    {
        get("\\[");
        getConstantPrimaryExpression();
        get("\\]");
    }
}

void SpinDragon::getConstantArray()
{
    getLiteral();

    while (look(","))
    {
        get(",");
        getConstantArrayItem();
    }
}

void SpinDragon::getConstantLine()
{

    getIndent();

    if (isIdentifier())
        getConstantAssignment();

    else if (look("#"))
        getConstantArray();

    else throw ExpectedError("constant assignment (e.g. VAR = 123)");
}

bool SpinDragon::isEmptyLine()
{
    return look("[ \t]*\n");
}

void SpinDragon::getLine()
{
    printf("%4i| ", _linenum);

    if (isEmptyLine())
    {
        eatSpace();
        return;
    }

    if (look("con", false))
    {
        match("con[ \t]*", false);
        _block = CON_BLOCK;
        return;
    }

    switch (_block)
    {
        case NO_BLOCK:   throw Error("You haven't opened a block yet (CON? PUB?)");
                         return;

        case CON_BLOCK:  getConstantLine();
                         break;

        case VAR_BLOCK:  return;

        case OBJ_BLOCK:  return;

        case PUB_BLOCK:   
        case PRI_BLOCK:  return;

        case DAT_BLOCK:  return;

        case ASM_BLOCK:  return;

        default:
            throw Error("INVALID BLOCK");
    }
}

void SpinDragon::getProgram()
{
    getLine();

    while (!atEnd())
    {
        getNewLine();
        if (atEnd())
            return;
        getLine();
    }
}


bool SpinDragon::parse(QString text)
{
    reset();
    _in = text;

    _block = NO_BLOCK;

    try
    {
        getProgram();
    }
    catch (Error & e)
    {
        fflush(stdout);
        printf("%s\n", qPrintable(QString("\n\033[1;31mError (line %1, col %2):\033[0m %3").arg(_linenum).arg(_colnum).arg(e.what())));

        printf("\n%s\n", qPrintable(_in.mid(_lineindex, _in.indexOf("\n",_lineindex) - _lineindex)));
        printf("%s\033[1;37m^\033[0m\n", qPrintable(QString(_colnum, ' ')));
        return false;
    }

    return true;
}
