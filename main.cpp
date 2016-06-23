#include "spindragon.h"

#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    QFile file(argv[1]);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Error: File not found:" << argv[1];
        return 1;
    }
    QString text = file.readAll();
    file.close();

    SpinDragon spind;

    bool success = spind.parse(text, argv[1]);

    if (success)
        return 0;
    else
        return 1;
}
