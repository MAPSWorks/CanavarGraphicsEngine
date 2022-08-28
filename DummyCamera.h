#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "Camera.h"

#include <QObject>

class DummyCamera : public Camera
{
    Q_OBJECT
private:
    friend class NodeManager;

    explicit DummyCamera(QObject *parent = nullptr);
    virtual ~DummyCamera();

    // Camera interface
public:
    void onKeyPressed(QKeyEvent *) override;
    void onKeyReleased(QKeyEvent *) override;
    void onMousePressed(QMouseEvent *) override;
    void onMouseReleased(QMouseEvent *) override;
    void onMouseMoved(QMouseEvent *) override;
    void onResized(int, int) override;
    void update(float) override;
};

#endif // DUMMYCAMERA_H
