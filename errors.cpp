#include "errors.h"

const char* Error::what() const throw()
{
    return strdup(qPrintable(_message));
}

Error::Error(QString message)
{
    _message = message;
}

ExpectedError::ExpectedError(QString message)
    :   Error("Expected: " + message)
{
}

