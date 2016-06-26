#include "buffer.h"
#include "errors.h"

#include <stdio.h>

#include <QRegularExpression>

Buffer::Buffer(QString text, QString name)
{
    reset();
    _in = text;
    _name = name;
}

Buffer::Buffer(const Buffer & other)
{
    _name       = other._name;
    _index      = other._index;
    _lineindex  = other._lineindex;
    _linenum    = other._linenum;
    _colnum     = other._colnum;

    _in         = other._in;
    _last       = other._last;
    _name       = other._name;
}

Buffer Buffer::operator=(const Buffer & other)
{
    _index      = other._index;;
    _lineindex  = other._lineindex;
    _linenum    = other._linenum;
    _colnum     = other._colnum;

    _in         = other._in;
    _last       = other._last;
    _name       = other._name;

    return *this;
}

Buffer::~Buffer()
{

}

void Buffer::reset()
{
    _index = 0;
    _lineindex = 0;
    _linenum = 1;
    _colnum = 1;

    _in = "";
    _last = Match();
}

void Buffer::eatToken(QString t)
{
    _index += t.size();
    _colnum = _index - _lineindex;
    _last = Match(line(), col(), name(), t, lastLine());
}

bool Buffer::atEnd()
{
    return (_index >= _in.size());
}

Match Buffer::match(QString pattern)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption
                                 | QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m = re.match(_in, _index);

    if (m.hasMatch() && m.capturedStart() == _index)
    {
        eatToken(m.captured());
        return _last;
    }
    else
    {
        throw ExpectedError(pattern);
    }
}

bool Buffer::look(QString pattern)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption
                                 | QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m = re.match(_in, _index);

    return (m.hasMatch() && m.capturedStart() == _index);
}

int Buffer::eatSpace()
{
    match("[ \t]*");
    return _last.text().size();
}

void Buffer::getNewLine()
{
    match("\n");

    _linenum++;
    _lineindex = _index;

    printf(" NL\n");
    fflush(stdout);
}

bool Buffer::isIdentifier()
{
    return look("[_a-zA-Z]");
}

bool Buffer::isNumber()
{
    return look("[0-9]|\\$|%|%%");
}

bool Buffer::isString()
{
    return look("\"");
}

bool Buffer::isEmptyLine()
{
    return look("[ \t]*\n");
}

QString Buffer::last()
{
    return _last.text();
}

QString Buffer::name()
{
    return _name;
}

int Buffer::col()
{
    return _colnum;
}

int Buffer::line()
{
    return _linenum;
}

int Buffer::pos()
{
    return _index;
}

QString Buffer::lastLine()
{
    return _in.mid(_lineindex, _in.indexOf("\n",_lineindex) - _lineindex);
}

Match Buffer::lastMatch()
{
    return _last;
}
