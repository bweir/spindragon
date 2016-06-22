#pragma once

#include <exception>
#include <QString>

class Error : public std::exception
{
public:
    explicit Error(QString message = "");
    virtual ~Error() throw () {}
    const char* what() const throw();

protected:
    QString _message;
};


class ExpectedError : public Error
{
public:
    explicit ExpectedError(QString message);
    virtual ~ExpectedError() throw () {}
};


