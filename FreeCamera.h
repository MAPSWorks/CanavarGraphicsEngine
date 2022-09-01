#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "PerspectiveCamera.h"

#include <QObject>

class FreeCamera : public PerspectiveCamera
{
    Q_OBJECT

private:
    friend class NodeManager;
    explicit FreeCamera(QObject *parent = nullptr);
    virtual ~FreeCamera();

    struct Mouse {
        bool pressed = false;
        bool grabbed = false;
        float x = 0.0f;
        float y = 0.0f;
        float dx = 0.0f;
        float dy = 0.0f;
        QPoint grabPosition;
    };

    struct Speed {
        float movement = 5.0f;
        float angular = 25.0f;
        float movementMultiplier = 1.0f;
        float angularMultiplier = 1.0f;
    };

public:
    void mouseDoubleClicked(QMouseEvent *) override;
    void mousePressed(QMouseEvent *event) override;
    void mouseReleased(QMouseEvent *event) override;
    void mouseMoved(QMouseEvent *event) override;
    void keyPressed(QKeyEvent *event) override;
    void keyReleased(QKeyEvent *event) override;
    void wheelMoved(QWheelEvent *) override;
    void update(float ifps) override;
    void reset() override;
    void animate(const Animation &animation) override;
    void drawGUI() override;

    enum class Mode { //
        RotateWhileMouseIsPressing,
        RotateWhileMouseIsMoving
    };

    bool getMouseGrabbed() const;

signals:
    emit void mouseGrabbed(bool grabbed);
    emit void setCursorPosition(QPoint mouseGrabPosition);

private:
    QMap<Qt::Key, bool> mPressedKeys;
    Mode mMode;

    Speed mSpeed;
    Mouse mMouse;

    bool mUpdateRotation;
    bool mUpdatePosition;

    float mTimeElapsed;

    QVector3D mPositionBeforeAnimation;
    QQuaternion mRotationBeforeAnimation;
    float mVerticalFovBeforeAnimation;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};

#endif // FREECAMERA_H
