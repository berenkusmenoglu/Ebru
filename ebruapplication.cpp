#include "ebruapplication.h"

#include <QtWidgets>

bool EbruApplication::event(QEvent *event)
{
    if (event->type() == QEvent::TabletEnterProximity ||
	  event->type() == QEvent::TabletLeaveProximity) {
	  myCanvas->setTabletDevice(static_cast<QTabletEvent *>(event));
	  return true;
    }
    return QApplication::event(event);
}
