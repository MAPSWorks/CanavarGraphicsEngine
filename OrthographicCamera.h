#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "Camera.h"

#include <QObject>

class OrthographicCamera : public Camera
{
    Q_OBJECT
public:
    explicit OrthographicCamera(QObject *parent = nullptr);

    virtual void mouseDoubleClicked(QMouseEvent *) override;
    virtual void mousePressed(QMouseEvent *event) override;
    virtual void mouseReleased(QMouseEvent *event) override;
    virtual void mouseMoved(QMouseEvent *event) override;
    virtual void wheelMoved(QWheelEvent *event) override;
    virtual void keyPressed(QKeyEvent *) override;
    virtual void keyReleased(QKeyEvent *) override;
    virtual void update(float) override;
    virtual void reset() override;
    virtual void drawGUI() override;
    virtual QMatrix4x4 projection() override;

    float left() const;
    float top() const;
    float zoom() const;

    void setZoom(float newZoom);
    void setLeft(float newLeft);
    void setTop(float newTop);

private:
    struct Mouse {
        bool pressed = false;
        bool grabbed = false;
        float x = 0.0f;
        float y = 0.0f;
        float dx = 0.0f;
        float dy = 0.0f;
        QPoint grabPosition;
    } mMouse;

    float mZoom;
    float mLeft;
    float mTop;

    bool mUpdatePosition;
};

#endif // ORTHOGRAPHICCAMERA_H
