#ifndef CAMERA_H
#define CAMERA_H

#include "Node.h"

#include <QKeyEvent>

namespace Canavar {
namespace Engine {

class Camera : public Node
{
    Q_OBJECT
protected:
    explicit Camera(QObject *parent = nullptr);
    virtual ~Camera();

public:
    virtual QMatrix4x4 getViewProjectionMatrix();
    virtual QMatrix4x4 getViewMatrix();
    virtual QMatrix4x4 getRotationMatrix();
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

    int width() const;
    void setWidth(int newWidth);

    int height() const;
    void setHeight(int newHeight);

    float zNear() const;
    void setZNear(float newZNear);

    float zFar() const;
    void setZFar(float newZFar);

signals:
    void activeChanged();

protected:
    bool mActive;
    int mWidth;
    int mHeight;
    float mZNear;
    float mZFar;
};

} // namespace Engine
} // namespace Canavar

#endif // CAMERA_H
