#include "Window.h"

#include <CameraManager.h>
#include <Controller.h>
#include <Gui.h>
#include <Model.h>
#include <Node.h>
#include <NodeManager.h>
#include <PerspectiveCamera.h>

#include <QDateTime>
#include <QKeyEvent>
#include <QVector3D>

#include <QDebug>

using namespace Canavar::Engine;

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

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

    if (mController->init())
    {
        mJet = dynamic_cast<Canavar::Engine::Model *>(NodeManager::instance()->createNode(Node::NodeType::Model, "f16c"));
        mJetRoot = NodeManager::instance()->createNode(Node::NodeType::DummyNode, "JET_ROOT_NODE");
        mJetRoot->addChild(mJet);

        mAircraft = new Aircraft;
        mAircraftController = new AircraftController(mAircraft);

        mAircraftController->init();
        mAircraft->init();

        mAircraftController->setJet(mJet);
        mAircraftController->setRootJetNode(mJetRoot);

        connect(this, &Window::destroyed, this, [=]() {
            qDebug() << thread() << "Controller is being deleted...";
            mAircraft->stop();
        });

        connect(
            mAircraft, &Aircraft::pfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);

        mFreeCamera = dynamic_cast<Canavar::Engine::PerspectiveCamera *>(NodeManager::instance()->createNode(Node::NodeType::FreeCamera));
        mFreeCamera->setPosition(QVector3D(0, 200, 0));
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

        auto effect = NodeManager::instance()->createNode(Node::NodeType::NozzleEffect);
        effect->setPosition(QVector3D(0, 0.14f, 11.5f));
        mJetRoot->addChild(effect);
    }
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

    mController->resize(w, h);
}

void Window::paintGL()
{
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
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    if (event->key() == Qt::Key_1)
        CameraManager::instance()->setActiveCamera(mFreeCamera);
    else if (event->key() == Qt::Key_2)
        CameraManager::instance()->setActiveCamera(mDummyCamera);

    mController->keyPressed(event);
    mAircraftController->keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->keyReleased(event);
    mAircraftController->keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    mController->wheelMoved(event);
}
