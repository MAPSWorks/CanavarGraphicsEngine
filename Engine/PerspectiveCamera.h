#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "Camera.h"

namespace Canavar {
namespace Engine {

class PerspectiveCamera : public Camera
{
    Q_OBJECT
protected:
    PerspectiveCamera();

public:
    virtual QMatrix4x4 getProjectionMatrix() override;

    const float &getVerticalFov() const;
    float &getVerticalFov_nonConst();
    void setVerticalFov(float newVerticalFov);

    const float &getHorizontalFov() const;
    float &getHorizontalFov_nonConst();
    void setHorizontalFov(float newHorizontalFov);

    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

protected:
    float mVerticalFov;
    float mHorizontalFov;
};

} // namespace Engine
} // namespace Canavar

#endif // PERSPECTIVECAMERA_H
