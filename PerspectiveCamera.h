#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "Camera.h"

class PerspectiveCamera : public Camera
{
    Q_OBJECT

protected:
    explicit PerspectiveCamera(QObject *parent = nullptr);
    virtual ~PerspectiveCamera();

public:
    struct Animation {
        PerspectiveCamera *subject = nullptr;
        PerspectiveCamera *activeCameraAfterAnimation;
        QQuaternion finalRotation;
        QVector3D finalPosition;
        float finalVerticalFov;
        QQuaternion startingRotation;
        QVector3D startingPosition;
        float startingVerticalFov;
        bool animating = false;
        bool saveFinalTransformation;
        float duration;
    };

    void setVerticalFov(float newVerticalFov);
    void setHorizontalFov(float newHorizontalFov);

    float verticalFov() const;
    float horizontalFov() const;

    virtual QMatrix4x4 projection() override;
    virtual void drawGUI() override;

    virtual void animate(const Animation &animation);

    const Animation &animation() const;

signals:
    void animationDone(PerspectiveCamera *newActiveCamera);

protected:
    float mVerticalFov;
    float mHorizontalFov;
    Animation mAnimation;
};

#endif // PERSPECTIVECAMERA_H
