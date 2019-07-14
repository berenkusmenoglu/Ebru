#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif

#include "scribblearea.h"

ScribbleArea::ScribbleArea()
	: QWidget(nullptr)
	, myColor(Qt::red)
	, myBrush(myColor)
	, myPen(myBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
	, deviceDown(false)
	, alphaChannelValuator(TangentialPressureValuator)
	, colorSaturationValuator(NoValuator)
	, lineWidthValuator(PressureValuator)
{
	// Roots the widget to the top left even if resized
	setAttribute(Qt::WA_StaticContents);
	resize(500, 500);
	setAutoFillBackground(true);
	setAttribute(Qt::WA_TabletTracking);

	modified = false;
	scribbling = false;
	myPenWidth = 1;
	myColor = Qt::blue;
	clearImage();
}

// Used to load the image and place it in the widget
bool ScribbleArea::openImage(const QString &fileName)
{

	bool success = imageMap.load(fileName);

	if (success) {
		modified = false;
		update();
		return true;
	}
	return false;
}

// Save the current image
bool ScribbleArea::saveImage(const QString &fileName)
{
	return imageMap.save(fileName);
}

// Used to change the pen color
void ScribbleArea::setPenColor(const QColor &newColor)
{
	myColor = newColor;
}

// Used to change the pen width
void ScribbleArea::setPenWidth(int newWidth)
{
	myPenWidth = newWidth;
}

// Color the image area with white
void ScribbleArea::clearImage()
{
	imageMap.fill(Qt::white);
	QImage backgroundImage(":/images/images/watercolorpaper.jpg");
	backgroundImage = backgroundImage.scaled(this->size(), Qt::AspectRatioMode::KeepAspectRatioByExpanding);
	imageMap = backgroundImage;


	for(int i = 0; i < imageMap.width(); i++)
	{
		for(int j = 0; j < imageMap.height();j++)
		{
			if(i == j)
			{
				imageMap.setPixelColor(i,j, Qt::black);
			}
		}
	}

	modified = true;
	update();
}

// If a mouse button is pressed check if it was the
// left button and if so store the current position
// Set that we are currently drawing
void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		lastPoint = event->pos();
		scribbling = true;
	}
}

void ScribbleArea::setTabletDevice(QTabletEvent *event)
{
	updateCursor(event);
}

// When the mouse moves if the left button is clicked
// we call the drawline function which draws a line
// from the last position to the current
void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
	if ((event->buttons() & Qt::LeftButton) && scribbling)
		drawLineTo(event->pos());
}

// If the button is released we set variables to stop drawing
void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && scribbling) {
		drawLineTo(event->pos());
		scribbling = false;
	}
}

void ScribbleArea::tabletEvent(QTabletEvent *event)
{
	switch (event->type()) {
		case QEvent::TabletPress:
			if (!deviceDown) {
				deviceDown = true;
				lastTabletPoint.pos = event->posF();
				lastTabletPoint.pressure = event->pressure();
				lastTabletPoint.rotation = event->rotation();
			}
			break;
		case QEvent::TabletMove:
#ifndef Q_OS_IOS
			if (event->device() == QTabletEvent::RotationStylus)
				updateCursor(event);
#endif
			if (deviceDown) {
				updateBrush(event);
				QPainter painter(&imageMap);
				paintPixmap(painter, event);
				lastTabletPoint.pos = event->posF();
				lastTabletPoint.pressure = event->pressure();
				lastTabletPoint.rotation = event->rotation();
			}
			break;
		case QEvent::TabletRelease:
			if (deviceDown && event->buttons() == Qt::NoButton)
				deviceDown = false;
			update();
			break;
		default:
			break;
	}
	event->accept();
}

qreal ScribbleArea::pressureToWidth(qreal pressure)
{
	return pressure * 10 + 1;
}

