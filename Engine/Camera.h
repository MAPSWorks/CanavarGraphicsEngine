#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"
#include "Node.h"

#include <QKeyEvent>

namespace Canavar {
namespace Engine {

class Camera : public Node
{
    Q_OBJECT
protected:
    Camera();

public:
    virtual QMatrix4x4 getViewProjectionMatrix();
    virtual QMatrix4x4 getViewMatrix();
    virtual QMatrix4x4 getRotationMatrix();
    virtual QVector3D getViewDirection();
    virtual QMatrix4x4 getProjectionMatrix() = 0;

    virtual void mouseDoubleClicked(QMouseEvent *) = 0;
    virtual void mousePressed(QMouseEvent *) = 0;
    virtual void mouseReleased(QMouseEvent *) = 0;
    virtual void mouseMoved(QMouseEvent *) = 0;
    virtual void wheelMoved(QWheelEvent *) = 0;
    virtual void keyPressed(QKeyEvent *) = 0;
    virtual void keyReleased(QKeyEvent *) = 0;
    virtual void update(float) = 0;
    virtual void reset() = 0;

    virtual void resize(int width, int height);

    bool active() const;
    void setActive(bool newActive);

    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

signals:
    void activeChanged();

protected:
    bool mActive;

    DEFINE_MEMBER(int, Width)
    DEFINE_MEMBER(int, Height)
    DEFINE_MEMBER(float, ZNear)
    DEFINE_MEMBER(float, ZFar)
};

} // namespace Engine
} // namespace Canavar

#endif // CAMERA_H
