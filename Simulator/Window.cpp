#include "Window.h"

#include <CameraManager.h>
#include <Controller.h>
#include <Gui.h>
#include <Helper.h>
#include <Model.h>
#include <Node.h>
#include <NodeManager.h>
#include <PerspectiveCamera.h>
#include <RendererManager.h>

#include <PersecutorCamera.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QVector3D>

#include <QDebug>

using namespace Canavar::Engine;

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSuccess(false)

{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    QtImGui::initialize(this);

    mController = new Canavar::Engine::Controller;
    mController->setWindow(this);

    mSuccess = mController->init("Resources/Config/Config.json");

    if (mSuccess)
    {
        mJet = dynamic_cast<Canavar::Engine::Model *>(NodeManager::instance()->getNodeByName("Model f16c"));
        mJetRoot = NodeManager::instance()->getNodeByName("JET_ROOT_NODE");
        mJetRoot->addChild(mJet);

        mAircraft = new Aircraft;
        mAircraftController = new AircraftController(mAircraft);

        mAircraftController->setJet(mJet);
        mAircraftController->setRootJetNode(mJetRoot);

        connect(this, &Window::destroyed, this, [=]() {
            qDebug() << thread() << "Controller is being deleted...";
            mAircraft->stop();
        });

        connect(
            mAircraft, &Aircraft::pfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);

        mFreeCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera *>(NodeManager::instance()->getNodeByName("Free Camera"));
        mDummyCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera *>(NodeManager::instance()->getNodeByName("Dummy Camera"));
        mPersecutorCamera = dynamic_cast<Canavar::Engine::PersecutorCamera *>(NodeManager::instance()->getNodeByName("Persecutor Camera"));
        mPersecutorCamera->setTarget(mJetRoot);

        CameraManager::instance()->setActiveCamera(mFreeCamera);
        Canavar::Engine::Sun::instance()->setDirection(QVector3D(1, -0.5, 1));

        mGui = new Canavar::Engine::Gui;
        mAircraftController->init();
        mAircraft->init();
    }
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

    mController->resize(w, h);
}

void Window::paintGL()
{
    if (!mSuccess)
        return;

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mAircraftController->update(ifps);
    mController->render(ifps);

    QtImGui::newFrame();

    mGui->draw();

    mAircraftController->drawGUI();

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    if (event->key() == Qt::Key_1)
        CameraManager::instance()->setActiveCamera(mFreeCamera);
    else if (event->key() == Qt::Key_2)
        CameraManager::instance()->setActiveCamera(mDummyCamera);
    else if (event->key() == Qt::Key_3)
        CameraManager::instance()->setActiveCamera(mPersecutorCamera);

    mController->keyPressed(event);
    mAircraftController->keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    mController->keyReleased(event);
    mAircraftController->keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->mousePressed(event);

    mGui->mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    mController->mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    if (!mSuccess)
        return;

    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->wheelMoved(event);
}

FirecrackerEffect *Window::createEfect()
{
    auto fireCracker = dynamic_cast<FirecrackerEffect *>(NodeManager::instance()->createNode(Node::NodeType::FirecrackerEffect));
    fireCracker->setWorldPosition(QVector3D(Helper::generateBetween(0, 1000), Helper::generateBetween(0, 1000), Helper::generateBetween(0, 1000)));
    fireCracker->setGravity(20);
    fireCracker->setSpanAngle(Helper::generateBetween(150, 170));
    fireCracker->setMaxLife(Helper::generateBetween(10, 15));
    fireCracker->setInitialSpeed(Helper::generateBetween(200, 300));
    fireCracker->setScale(1);

    return fireCracker;
}
