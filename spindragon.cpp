#include "spindragon.h"
#include "errors.h"

#include <QFileInfo>
#include <QDebug>

SpinDragon::SpinDragon()
    : QObject()
{
    reset();
}

SpinDragon::~SpinDragon()
{
    
}

void SpinDragon::reset()
{
    tok.reset();

    _block = NO_BLOCK;
}

void SpinDragon::print(QString s, QString s2)
{
    printf(" %s '%s'", qPrintable(s), qPrintable(s2));
    fflush(stdout);
}

void SpinDragon::getQuaternary()
{
    tok.match("%%");
    QString s;
    try
    {
        s = tok.match(allowUnderscore("0-3"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid quaternary number (0-3)");
    }

    if (isIdentifier()) throw Error("Not a valid quaternary number (0-3)");
    print("QUAD", s);
}

void SpinDragon::getBinary()
{
    tok.match("%");
    QString s;
    try
    {
        s = tok.match(allowUnderscore("0-1"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid binary number (0-1)");
    }

    if (isIdentifier()) throw Error("Not a valid binary number (0-1)");
    print("BIN", s);
}

void SpinDragon::getHexadecimal()
{
    tok.match("\\$");
    QString s;
    try
    {
        s = tok.match(allowUnderscore("0-9a-fA-F"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid hexadecimal number (0-F)");
    }

    if (tok.look("[g-zG-Z]")) throw Error("Not a valid hexadecimal number (0-F)");
    print("HEX", s);
}

void SpinDragon::getFloat()
{
    QString s;
    try
    {
        s = tok.match(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid floating point number (0.0)");
    }

    if (isIdentifier()) throw Error("Not a valid floating point number (0.0)");
    print("FLOAT", s);
}

void SpinDragon::getDecimal()
{
    QString s;
    try
    {
        s = tok.match(allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid decimal number (0-9)");
    }

    if (isIdentifier()) throw Error("Not a valid decimal number (0-9)");
    print("DEC", s);
}

void SpinDragon::getLiteral()
{
    tok.match("#");
    getNumber();
    tok.eatSpace();
}

void SpinDragon::getArrayIndex()
{
    get("\\[");
    getConstantExpression();
    get("\\]");
}

void SpinDragon::getFunction()
{
    getIdentifier();

    if (tok.look("\\."))
    {
        QString s = tok.match("\\.");
        print("DOT", s);

        if (tok.look("[_a-zA-Z]"))
            getIdentifier();
    }
    else
    {
        tok.eatSpace();
    }

    if (tok.look("\\("))
    {
        getParameters();
    }
}

void SpinDragon::getIdentifier()
{
    try
    {
        QString s = tok.match("[_a-zA-Z][a-zA-Z_0-9]*");
        print("IDENT", s);
    }
    catch (Error & e)
    {
        throw Error("Expected an identifer (e.g. 'foobar', 'foo_bar')");
    }
}

void SpinDragon::getParameter()
{
    printf(" P");
    fflush(stdout);

    getExpression();

    if (tok.look(","))
    {
        tok.match(",");
        tok.eatSpace();

        getParameter();
    }
}

void SpinDragon::getParameters()
{
    printf(" PARAMS ");
    fflush(stdout);

    tok.match("\\(");
    getParameter();
    tok.match("\\)");
}

void SpinDragon::getOperator()
{
    QString s = tok.match(":|=| |\\+");
    print("OP", s);
}

void SpinDragon::getIndent()
{
    int indent = tok.eatSpace();

    for (int i = 0; i < indent; i++)
        printf(".");

    fflush(stdout);
}

void SpinDragon::getString()
{
    tok.match("\"");

    QString s; 

    while (!tok.atEnd())
    {
        if (tok.look("\""))
        {
            tok.match("\"");
            break;
        }
        else if (tok.look("\n") || tok.atEnd())
        {
            throw ExpectedError("string");
        }
        else if (tok.look("."))
        {
            s += tok.match(".");
        }
        else
        {
            throw ExpectedError("string");
        }
    }

    print("STRING", s);
    tok.eatSpace();
}

QString SpinDragon::allowUnderscore(QString s)
{
    return  "["+s+"]+(["+s+"_]+["+s+"]|["+s+"]*)";
}

void SpinDragon::getNumber()
{
    if (tok.look(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9")))
        getFloat();

    else if (tok.look("[0-9]"))
        getDecimal();

    else if (tok.look("\\$"))
        getHexadecimal();

    else if (tok.look("%%"))
        getQuaternary();

    else if (tok.look("%"))
        getBinary();

    else throw ExpectedError("number");
}


bool SpinDragon::isIdentifier()
{
    return tok.look("[_a-zA-Z]");
}

bool SpinDragon::isNumber()
{
    return tok.look("[0-9]|\\$|%|%%");
}

bool SpinDragon::isString()
{
    return tok.look("\"");
}

void SpinDragon::getExpression()
{
    if (isIdentifier())
        getFunction();

    else if (isNumber())
        getNumber();

    else if (tok.look(":|=|\\+"))
        getOperator();

    else if (isString())
        getString();

    else if (tok.look("\n"))
        tok.getNewLine();

    else throw ExpectedError("expression");
}

void SpinDragon::get(QString s)
{
    tok.match(s);
    print("OP", s);
    tok.eatSpace();
}


void SpinDragon::getConstantFactor()
{
    if (tok.look("\\("))
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

    while (tok.look("\\*|/"))
    {
        if (tok.look("\\*"))
            getConstantMultiply();

        else if (tok.look("/"))
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
    if (!tok.look("\\+|-"))
        getConstantTerm();

    while (tok.look("\\+|-"))
    {
        if (tok.look("\\+"))
            getConstantAdd();

        else if (tok.look("-"))
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

    tok.eatSpace();
}

void SpinDragon::getConstantIdentifier()
{
    getIdentifier();
    tok.eatSpace();
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
    tok.eatSpace();

    if (tok.look("\\["))
    {
        get("\\[");
        getConstantPrimaryExpression();
        get("\\]");
    }
}

void SpinDragon::getConstantArray()
{
    getLiteral();

    while (tok.look(","))
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

    else if (tok.look("#"))
        getConstantArray();

    else throw ExpectedError("constant assignment (e.g. VAR = 123)");
}

void SpinDragon::getObjectString()
{
    tok.match("\"");

    QString s; 

    while (!tok.atEnd())
    {
        if (tok.look("\""))
        {
            tok.match("\"");
            break;
        }
        else if (tok.look("\n") || tok.atEnd())
        {
            throw Error("Invalid character in object name (valid characters: a-zA-Z0-9._-)");
        }
        else if (tok.look("[_a-zA-Z0-9.-]"))
        {
            s += tok.match("[_a-zA-Z0-9.-]+");
        }
        else
        {
            throw Error("Invalid character in object name (valid characters: a-zA-Z0-9._-)");
        }
    }

    print("OBJ", s);
}


void SpinDragon::getObjectLine()
{
    tok.eatSpace();

    getIdentifier();
    tok.eatSpace();

    if (tok.look("\\["))
        getArrayIndex();

    get(":");
    getObjectString();
}


bool SpinDragon::isEmptyLine()
{
    return tok.look("[ \t]*\n");
}

void SpinDragon::getNewBlock(QString pattern, Block block)
{
    QString s = tok.match(pattern);
    print("NEWBLOCK", s);
    tok.eatSpace();
    _block = block;
    if (tok.look("[^\n]")) 
    {
        throw Error(QString("%1 blocks must be on a line by themselves")
                .arg(pattern.toUpper()));
    }
}

void SpinDragon::getNewFunctionBlock(QString pattern, Block block)
{
    QString s = tok.match(pattern);
    print("NEWBLOCK", s);
    tok.eatSpace();
    _block = block;

    getFunction();

    if (tok.look("[^\n]")) 
    {
        throw Error(QString("%1 blocks should contain only a function declaration (e.g. 'PUB foo(bar, baz)'")
                .arg(pattern.toUpper()));
    }
}

void SpinDragon::getLine()
{
    printf("%4i| ", tok.line());
    fflush(stdout);

    if (isEmptyLine())
        tok.eatSpace();

    else if (tok.look("con"))
        getNewBlock("con", CON_BLOCK);

    else if (tok.look("var"))
        getNewBlock("var", VAR_BLOCK);

    else if (tok.look("obj"))
        getNewBlock("obj", OBJ_BLOCK);

    else if (tok.look("dat"))
        getNewBlock("dat", DAT_BLOCK);

    else if (tok.look("pub"))
        getNewFunctionBlock("pub", DAT_BLOCK);

    else if (tok.look("pri"))
        getNewFunctionBlock("pri", DAT_BLOCK);

    else
    {
        switch (_block)
        {
            case NO_BLOCK:   throw Error("You haven't opened a block yet (CON? PUB?)");
                             return;
    
            case CON_BLOCK:  getConstantLine();
                             break;
    
            case VAR_BLOCK:  return;
    
            case OBJ_BLOCK:  getObjectLine();
                             return;
    
            case PUB_BLOCK:   
            case PRI_BLOCK:  return;
    
            case DAT_BLOCK:  return;
    
            case ASM_BLOCK:  return;
    
            default:
                throw Error("INVALID BLOCK");
        }
    }
}

void SpinDragon::getProgram()
{
    getLine();

    while (!tok.atEnd())
    {
        tok.getNewLine();
        if (tok.atEnd())
            return;
        getLine();
    }
}


bool SpinDragon::parse(QString text, QString filename, SpinDragonPaths paths)
{
    reset();

    tok = SpinDragonBuffer(text, QFileInfo(filename).fileName());

    _block = NO_BLOCK;

    qDebug() << paths.listFiles();
    qDebug() << paths.listPaths();

    try
    {
        getProgram();
    }
    catch (Error & e)
    {
        error(e.what());
        return false;
    }

    return true;
}

void SpinDragon::error(QString s)
{
    fprintf(stderr, "%s\n", qPrintable(QString("\n\033[1;31m%1(%2,%3) Error:\033[0m %4")
                .arg(tok.name())
                .arg(tok.line())
                .arg(tok.col())
                .arg(s)
                ));

    fprintf(stderr,"\n%s\n", qPrintable(tok.lastLine()));
    fprintf(stderr,"%s\033[1;37m^\033[0m\n", qPrintable(QString(tok.col(), ' ')));
    fflush(stderr);
}
