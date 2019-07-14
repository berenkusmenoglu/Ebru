#ifndef EBRUAPPLICATION_H
#define EBRUAPPLICATION_H

#include <QMainWindow>
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <scribblearea.h>

class EbruApplication : public QApplication
{
		Q_OBJECT
	public:
		EbruApplication(int &argv, char **args)
		 : QApplication(argv, args) {}

		bool event(QEvent* event) override;
		void setCanvas(ScribbleArea* canvas){myCanvas = canvas;}

	signals:

	public slots:

	private:
		ScribbleArea* myCanvas;
};

#endif // EBRUAPPLICATION_H