void ScribbleArea::updateCursor(const QTabletEvent *event)
{
	QCursor cursor;
	if (event->type() != QEvent::TabletLeaveProximity) {
		if (event->pointerType() == QTabletEvent::Eraser) {
			cursor = QCursor(QPixmap(":/images/cursor-eraser.png"), 3, 28);
		} else {
			switch (event->device()) {
				case QTabletEvent::Stylus:
					cursor = QCursor(QPixmap(":/images/cursor-pencil.png"), 0, 0);
					break;
				case QTabletEvent::Airbrush:
					cursor = QCursor(QPixmap(":/images/cursor-airbrush.png"), 3, 4);
					break;
				case QTabletEvent::RotationStylus: {
					QImage origImg(QLatin1String(":/images/cursor-felt-marker.png"));
					QImage img(32, 32, QImage::Format_ARGB32);
					QColor solid = myColor;
					solid.setAlpha(255);
					img.fill(solid);
					QPainter painter(&img);
					QTransform transform = painter.transform();
					transform.translate(16, 16);
					transform.rotate(event->rotation());
					painter.setTransform(transform);
					painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
					painter.drawImage(-24, -24, origImg);
					painter.setCompositionMode(QPainter::CompositionMode_HardLight);
					painter.drawImage(-24, -24, origImg);
					painter.end();
					cursor = QCursor(QPixmap::fromImage(img), 16, 16);
				} break;
				default:
					break;
			}
		}
	}
	setCursor(cursor);
}

void ScribbleArea::initPixmap()
{
	qreal dpr = devicePixelRatioF();
	QPixmap newPixmap = QPixmap(width() * (int)dpr, height() *(int) dpr);
	newPixmap.setDevicePixelRatio(dpr);
	newPixmap.fill(Qt::white);
	QPainter painter(&newPixmap);
	if (!imageMap.isNull())
	{
		painter.drawPixmap(0, 0, newPixmap);
	}

	painter.end();
	imageMap = newPixmap.toImage();
}

void ScribbleArea::paintPixmap(QPainter &painter, QTabletEvent *event)
{
	static qreal maxPenRadius = pressureToWidth(1.0);
	painter.setRenderHint(QPainter::Antialiasing);

	switch (event->device()) {
		case QTabletEvent::Airbrush:
		{
			painter.setPen(Qt::NoPen);
			QRadialGradient grad(lastTabletPoint.pos, myPen.widthF() * 10.0);
			QColor color = myBrush.color();
			color.setAlphaF(color.alphaF() * 0.25);
			grad.setColorAt(0, myBrush.color());
			grad.setColorAt(0.5, Qt::transparent);
			painter.setBrush(grad);
			qreal radius = grad.radius();
			painter.drawEllipse(event->posF(), radius, radius);
			update(QRect(event->pos() - QPoint(radius, radius), QSize(radius * 2, radius * 2)));
		}
			break;
		case QTabletEvent::RotationStylus:
		{
			myBrush.setStyle(Qt::SolidPattern);
			painter.setPen(Qt::NoPen);
			painter.setBrush(myBrush);
			QPolygonF poly;
			qreal halfWidth = pressureToWidth(lastTabletPoint.pressure);
			QPointF brushAdjust(qSin(qDegreesToRadians(-lastTabletPoint.rotation)) * halfWidth,
						  qCos(qDegreesToRadians(-lastTabletPoint.rotation)) * halfWidth);
			poly << lastTabletPoint.pos + brushAdjust;
			poly << lastTabletPoint.pos - brushAdjust;
			halfWidth = myPen.widthF();
			brushAdjust = QPointF(qSin(qDegreesToRadians(-event->rotation())) * halfWidth,
						    qCos(qDegreesToRadians(-event->rotation())) * halfWidth);
			poly << event->posF() - brushAdjust;
			poly << event->posF() + brushAdjust;
			painter.drawConvexPolygon(poly);
			update(poly.boundingRect().toRect());
		}
			break;
		case QTabletEvent::Puck:
		case QTabletEvent::FourDMouse:
		{
			const QString error(tr("This input device is not supported by the example."));
#if QT_CONFIG(statustip)
			QStatusTipEvent status(error);
			QApplication::sendEvent(this, &status);
#else
			qWarning() << error;
#endif
		}
			break;
		default:
		{
			const QString error(tr("Unknown tablet device - treating as stylus"));
#if QT_CONFIG(statustip)
			QStatusTipEvent status(error);
			QApplication::sendEvent(this, &status);
#else
			qWarning() << error;
#endif
		}
			Q_FALLTHROUGH();
		case QTabletEvent::Stylus:
			painter.setPen(myPen);
			painter.drawLine(lastTabletPoint.pos, event->posF());
			update(QRect(lastTabletPoint.pos.toPoint(), event->pos()).normalized()
				 .adjusted(-maxPenRadius, -maxPenRadius, maxPenRadius, maxPenRadius));
			break;
	}
}


