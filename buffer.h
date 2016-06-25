#pragma once

#include <QString>

class SpinDragonBuffer
{
    int     _linenum;
    int     _colnum;
    int     _lineindex;
    int     _tokenindex;
    int     _index;

    QString _in;
    QString _last;
    QString _name;

public:
    SpinDragonBuffer(QString text = QString(), QString name = QString());
    SpinDragonBuffer(const SpinDragonBuffer & other);
    SpinDragonBuffer operator=(const SpinDragonBuffer & other);
    ~SpinDragonBuffer();
    void reset();
    void eatToken(QString t);
    int eatSpace();
    QString match(QString pattern);
    bool look(QString pattern);
    bool atEnd();
    void getNewLine();

    QString name();
    int pos();
    int line();
    int col();
    QString last();
    QString lastLine();
};
