#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "Camera.h"

#include <QObject>

class FreeCamera : public Camera
{
    Q_OBJECT

public:
    explicit FreeCamera(QObject *parent = nullptr);

    void onKeyPressed(QKeyEvent *event) override;
    void onKeyReleased(QKeyEvent *event) override;
    void onMousePressed(CustomMouseEvent event) override;
    void onMouseReleased(CustomMouseEvent event) override;
    void onMouseMoved(CustomMouseEvent event) override;
    void update(float ifps) override;

    enum class Mode { //
        RotateWhileMouseIsPressing,
        RotateWhileMouseIsMoving
    };

private:
    QMap<Qt::Key, bool> mPressedKeys;

    float mMovementSpeed;
    float mAngularSpeed;

    bool mMousePressed;
    float mMousePreviousX;
    float mMousePreviousY;
    float mMouseDeltaX;
    float mMouseDeltaY;

    bool mUpdateRotation;
    bool mUpdatePosition;

    Mode mMode;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};

#endif // FREECAMERA_H
