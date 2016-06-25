#include "spindragon.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "paths.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);

    SpinDragonPaths paths;

    QString filename = argv[1];
    QString text = paths.openFile(filename);
    if (text.isEmpty())
    {
        qDebug() << "Error: File not found:" << filename;
        return 1;
    }

    paths.addPath(QFileInfo(filename).path());
    paths.addPath("/home/brett/Projects/lamestation-sdk/library");
    paths.addPath("/home/brett/Projects/spin-standard-library/library");

//    paths.findObject("obj");
//    paths.findObject("obj.spin");
//    paths.findObject("OBJ.spin.spin");
//    paths.findObject("OBJ.spin");
//    paths.findObject("OBJ");
//
//    paths.findObject("media/fonts/8x8/gfx_font8x8");
//    paths.findObject("media.fonts.8x8.gfx_font8x8");
//    paths.findObject("media.fonts.8x8.gfx_font8x8.spin");
//
//    paths.findObject("LameLCD.spin");
//
//    paths.findObject("LameLCD");
//    paths.findObject("lamelcd");

    SpinDragon spind;

    bool success = spind.parse(text, filename, paths);

    if (success)
        return 0;
    else
        return 1;
}
