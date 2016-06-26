#include "match.h"

Match::Match()
{
    _line = 1;
    _col = 1;
    _file = "";
    _text = "";
    _linetext = "";
}

Match::Match(int line, int col, QString file, QString text, QString linetext)
{
    _line = line;
    _col = col;
    _file = file;
    _text = text;
    _linetext = linetext;
}


int Match::line()
{
    return _line;
}

int Match::col()
{
    return _col;
}

QString Match::file()
{
    return _file;
}

QString Match::text()
{
    return _text;
}

QString Match::lineText()
{
    return _linetext;
}

