#ifndef CUSTOMMOUSEEVENT_H
#define CUSTOMMOUSEEVENT_H

#include <QMouseEvent>
#include <QObject>

class CustomMouseEvent
{
public:
    CustomMouseEvent();

    QPoint mouseGrabPosition() const;
    void setMouseGrabPosition(QPoint newMouseGrabPosition);

    QMouseEvent *mouseEvent() const;
    void setMouseEvent(QMouseEvent *newMouseEvent);

private:
    QMouseEvent *mMouseEvent;
    QPoint mMouseGrabPosition;
};

#endif // CUSTOMMOUSEEVENT_H
