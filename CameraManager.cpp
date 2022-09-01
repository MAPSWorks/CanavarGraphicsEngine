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

void CameraManager::mouseDoubleClicked(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseDoubleClicked(event);
}

void CameraManager::mousePressed(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mousePressed(event);
}

void CameraManager::mouseReleased(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseReleased(event);
}

void CameraManager::mouseMoved(QMouseEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->mouseMoved(event);
}

void CameraManager::wheelMoved(QWheelEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->wheelMoved(event);
}

void CameraManager::keyPressed(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->keyPressed(event);
}

void CameraManager::keyReleased(QKeyEvent *event)
{
    if (mActiveCamera)
        mActiveCamera->keyReleased(event);
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
