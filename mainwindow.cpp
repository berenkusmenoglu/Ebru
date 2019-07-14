#include <QtWidgets>

#include "mainwindow.h"
#include "scribblearea.h"

// MainWindow constructor
MainWindow::MainWindow()
	:
	  myCanvas(nullptr),
	  colorDialog(nullptr)
{
	// Create the ScribbleArea widget and make it
	// the central widget
	myCanvas = new ScribbleArea;
	setCentralWidget(myCanvas);

	createMenus();

	// Set the title
	setWindowTitle(tr("Ebru by Beren Kusmenoglu"));
	QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents);
	// Size the app
	resize(500, 500);
}


// Open an about dialog
void MainWindow::about()
{
	// Window title and text to display
	QMessageBox::about(this, tr("About Scribble"),
				 tr("<p>The <b>Scribble</b> example is awesome</p>"));
}


// Create the menubar
void MainWindow::createMenus()
{
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	   fileMenu->addAction(tr("&Open..."), this, &MainWindow::load, QKeySequence::Open);
	   fileMenu->addAction(tr("&Save As..."), this, &MainWindow::save, QKeySequence::SaveAs);
	   fileMenu->addAction(tr("&New"), this, &MainWindow::clear, QKeySequence::New);
	   fileMenu->addAction(tr("E&xit"), this, &MainWindow::close, QKeySequence::Quit);

	   QMenu *brushMenu = menuBar()->addMenu(tr("&Brush"));
	   brushMenu->addAction(tr("&Brush Color..."), this, &MainWindow::setBrushColor, tr("Ctrl+B"));

	   QMenu *tabletMenu = menuBar()->addMenu(tr("&Tablet"));
	   QMenu *lineWidthMenu = tabletMenu->addMenu(tr("&Line Width"));

	   QAction *lineWidthPressureAction = lineWidthMenu->addAction(tr("&Pressure"));
	   lineWidthPressureAction->setData(ScribbleArea::PressureValuator);
	   lineWidthPressureAction->setCheckable(true);
	   lineWidthPressureAction->setChecked(true);

	   QAction *lineWidthTiltAction = lineWidthMenu->addAction(tr("&Tilt"));
	   lineWidthTiltAction->setData(ScribbleArea::TiltValuator);
	   lineWidthTiltAction->setCheckable(true);

	   QAction *lineWidthFixedAction = lineWidthMenu->addAction(tr("&Fixed"));
	   lineWidthFixedAction->setData(ScribbleArea::NoValuator);
	   lineWidthFixedAction->setCheckable(true);

	   QActionGroup *lineWidthGroup = new QActionGroup(this);
	   lineWidthGroup->addAction(lineWidthPressureAction);
	   lineWidthGroup->addAction(lineWidthTiltAction);
	   lineWidthGroup->addAction(lineWidthFixedAction);
	   connect(lineWidthGroup, &QActionGroup::triggered, this,
		     &MainWindow::setLineWidthValuator);

	   QMenu *alphaChannelMenu = tabletMenu->addMenu(tr("&Alpha Channel"));
	   QAction *alphaChannelPressureAction = alphaChannelMenu->addAction(tr("&Pressure"));
	   alphaChannelPressureAction->setData(ScribbleArea::PressureValuator);
	   alphaChannelPressureAction->setCheckable(true);

	   QAction *alphaChannelTangentialPressureAction = alphaChannelMenu->addAction(tr("T&angential Pressure"));
	   alphaChannelTangentialPressureAction->setData(ScribbleArea::TangentialPressureValuator);
	   alphaChannelTangentialPressureAction->setCheckable(true);
	   alphaChannelTangentialPressureAction->setChecked(true);

	   QAction *alphaChannelTiltAction = alphaChannelMenu->addAction(tr("&Tilt"));
	   alphaChannelTiltAction->setData(ScribbleArea::TiltValuator);
	   alphaChannelTiltAction->setCheckable(true);

	   QAction *noAlphaChannelAction = alphaChannelMenu->addAction(tr("No Alpha Channel"));
	   noAlphaChannelAction->setData(ScribbleArea::NoValuator);
	   noAlphaChannelAction->setCheckable(true);

	   QActionGroup *alphaChannelGroup = new QActionGroup(this);
	   alphaChannelGroup->addAction(alphaChannelPressureAction);
	   alphaChannelGroup->addAction(alphaChannelTangentialPressureAction);
	   alphaChannelGroup->addAction(alphaChannelTiltAction);
	   alphaChannelGroup->addAction(noAlphaChannelAction);
	   connect(alphaChannelGroup, &QActionGroup::triggered,
		     this, &MainWindow::setAlphaValuator);

	   QMenu *colorSaturationMenu = tabletMenu->addMenu(tr("&Color Saturation"));

	   QAction *colorSaturationVTiltAction = colorSaturationMenu->addAction(tr("&Vertical Tilt"));
	   colorSaturationVTiltAction->setData(ScribbleArea::VTiltValuator);
	   colorSaturationVTiltAction->setCheckable(true);

	   QAction *colorSaturationHTiltAction = colorSaturationMenu->addAction(tr("&Horizontal Tilt"));
	   colorSaturationHTiltAction->setData(ScribbleArea::HTiltValuator);
	   colorSaturationHTiltAction->setCheckable(true);

	   QAction *colorSaturationPressureAction = colorSaturationMenu->addAction(tr("&Pressure"));
	   colorSaturationPressureAction->setData(ScribbleArea::PressureValuator);
	   colorSaturationPressureAction->setCheckable(true);

	   QAction *noColorSaturationAction = colorSaturationMenu->addAction(tr("&No Color Saturation"));
	   noColorSaturationAction->setData(ScribbleArea::NoValuator);
	   noColorSaturationAction->setCheckable(true);
	   noColorSaturationAction->setChecked(true);

	   QActionGroup *colorSaturationGroup = new QActionGroup(this);
	   colorSaturationGroup->addAction(colorSaturationVTiltAction);
	   colorSaturationGroup->addAction(colorSaturationHTiltAction);
	   colorSaturationGroup->addAction(colorSaturationPressureAction);
	   colorSaturationGroup->addAction(noColorSaturationAction);
	   connect(colorSaturationGroup, &QActionGroup::triggered,
		     this, &MainWindow::setSaturationValuator);

	   QAction *compressAction = tabletMenu->addAction(tr("Co&mpress events"));
	   compressAction->setCheckable(true);
	   connect(compressAction, &QAction::toggled, this, &MainWindow::setEventCompression);

	   QMenu *helpMenu = menuBar()->addMenu("&Help");
	   helpMenu->addAction(tr("A&bout"), this, &MainWindow::about);
	   helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);

}

