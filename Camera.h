#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

class Camera : public QObject
{
    Q_OBJECT
protected:
    explicit Camera(QObject *parent = nullptr);

signals:
    void activeChanged(bool active);

public:
    virtual const QVector3D &position() const;
    virtual void setPosition(const QVector3D &newPosition);

    virtual const QQuaternion &rotation() const;
    virtual void setRotation(const QQuaternion &newRotation);

    virtual float verticalFov() const;
    virtual void setVerticalFov(float newVerticalFov);

    virtual float horizontalFov() const;
    virtual void setHorizontalFov(float newHorizontalFov);

    virtual float aspectRatio() const;
    virtual void setAspectRatio(float newAspectRatio);

    virtual float zNear() const;
    virtual void setZNear(float newZNear);

    virtual float zFar() const;
    virtual void setZFar(float newZFar);

    virtual const QMatrix4x4 &transformation() const;
    virtual const QMatrix4x4 &projection() const;

    virtual QVector3D getViewDirection();

    virtual void onKeyPressed(QKeyEvent *) = 0;
    virtual void onKeyReleased(QKeyEvent *) = 0;
    virtual void onMousePressed(QMouseEvent *event) = 0;
    virtual void onMouseReleased(QMouseEvent *event) = 0;
    virtual void onMouseMoved(QMouseEvent *event) = 0;
    virtual void update(float) = 0;

    bool active() const;
    void setActive(bool newActive);

private:
    void updateTransformation();
    void updateProjection();

protected:
    QVector3D mPosition;
    QQuaternion mRotation;
    QMatrix4x4 mProjection;
    QMatrix4x4 mTransformation;

    float mVerticalFov;
    float mHorizontalFov;
    float mAspectRatio;
    float mZNear;
    float mZFar;

    bool mActive;
};

#endif // CAMERA_H
