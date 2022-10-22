#include "PerspectiveCamera.h"
#include "Helper.h"

Canavar::Engine::PerspectiveCamera::PerspectiveCamera(QObject *parent)
    : Camera(parent)
{
    setVerticalFov(60);
}

Canavar::Engine::PerspectiveCamera::~PerspectiveCamera()
{
    // TODO
}

QMatrix4x4 Canavar::Engine::PerspectiveCamera::getProjectionMatrix()
{
    QMatrix4x4 projection;
    projection.perspective(mVerticalFov, float(mWidth) / float(mHeight), mZNear, mZFar);
    return projection;
}

const float &Canavar::Engine::PerspectiveCamera::getHorizontalFov() const
{
    return mHorizontalFov;
}

float &Canavar::Engine::PerspectiveCamera::getHorizontalFov_nonConst()
{
    return mHorizontalFov;
}

void Canavar::Engine::PerspectiveCamera::setHorizontalFov(float newHorizontalFov)
{
    mHorizontalFov = newHorizontalFov;
    mVerticalFov = Helper::calculateVerticalFovForGivenHorizontalFov(mHorizontalFov, mWidth, mHeight);
}

const float &Canavar::Engine::PerspectiveCamera::getVerticalFov() const
{
    return mVerticalFov;
}

float &Canavar::Engine::PerspectiveCamera::getVerticalFov_nonConst()
{
    return mVerticalFov;
}

void Canavar::Engine::PerspectiveCamera::setVerticalFov(float newVerticalFov)
{
    mVerticalFov = newVerticalFov;
    mHorizontalFov = Helper::calculateHorizontalFovForGivenVerticalFov(mVerticalFov, mWidth, mHeight);
}
