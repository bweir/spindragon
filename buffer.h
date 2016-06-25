#pragma once

#include <QString>

class Buffer
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
    Buffer(QString text = QString(), QString name = QString());
    Buffer(const Buffer & other);
    Buffer operator=(const Buffer & other);
    ~Buffer();
    void reset();
    void eatToken(QString t);
    int eatSpace();
    QString match(QString pattern);
    bool look(QString pattern);
    bool atEnd();
    void getNewLine();

    bool isEmptyLine();
    bool isIdentifier();
    bool isNumber();
    bool isString();

    QString name();
    int pos();
    int line();
    int col();
    QString last();
    QString lastLine();
};
