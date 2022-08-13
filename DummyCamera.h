#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "Camera.h"

#include <QObject>

class DummyCamera : public Camera
{
    Q_OBJECT
public:
    explicit DummyCamera(QObject *parent = nullptr);
    virtual ~DummyCamera();

    // Camera interface
public:
    void onKeyPressed(QKeyEvent *);
    void onKeyReleased(QKeyEvent *);
    void onMousePressed(CustomMouseEvent);
    void onMouseReleased(CustomMouseEvent);
    void onMouseMoved(CustomMouseEvent);
    void update(float);
};

#endif // DUMMYCAMERA_H
