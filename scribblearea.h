#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QPen>
#include <QBrush>

class ScribbleArea : public QWidget
{
		// Declares our class as a QObject which is the base class
		// for all Qt objects
		// QObjects handle events
		Q_OBJECT

	public:

		enum Valuator
		{
			PressureValuator,
			TangentialPressureValuator,
			TiltValuator ,
			VTiltValuator ,
			HTiltValuator,
			NoValuator
		};
		Q_ENUM(Valuator)

		ScribbleArea();


		// Handles all events
		bool openImage(const QString &fileName);
		bool saveImage(const QString &fileName);
		void setPenColor(const QColor &newColor);
		void setPenWidth(int newWidth);
		void setAlphaChannelValuator(Valuator valType){ alphaChannelValuator = valType; }
		void setColorSaturationValuator(Valuator valType){ colorSaturationValuator = valType; }
		void setLineWidthValuator(Valuator valType){ lineWidthValuator =valType; }
		void setTabletDevice(QTabletEvent *event);
		int maximum(int a, int b){return a>b? a:b;}

		// Has the image been modified since last save
		bool isModified() const { return modified; }
		QColor getColor() const { return myColor; }
		void setColor(QColor val){ myColor = val; }
		int penWidth() const { return myPenWidth; }

	public slots:

		// Events to handle
		void clearImage();
		void print();

	protected:
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;

		void tabletEvent(QTabletEvent* event) override;

		// Updates the scribble area where we are painting
		void paintEvent(QPaintEvent* event) override;

		// Makes sure the area we are drawing on remains
		// as large as the widget
		void resizeEvent(QResizeEvent *event) override;

	private:

		void initPixmap();
		void paintPixmap(QPainter &painter, QTabletEvent* event);
		Qt::BrushStyle brushPattern(qreal value);
		static qreal pressureToWidth(qreal pressure);
		void updateBrush(const QTabletEvent* event);
		void updateCursor(const QTabletEvent* event);

		void drawLineTo(const QPoint &endPoint);
		void resizeImage(QPixmap *givenPixmap, const QSize &newSize);

		// Will be marked true or false depending on if
		// we have saved after a change
		bool modified;

		// Marked true or false depending on if the user
		// is drawing
		bool scribbling;

		// Holds the current pen width & color
		int myPenWidth;
		QColor myColor;
		QImage imageMap;
		QBrush myBrush;
		QPen myPen;
		bool deviceDown;

		// Stores the location at the current mouse event
		QPoint lastPoint;
		Valuator alphaChannelValuator;
		Valuator colorSaturationValuator;
		Valuator lineWidthValuator;

		struct TabletPoint {
				QPointF pos;
				qreal pressure;
				qreal rotation;
		} lastTabletPoint;
};

#endif
