#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "Camera.h"

namespace Canavar {
namespace Engine {

class PerspectiveCamera : public Camera
{
    Q_OBJECT
protected:
    explicit PerspectiveCamera(QObject *parent);
    virtual ~PerspectiveCamera();

public:
    virtual QMatrix4x4 getProjectionMatrix() override;

    float verticalFov() const;
    void setVerticalFov(float newVerticalFov);

    float horizontalFov() const;
    void setHorizontalFov(float newHorizontalFov);

protected:
    float mVerticalFov;
    float mHorizontalFov;
};

} // namespace Engine
} // namespace Canavar

#endif // PERSPECTIVECAMERA_H
