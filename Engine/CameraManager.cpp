#include "CameraManager.h"
#include <QtMath>

Canavar::Engine::CameraManager::CameraManager(QObject *parent)
    : Manager(parent)
    , mActiveCamera(nullptr)
{}

Canavar::Engine::CameraManager *Canavar::Engine::CameraManager::instance()
{
    static CameraManager instance;
    return &instance;
}

bool Canavar::Engine::CameraManager::init()
{
    return true;
}

void Canavar::Engine::CameraManager::addCamera(Camera *camera)
{
    mCameras << camera;
}

void Canavar::Engine::CameraManager::removeCamera(Camera *camera)
{
    if (camera)
    {
        if (camera == mActiveCamera)
            mActiveCamera = nullptr;

        mCameras.removeAll(camera);
    }
}

Canavar::Engine::Camera *Canavar::Engine::CameraManager::activeCamera() const
{
    return mActiveCamera;
}

void Canavar::Engine::CameraManager::setActiveCamera(Camera *newActiveCamera)
{
    if (mActiveCamera == newActiveCamera)
        return;

    if (mActiveCamera)
        mActiveCamera->setActive(false);

    if (newActiveCamera)
    {
        newActiveCamera->setActive(true);
        newActiveCamera->setWidth(mWidth);
        newActiveCamera->setHeight(mHeight);
    }

    mActiveCamera = newActiveCamera;
}

void Canavar::Engine::CameraManager::mouseDoubleClicked(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseDoubleClicked(event);
}

void Canavar::Engine::CameraManager::mousePressed(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mousePressed(event);
}

void Canavar::Engine::CameraManager::mouseReleased(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseReleased(event);
}

void Canavar::Engine::CameraManager::mouseMoved(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseMoved(event);
}

void Canavar::Engine::CameraManager::wheelMoved(QWheelEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->wheelMoved(event);
}

void Canavar::Engine::CameraManager::keyPressed(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->keyPressed(event);
}

void Canavar::Engine::CameraManager::keyReleased(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->keyReleased(event);
}

void Canavar::Engine::CameraManager::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    if (mActiveCamera)
    {
        mActiveCamera->setWidth(mWidth);
        mActiveCamera->setHeight(mHeight);
    }
}

void Canavar::Engine::CameraManager::update(float ifps)
{
    if (mActiveCamera)
        mActiveCamera->update(ifps);
}

void Canavar::Engine::CameraManager::reset()
{
    if (mActiveCamera)
        mActiveCamera->reset();
}
