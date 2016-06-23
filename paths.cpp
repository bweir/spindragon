#include "paths.h"
#include "errors.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

SpinDragonPaths::SpinDragonPaths()
    : QObject()
{
    clear();
}

SpinDragonPaths::SpinDragonPaths(const SpinDragonPaths & other)
    : QObject()
{
    _paths = other._paths;
    _files = other._files;
}

SpinDragonPaths::~SpinDragonPaths()
{
}

void SpinDragonPaths::clear()
{
    clearPaths();
    clearFiles();
}

void SpinDragonPaths::clearPaths()
{
    _paths.clear();
}

void SpinDragonPaths::clearFiles()
{
    _files.clear();
}

bool SpinDragonPaths::addPath(QString pathname)
{
    if (QDir(pathname).exists())
    {
        _paths << pathname;
        qDebug() << _paths;
        return true;
    }
    return false;
}

QString SpinDragonPaths::openFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();

    QString text = file.readAll();
    file.close();
    return text;
}

QString SpinDragonPaths::getFileName(QStringList entries, 
        QString name, 
        Qt::CaseSensitivity cs)
{
    QStringList filtered = entries.filter(name, cs);

    if (filtered.isEmpty())
    {
        return QString();
    }
    else 
    {
        if (filtered.size() > 1)
        {
            qDebug() << "Warning: More than one candidate for"
                << name 
                <<"; selecting first file:" 
                << filtered[0];
        }

        return filtered[0];
    }
}

QString SpinDragonPaths::searchDirectory(QString path, QString name)
{
    QStringList entries = QDir(path).entryList();
    QString fn;

    QFileInfo fi(name);
    if (fi.suffix().compare("spin",Qt::CaseInsensitive))
        name = fi.completeBaseName();
    
    if (!(fn = getFileName(entries, name)).isEmpty())
        return fn;
    
    if (!(fn = getFileName(entries, name, Qt::CaseInsensitive)).isEmpty())
        return fn;
    
    name += ".spin";
    
    if (!(fn = getFileName(entries, name)).isEmpty())
        return fn;
    
    if (!(fn = getFileName(entries, name, Qt::CaseInsensitive)).isEmpty())
        return fn;

    return QString();
}

QString SpinDragonPaths::searchLibraries(QString name)
{
    foreach (QString path, _paths)
    {
        QString fn;
        if (!(fn = searchDirectory(path, name)).isEmpty())
            return QDir(path).filePath(fn);
    }

    return QString();
}

QString SpinDragonPaths::findObject(QString name)
{
    if (_files.contains(name))
    {
        qDebug() << "Already open" << name;
        return _files[name];
    }

    QString fn = searchLibraries(name);

    QFileInfo fi(fn);

    if (!fi.exists())
    {
        qDebug() << "File does not exist" << name;
        return QString();
    }

    QString text = openFile(fn);

    _files[name] = text;

    return text;
}

QStringList SpinDragonPaths::listPaths()
{
    return _paths;
}

QStringList SpinDragonPaths::listFiles()
{
    return _files.keys();
}