void MainWindow::setAlphaValuator(QAction *action)
{
	myCanvas->setAlphaChannelValuator(action->data().value<ScribbleArea::Valuator>());
}

void MainWindow::setLineWidthValuator(QAction* action)
{
	myCanvas->setLineWidthValuator(action->data().value<ScribbleArea::Valuator>());

}

void MainWindow::setSaturationValuator(QAction* action)
{
	myCanvas->setColorSaturationValuator(action->data().value<ScribbleArea::Valuator>());

}

void MainWindow::setEventCompression(bool compress)
{
    QCoreApplication::setAttribute(Qt::AA_CompressTabletEvents, compress);
}

void MainWindow::setBrushColor()
{
	if (!colorDialog) {
		colorDialog = new QColorDialog(this);
		colorDialog->setModal(false);
		colorDialog->setCurrentColor(myCanvas->getColor());
		connect(colorDialog, &QColorDialog::colorSelected, myCanvas, &ScribbleArea::setColor);
	}
	colorDialog->setVisible(true);
}

bool MainWindow::save()
{
	QString path = QDir::currentPath() + "/untitled.png";
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Picture"),
									path);
	bool success = myCanvas->saveImage(fileName);
	if (!success)
		QMessageBox::information(this, "Error Saving Picture",
						 "Could not save the image");
	return success;
}

void MainWindow::load()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Picture"),
									QDir::currentPath());

	if (!myCanvas->openImage(fileName))
		QMessageBox::information(this, "Error Opening Picture",
						 "Could not open picture");
}

void MainWindow::clear()
{
	if (QMessageBox::question(this, tr("Save changes"), tr("Do you want to save your changes?"),
					    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
					    QMessageBox::Save) != QMessageBox::Save || save())
	myCanvas->clearImage();
}
