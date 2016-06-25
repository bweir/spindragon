#include "buffer.h"
#include "errors.h"

#include <stdio.h>

#include <QRegularExpression>

SpinDragonBuffer::SpinDragonBuffer(QString text, QString name)
{
    reset();
    _in = text;
    _name = name;
}

SpinDragonBuffer::SpinDragonBuffer(const SpinDragonBuffer & other)
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

SpinDragonBuffer SpinDragonBuffer::operator=(const SpinDragonBuffer & other)
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

SpinDragonBuffer::~SpinDragonBuffer()
{

}

void SpinDragonBuffer::reset()
{
    _index = 0;
    _lineindex = 0;
    _linenum = 1;
    _colnum = 1;

    _in = "";
    _last = "";
}

void SpinDragonBuffer::eatToken(QString t)
{
    _last = t;
    _index += t.size();
    _colnum = _index - _lineindex;
}

bool SpinDragonBuffer::atEnd()
{
    return (_index >= _in.size());
}

QString SpinDragonBuffer::match(QString pattern)
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

bool SpinDragonBuffer::look(QString pattern)
{
    QRegularExpression re(pattern, QRegularExpression::DotMatchesEverythingOption
                                 | QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m = re.match(_in, _index);

    return (m.hasMatch() && m.capturedStart() == _index);
}

int SpinDragonBuffer::eatSpace()
{
    match("[ \t]*");
    return _last.size();
}

void SpinDragonBuffer::getNewLine()
{
    match("\n");

    _linenum++;
    _lineindex = _index;

    printf(" NL\n");
    fflush(stdout);
}

QString SpinDragonBuffer::last()
{
    return _last;
}

QString SpinDragonBuffer::name()
{
    return _name;
}

int SpinDragonBuffer::col()
{
    return _colnum;
}

int SpinDragonBuffer::line()
{
    return _linenum;
}

int SpinDragonBuffer::pos()
{
    return _index;
}

QString SpinDragonBuffer::lastLine()
{
    return _in.mid(_lineindex, _in.indexOf("\n",_lineindex) - _lineindex);
}
