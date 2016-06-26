#pragma once

#include <QString>

class Match
{
    int     _line;
    int     _col;

    QString _file;
    QString _text;
    QString _linetext;

public:
    Match();
    Match(int line, int col, QString file, QString text, QString lastline);

    int line();
    int col();
    QString file();
    QString text();
    QString lineText();
};
