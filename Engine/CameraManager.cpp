#include "CameraManager.h"
#include "NodeManager.h"
#include <QtMath>

Canavar::Engine::CameraManager::CameraManager()
    : Manager()
    , mDefaultCamera(nullptr)
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

void Canavar::Engine::CameraManager::postInit()
{
    mDefaultCamera = dynamic_cast<FreeCamera *>(NodeManager::instance()->createNode(Node::NodeType::FreeCamera, "Default Free Camera"));
    mDefaultCamera->setExcludeFromExport(true);
    setActiveCamera(mDefaultCamera);
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
            setActiveCamera(nullptr);

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

    if (!mActiveCamera)
        setActiveCamera(mDefaultCamera);
}

void Canavar::Engine::CameraManager::mouseDoubleClicked(QMouseEvent *event)
{
    mActiveCamera->mouseDoubleClicked(event);
}

void Canavar::Engine::CameraManager::mousePressed(QMouseEvent *event)
{
    mActiveCamera->mousePressed(event);
}

void Canavar::Engine::CameraManager::mouseReleased(QMouseEvent *event)
{
    mActiveCamera->mouseReleased(event);
}

void Canavar::Engine::CameraManager::mouseMoved(QMouseEvent *event)
{
    mActiveCamera->mouseMoved(event);
}

void Canavar::Engine::CameraManager::wheelMoved(QWheelEvent *event)
{
    mActiveCamera->wheelMoved(event);
}

void Canavar::Engine::CameraManager::keyPressed(QKeyEvent *event)
{
    mActiveCamera->keyPressed(event);
}

void Canavar::Engine::CameraManager::keyReleased(QKeyEvent *event)
{
    mActiveCamera->keyReleased(event);
}

void Canavar::Engine::CameraManager::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    mActiveCamera->setWidth(mWidth);
    mActiveCamera->setHeight(mHeight);
}

void Canavar::Engine::CameraManager::update(float ifps)
{
    mActiveCamera->update(ifps);
}

void Canavar::Engine::CameraManager::reset()
{
    mActiveCamera->reset();
}
