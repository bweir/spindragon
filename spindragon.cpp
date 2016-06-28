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

Node * SpinDragon::getQuaternary()
{
    tok.match("%%");
    Match m;
    try
    {
        m = tok.match(allowUnderscore("0-3"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid quaternary number (0-3)");
    }

    if (tok.isIdentifier())
        throw Error("Quaternary numbers may not contain letters.");

    return new QuaternaryNode(m);
}

Node * SpinDragon::getBinary()
{
    tok.match("%");
    Match m;
    try
    {
        m = tok.match(allowUnderscore("0-1"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid binary number (0-1)");
    }

    if (tok.isIdentifier())
        throw Error("Binary numbers may not contain letters.");

    return new BinaryNode(m);
}

Node * SpinDragon::getHexadecimal()
{
    tok.match("\\$");
    Match m;
    try
    {
        m = tok.match(allowUnderscore("0-9a-fA-F"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid hexadecimal number (0-F)");
    }

    if (tok.look("[g-zG-Z]"))
        throw Error("Not a valid hexadecimal number (0-F)");

    return new HexadecimalNode(m);
}

Node * SpinDragon::getFloat()
{
    Match m;
    try
    {
        m = tok.match(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid floating point number (0.0)");
    }

    if (tok.isIdentifier())
        throw Error("Floating point numbers may not contain letters.");
    else if (tok.look("\\."))
        throw Error("Floating point numbers may only contain one decimal point.");

    return new FloatNode(m);
}

Node * SpinDragon::getDecimal()
{
    Match m;
    try
    {
        m = tok.match(allowUnderscore("0-9"));
    }
    catch (Error & e)
    {
        throw Error("Not a valid decimal number (0-9)");
    }

    if (tok.isIdentifier())
        throw Error("Decimal numbers may not contain letters.");

    return new DecimalNode(m);
}

Node * SpinDragon::getLiteral()
{
    tok.match("#");
    Node * n = getBitwiseOrExpression();
    tok.eatSpace();
    return n;
}

void SpinDragon::getArrayIndex()
{
    tok.get("\\[");
    getExpression();
    tok.get("\\]");
}

void SpinDragon::getFunction()
{
    getIdentifier()->print();

    if (tok.look("\\."))
    {
        Match m = tok.match("\\.");
        print("DOT", m.text());

        if (tok.look("[_a-zA-Z]"))
            getIdentifier()->print();
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

Node * SpinDragon::getIdentifier()
{
    try
    {
        Match m = tok.match("[_a-zA-Z][a-zA-Z_0-9]*");
        return new IdentifierNode(m);
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
            s += tok.match(".").text();
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

Node * SpinDragon::getNumber()
{
    if (tok.look(allowUnderscore("0-9")+"\\."+allowUnderscore("0-9")))
        return getFloat();

    else if (tok.look("[0-9]"))
        return getDecimal();

    else if (tok.look("\\$"))
        return getHexadecimal();

    else if (tok.look("%%"))
        return getQuaternary();

    else if (tok.look("%"))
        return getBinary();

    else throw ExpectedError("number");
}


Node * SpinDragon::getFactor()
{
    if (tok.look("\\("))
    {
        tok.get("\\(");
        Node * n = new FactorNode(getBitwiseXorExpression());
        tok.get("\\)");
        return n;
    }
    else
    {
        return getPrimaryExpression();
    }
}

Node * SpinDragon::getTerm()
{
    return getExpression(QStringList() 
            << "\\*" 
            << "//" 
            << "/", 
            &SpinDragon::getFactor);
}

Node * SpinDragon::getExpression()
{
    return getExpression(QStringList() 
            << "\\+" 
            << "-", 
            &SpinDragon::getTerm);
}

Node * SpinDragon::getShiftExpression()
{
    return getExpression(QStringList() 
            << "<<" 
            << ">>" 
            << "~>", 
            &SpinDragon::getExpression);
}

Node * SpinDragon::getRelationalExpression()
{
    return getExpression(QStringList() 
            << "<" 
            << ">" 
            << "<=" 
            << ">=", 
            &SpinDragon::getShiftExpression);
}

Node * SpinDragon::getEqualityExpression()
{
    return getExpression(QStringList() 
            << "==" 
            << "!=", 
            &SpinDragon::getRelationalExpression);
}

Node * SpinDragon::getBitwiseAndExpression()
{
    return getExpression(QStringList() 
            << "&", 
            &SpinDragon::getEqualityExpression);
}

Node * SpinDragon::getBitwiseXorExpression()
{
    return getExpression(QStringList() 
            << "\\^", 
            &SpinDragon::getBitwiseAndExpression);
}

Node * SpinDragon::getBitwiseOrExpression()
{
    return getExpression(QStringList() 
            << "\\|", 
            &SpinDragon::getBitwiseXorExpression);
}

Node * SpinDragon::getExpression(QStringList ops, Node * (SpinDragon::*expr)())
{
    ExpressionNode * n = new ExpressionNode((this->*expr)());

    QString optoken = ops.join("|");

    while (tok.look(optoken))
    {
        foreach(QString op, ops)
        {
            if (tok.look(op))
            {
                Match m = tok.match(op);
                tok.eatSpace();
                n->add(op, (this->*expr)());
                break;
            }
        }
    }

    return n;
}

Node * SpinDragon::getPrimaryExpression()
{
    Node * n;

    if (tok.isIdentifier())
        n = getIdentifier();

    else if (tok.isNumber())
        n = getNumber();

    else throw ExpectedError("primary expression");

    tok.eatSpace();

    return n;
}

Node * SpinDragon::getAssignment()
{
    Node * lhs = getIdentifier();
    tok.eatSpace();
    Match m = tok.get("=");
    Node * rhs = getExpression();

    return new BinaryOpNode(lhs, m, rhs);
}

void SpinDragon::getArrayItem()
{
    getIdentifier()->print();
    tok.eatSpace();

    if (tok.look("\\["))
    {
        tok.get("\\[");
        getPrimaryExpression();
        tok.get("\\]");
    }
}

void SpinDragon::getConstantArray()
{
    Node * n = getLiteral();
    n->print();
    printf("\n%i\n",n->value().toInt());

    while (tok.look(","))
    {
        tok.get(",");
        getArrayItem();
    }
}

void SpinDragon::getConstantLine()
{
    getIndent();

    if (tok.isIdentifier())
        getAssignment()->print();

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
            s += tok.match("[_a-zA-Z0-9.-]+").text();
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

    getIdentifier()->print();
    tok.eatSpace();

    if (tok.look("\\["))
        getArrayIndex();

    tok.get(":");
    getObjectString();
}


void SpinDragon::getNewBlock(QString pattern, Block block)
{
    Match m = tok.match(pattern);
    print("BLOCK", m.text());
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
    Match m = tok.match(pattern);
    print("BLOCK", m.text());
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

    if (tok.isEmptyLine())
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

    tok = Buffer(text, QFileInfo(filename).fileName());

    _block = NO_BLOCK;

    qDebug() << paths.listFiles();
    qDebug() << paths.listPaths();

    try
    {
        getProgram();
    }
    catch (Error & e)
    {
        error(tok, e.what());
        return false;
    }

    return true;
}


void SpinDragon::error(Buffer & in, QString text)
{
    error(in.lastMatch(), text);
}

void SpinDragon::error(Match m, QString text)
{
    fprintf(stderr, "%s\n", qPrintable(QString("\n\033[1;31m%1(%2,%3) Error:\033[0m %4")
                .arg(m.file())
                .arg(m.line())
                .arg(m.col())
                .arg(text)
                ));

    fprintf(stderr,"\n%s\n", qPrintable(m.lineText()));
    fprintf(stderr,"%s\033[1;37m^\033[0m\n", qPrintable(QString(m.col(), ' ')));
    fflush(stderr);
}
