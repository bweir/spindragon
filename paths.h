#pragma once

#include <QObject>
#include <QStringList>
#include <QHash>

class SpinDragonPaths : public QObject
{
    Q_OBJECT

    QStringList _paths;
    QHash<QString, QString> _files;

    QString getFileName(QStringList entries, 
            QString name, 
            Qt::CaseSensitivity cs = Qt::CaseSensitive);

    QString searchDirectory(QString path, QString name);
    QString searchSubdirectories(QString path, QString name);
    QString searchLibraries(QString filename);

public:
    explicit SpinDragonPaths();
    SpinDragonPaths(const SpinDragonPaths & other);
    ~SpinDragonPaths();

    void clear();
    void clearFiles();
    void clearPaths();
    bool setFile(QString filename);
    bool addPath(QString pathname);
    QString findObject(QString filename);
    QString openFile(QString filename);

    QStringList listPaths();
    QStringList listFiles();

};
