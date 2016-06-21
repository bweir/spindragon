#include "spindragon.h"

#include <QFile>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    QFile file(argv[1]);
    file.open(QFile::ReadOnly);
    QString text = file.readAll();
    file.close();

    SpinDragon spind;

    spind.parse(text);
}
