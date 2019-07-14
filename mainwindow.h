#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QColorDialog>

// ScribbleArea used to paint the image
class ScribbleArea;

class MainWindow : public QMainWindow
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    MainWindow();

// The events that can be triggered
private slots:
    void setBrushColor();
    void setAlphaValuator(QAction *action);
    void setLineWidthValuator(QAction *action);
    void setSaturationValuator(QAction *action);
    void setEventCompression(bool compress);
    bool save();
    void load();
    void clear();
    void about();

private:

    void createMenus();

    // What we'll draw on
    ScribbleArea *myCanvas;
    QColorDialog* colorDialog;

};

#endif