// QPainter provides functions to draw on the widget
// The QPaintEvent is sent to widgets that need to
// update themselves
void ScribbleArea::paintEvent(QPaintEvent *event)
{
	if(imageMap.isNull())
	{
		initPixmap();
	}
	QPainter painter(this);

	QRect pixmapPortion = QRect(event->rect().topLeft() * devicePixelRatioF(),
					    event->rect().size() * devicePixelRatioF());
	QPixmap pixmap = QPixmap::fromImage(imageMap);
	painter.drawPixmap(event->rect().topLeft(), pixmap, pixmapPortion);
	imageMap = pixmap.toImage();
}

void ScribbleArea::updateBrush(const QTabletEvent *event)
{
	int hue, saturation, value, alpha;
	myColor.getHsv(&hue, &saturation, &value, &alpha);

	int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
	int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);

	switch (alphaChannelValuator) {
		case PressureValuator:
			myColor.setAlphaF(event->pressure());
			break;
		case TangentialPressureValuator:
			if (event->device() == QTabletEvent::Airbrush)
				myColor.setAlphaF(qMax(0.01, (event->tangentialPressure() + 1.0) / 2.0));
			else
				myColor.setAlpha(255);
			break;
		case TiltValuator:
			myColor.setAlpha(maximum(abs(vValue - 127), abs(hValue - 127)));
			break;
		default:
			myColor.setAlpha(255);
	}
	switch (colorSaturationValuator) {
		case VTiltValuator:
			myColor.setHsv(hue, vValue, value, alpha);
			break;
		case HTiltValuator:
			myColor.setHsv(hue, hValue, value, alpha);
			break;
		case PressureValuator:
			myColor.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
			break;
		default:
			;
	}
	switch (lineWidthValuator) {
		case PressureValuator:
			myPen.setWidthF(pressureToWidth(event->pressure()));
			break;
		case TiltValuator:
			myPen.setWidthF(maximum(abs(vValue - 127), abs(hValue - 127)) / 12);
			break;
		default:
			myPen.setWidthF(1);
	}
	if (event->pointerType() == QTabletEvent::Eraser) {
		myBrush.setColor(Qt::white);
		myPen.setColor(Qt::white);
		myPen.setWidthF(event->pressure() * 10 + 1);
	} else {
		myBrush.setColor(myColor);
		myPen.setColor(myColor);
	}

}

// Resize the image to slightly larger then the main window
// to cut down on the need to resize the image
void ScribbleArea::resizeEvent(QResizeEvent *event)
{
	if (width() > imageMap.width() || height() > imageMap.height()) {
		int newWidth = qMax(width() + 128, imageMap.width());
		int newHeight = qMax(height() + 128, imageMap.height());
		QPixmap pixmap = QPixmap::fromImage(imageMap);
		resizeImage(&pixmap, QSize(newWidth, newHeight));
		imageMap = pixmap.toImage();
		update();
	}
	QWidget::resizeEvent(event);
}

void ScribbleArea::drawLineTo(const QPoint &endPoint)
{
	// Used to draw on the widget
	QPainter painter(&imageMap);

	// Set the current settings for the pen
	painter.setPen(QPen(myColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
				  Qt::RoundJoin));

	// Draw a line from the last registered point to the current
	painter.drawLine(lastPoint, endPoint);

	// Set that the image hasn't been saved
	modified = true;

	int rad = (myPenWidth / 2) + 2;

	// Call to update the rectangular space where we drew
	update(QRect(lastPoint, endPoint).normalized()
		 .adjusted(-rad, -rad, +rad, +rad));

	// Update the last position where we left off drawing
	lastPoint = endPoint;
}

// When the app is resized create a new image using
// the changes made to the image
void ScribbleArea::resizeImage(QPixmap *givenPixmap, const QSize &newSize)
{
	// Check if we need to redraw the image
	if (givenPixmap->size() == newSize)
		return;

	initPixmap();
}

// Print the image
void ScribbleArea::print()
{
	// Check for print dialog availability
#if QT_CONFIG(printdialog)

	// Can be used to print
	QPrinter printer(QPrinter::HighResolution);

	// Open printer dialog and print if asked
	QPrintDialog printDialog(&printer, this);
	if (printDialog.exec() == QDialog::Accepted) {
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QPixmap pixmap = QPixmap::fromImage(imageMap);
		QSize size = pixmap.size();
		size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(pixmap.rect());
		painter.drawPixmap(0, 0, pixmap);
		imageMap = pixmap.toImage();
	}
#endif // QT_CONFIG(printdialog)
}
