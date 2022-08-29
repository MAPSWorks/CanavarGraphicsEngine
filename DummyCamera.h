#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "PerspectiveCamera.h"

#include <QObject>

class DummyCamera : public PerspectiveCamera
{
    Q_OBJECT
private:
    friend class NodeManager;

    explicit DummyCamera(QObject *parent = nullptr);
    virtual ~DummyCamera();

public:
    void onMouseDoubleClicked(QMouseEvent *) override;
    void onMousePressed(QMouseEvent *) override;
    void onMouseReleased(QMouseEvent *) override;
    void onMouseMoved(QMouseEvent *) override;
    void onWheelMoved(QWheelEvent *) override;
    void onKeyPressed(QKeyEvent *) override;
    void onKeyReleased(QKeyEvent *) override;
    void update(float) override;
};

#endif // DUMMYCAMERA_H
