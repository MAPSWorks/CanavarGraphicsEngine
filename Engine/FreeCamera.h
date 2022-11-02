#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Mouse.h"
#include "PerspectiveCamera.h"
#include "Speed.h"

namespace Canavar {
namespace Engine {

class FreeCamera : public PerspectiveCamera
{
    Q_OBJECT
protected:
    friend class NodeManager;
    FreeCamera(const QString &uuid = QString());

public:
    void mouseDoubleClicked(QMouseEvent *) override;
    void wheelMoved(QWheelEvent *) override;
    void mousePressed(QMouseEvent *event) override;
    void mouseReleased(QMouseEvent *event) override;
    void mouseMoved(QMouseEvent *event) override;
    void keyPressed(QKeyEvent *event) override;
    void keyReleased(QKeyEvent *event) override;
    void update(float ifps) override;
    void reset() override;

private:
    QMap<Qt::Key, bool> mPressedKeys;

    Mouse mMouse;
    Speed mSpeed;
    bool mUpdateRotation;
    bool mUpdatePosition;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};
} // namespace Engine
} // namespace Canavar

#endif // FREECAMERA_H
