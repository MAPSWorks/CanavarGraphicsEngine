#include "Controller.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "RendererManager.h"
#include "SelectableNodeRenderer.h"
#include "ShaderManager.h"

Canavar::Engine::Controller::Controller(QObject *parent)
    : QObject(parent)
    , mWindow(nullptr)
    , mSuccess(true)
{}

bool Canavar::Engine::Controller::init(const QString &configFile)
{
    initializeOpenGLFunctions();

    mConfig = Config::instance()->instance();
    mConfig->load(configFile);

    mModelDataManager = ModelDataManager::instance();
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
    mRendererManager = RendererManager::instance();
    mSelectableNodeRenderer = SelectableNodeRenderer::instance();

    mManagers << mModelDataManager;
    mManagers << mShaderManager;
    mManagers << mCameraManager;
    mManagers << mLightManager;
    mManagers << mNodeManager;
    mManagers << mSelectableNodeRenderer;
    mManagers << mRendererManager;

    for (const auto &manager : qAsConst(mManagers))
        if (!manager->init())
        {
            mSuccess = false;
            break;
        }

    return mSuccess;
}

void Canavar::Engine::Controller::render(float ifps)
{
    if (!mSuccess)
        return;

    if (!mWindow)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->update(ifps);

    for (const auto &manager : qAsConst(mManagers))
        manager->render(ifps);
}

void Canavar::Engine::Controller::setWindow(QOpenGLWindow *newWindow)
{
    mWindow = newWindow;
}

void Canavar::Engine::Controller::mouseDoubleClicked(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->mouseDoubleClicked(event);
}

void Canavar::Engine::Controller::wheelMoved(QWheelEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->wheelMoved(event);
}

void Canavar::Engine::Controller::mousePressed(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->mousePressed(event);
}

void Canavar::Engine::Controller::mouseReleased(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->mouseReleased(event);
}

void Canavar::Engine::Controller::mouseMoved(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->mouseMoved(event);
}

void Canavar::Engine::Controller::keyPressed(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->keyPressed(event);
}

void Canavar::Engine::Controller::keyReleased(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    for (const auto &manager : qAsConst(mManagers))
        manager->keyReleased(event);
}

void Canavar::Engine::Controller::resize(int w, int h)
{
    if (!mSuccess)
        return;

    mWindow->makeCurrent();

    glViewport(0, 0, w * mWindow->devicePixelRatioF(), h * mWindow->devicePixelRatioF());

    for (const auto &manager : qAsConst(mManagers))
        manager->resize(w * mWindow->devicePixelRatioF(), h * mWindow->devicePixelRatioF());

    mWindow->doneCurrent();
}
