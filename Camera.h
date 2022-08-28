#ifndef CAMERA_H
#define CAMERA_H

#include "Node.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

class Camera : public Node
{
    Q_OBJECT
protected:
    explicit Camera(QObject *parent = nullptr);
    virtual ~Camera();

signals:
    void activeChanged(bool active);

public:
    float verticalFov() const;
    void setVerticalFov(float newVerticalFov);

    float horizontalFov() const;
    void setHorizontalFov(float newHorizontalFov);

    float aspectRatio() const;
    void setAspectRatio(float newAspectRatio);

    float zNear() const;
    void setZNear(float newZNear);

    float zFar() const;
    void setZFar(float newZFar);

    bool active() const;
    void setActive(bool newActive);

    virtual QMatrix4x4 transformation() const override;
    virtual QMatrix4x4 worldTransformation() const override;
    virtual QMatrix4x4 projection() const;
    virtual QVector3D getViewDirection() const;
    virtual QMatrix4x4 getVP() const;
    const QMatrix4x4 &previousVP() const;
    virtual void updateVP();

    virtual void onKeyPressed(QKeyEvent *) = 0;
    virtual void onKeyReleased(QKeyEvent *) = 0;
    virtual void onMousePressed(QMouseEvent *event) = 0;
    virtual void onMouseReleased(QMouseEvent *event) = 0;
    virtual void onMouseMoved(QMouseEvent *event) = 0;
    virtual void onResized(int, int) = 0;
    virtual void update(float) = 0;

protected:
    float mVerticalFov;
    float mHorizontalFov;
    float mAspectRatio;
    float mZNear;
    float mZFar;
    bool mActive;

    QMatrix4x4 mPreviousVP;
};

#endif // CAMERA_H
