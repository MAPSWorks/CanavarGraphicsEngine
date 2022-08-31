#include "CameraManager.h"
#include <QtMath>

CameraManager::CameraManager(QObject *parent)
    : Manager(parent)
    , mActiveCamera(nullptr)
{}

void CameraManager::addCamera(Camera *camera)
{
    mCameras << camera;
}

void CameraManager::removeCamera(Camera *camera)
{
    if (camera)
    {
        if (camera == mActiveCamera)
            mActiveCamera = nullptr;

        mCameras.removeAll(camera);
    }
}

bool CameraManager::init()
{
    return true;
}

PerspectiveCamera *CameraManager::activeCamera() const
{
    return mActiveCamera;
}

void CameraManager::setActiveCamera(PerspectiveCamera *newActiveCamera)
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

void CameraManager::onMouseDoubleClicked(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMouseDoubleClicked(event);
}

void CameraManager::onMousePressed(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMousePressed(event);
}

void CameraManager::onMouseReleased(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMouseReleased(event);
}

void CameraManager::onMouseMoved(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onMouseMoved(event);
}

void CameraManager::onWheelMoved(QWheelEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onWheelMoved(event);
}

void CameraManager::onKeyPressed(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onKeyPressed(event);
}

void CameraManager::onKeyReleased(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->onKeyReleased(event);
}

void CameraManager::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    if (mActiveCamera)
    {
        mActiveCamera->setWidth(mWidth);
        mActiveCamera->setHeight(mHeight);
    }
}

void CameraManager::update(float ifps)
{
    if (mActiveCamera)
        mActiveCamera->update(ifps);
}

void CameraManager::reset()
{
    if (mActiveCamera)
        mActiveCamera->reset();
}

CameraManager *CameraManager::instance()
{
    static CameraManager instance;
    return &instance;
}
