#include "mainwindow.h"
#include <QApplication>
#include <scribblearea.h>
#include <ebruapplication.h>

int main(int argc, char *argv[])
{
    EbruApplication app(argc, argv);
    ScribbleArea* canvas = new ScribbleArea();
    app.setCanvas(canvas);
    MainWindow w;
    w.resize(500, 500);
    w.show();

    return app.exec();
}
