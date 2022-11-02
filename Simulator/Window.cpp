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

    mSuccess = mController->init();

    if (mSuccess)
    {
        mJet = NodeManager::instance()->createModel("f16c");
        mJetRoot = NodeManager::instance()->createNode(Node::NodeType::DummyNode, "JET_ROOT_NODE");
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

        mFreeCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera *>(NodeManager::instance()->createNode(Node::NodeType::FreeCamera));
        mFreeCamera->setPosition(QVector3D(0, 100, 0));
        mFreeCamera->setVerticalFov(60.0f);
        mFreeCamera->setZNear(2.0f);
        mFreeCamera->setZFar(1000000.0f);
        CameraManager::instance()->setActiveCamera(mFreeCamera);

        mDummyCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera *>(NodeManager::instance()->createNode(Node::NodeType::DummyCamera));

        mJetRoot->addChild(mDummyCamera);
        mDummyCamera->setPosition(QVector3D(0, 5, 40));
        mDummyCamera->setVerticalFov(75.0f);
        mDummyCamera->setZNear(5.0f);
        mDummyCamera->setZFar(1000000.0f);

        mPersecutorCamera = dynamic_cast<Canavar::Engine::PersecutorCamera *>(NodeManager::instance()->createNode(Node::NodeType::PersecutorCamera));
        mPersecutorCamera->setZNear(5.0f);
        mPersecutorCamera->setZFar(1000000.0f);
        mPersecutorCamera->setTarget(mJetRoot);

        auto effect = NodeManager::instance()->createNode(Node::NodeType::NozzleEffect);
        effect->setPosition(QVector3D(0, 0.14f, 11.5f));
        mJetRoot->addChild(effect);

        auto redLight = dynamic_cast<Canavar::Engine::PointLight *>(NodeManager::instance()->createNode(Node::NodeType::PointLight));
        redLight->setName("Red Light");
        redLight->setColor(QVector4D(1, 0, 0, 1));
        redLight->setConstant(0.075f);
        redLight->setLinear(0.25f);
        redLight->setQuadratic(0.1f);

        auto redLightContainer = NodeManager::instance()->createModel("Sphere");
        redLightContainer->setColor(QVector4D(1, 0, 0, 1));
        redLightContainer->setScale(QVector3D(0.5, 0.5, 0.5));
        redLightContainer->addChild(redLight);
        redLightContainer->setPosition(QVector3D(-11.42f, 0.16f, -0.83f));
        mJetRoot->addChild(redLightContainer);

        auto greenLight = dynamic_cast<Canavar::Engine::PointLight *>(NodeManager::instance()->createNode(Node::NodeType::PointLight));
        greenLight->setName("Green Light");
        greenLight->setColor(QVector4D(0, 1, 0, 1));
        greenLight->setConstant(0.075f);
        greenLight->setLinear(0.25f);
        greenLight->setQuadratic(0.1f);

        auto greenLightContainer = NodeManager::instance()->createModel("Sphere");
        greenLightContainer->setColor(QVector4D(0, 1, 0, 1));
        greenLightContainer->setScale(QVector3D(0.5, 0.5, 0.5));
        greenLightContainer->addChild(greenLight);
        greenLightContainer->setPosition(QVector3D(11.42f, 0.16f, -0.83f));
        mJetRoot->addChild(greenLightContainer);

        Canavar::Engine::Sun::instance()->setDirection(QVector3D(1, -0.5, 1));

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

    Gui::draw();

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
