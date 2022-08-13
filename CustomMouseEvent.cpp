#include "CustomMouseEvent.h"

CustomMouseEvent::CustomMouseEvent() {}

QPoint CustomMouseEvent::mouseGrabPosition() const
{
    return mMouseGrabPosition;
}

void CustomMouseEvent::setMouseGrabPosition(QPoint newMouseGrabPosition)
{
    mMouseGrabPosition = newMouseGrabPosition;
}

QMouseEvent *CustomMouseEvent::mouseEvent() const
{
    return mMouseEvent;
}

void CustomMouseEvent::setMouseEvent(QMouseEvent *newMouseEvent)
{
    mMouseEvent = newMouseEvent;
}
