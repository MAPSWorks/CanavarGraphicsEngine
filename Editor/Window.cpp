#include "Window.h"

#include "../Engine/CameraManager.h"
#include "../Engine/Controller.h"
#include "../Engine/LightManager.h"
#include "../Engine/Node.h"
#include "../Engine/NodeManager.h"
#include "../Engine/Sun.h"

#include <QDateTime>
#include <QKeyEvent>

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
        //mPlane = NodeManager::instance()->createNode(Node::NodeType::Model, "Plane");
        mJet = NodeManager::instance()->createNode(Node::NodeType::Model, "f16c");
        mCamera = dynamic_cast<Camera *>(NodeManager::instance()->createNode(Node::NodeType::FreeCamera));
        mSun = dynamic_cast<Sun *>(NodeManager::instance()->createNode(Node::NodeType::Sun));

        NodeManager::instance()->createNode(Node::NodeType::Model, "Nanosuit")->setWorldPosition(QVector3D(0, 10, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Cyborg")->setWorldPosition(QVector3D(0, 30, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Planet")->setWorldPosition(QVector3D(0, 40, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Suzanne")->setWorldPosition(QVector3D(0, 50, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Backpack")->setWorldPosition(QVector3D(0, 60, 0));

        CameraManager::instance()->setActiveCamera(mCamera);
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

    mController->render(ifps);

    QtImGui::newFrame();
    //ImGui::ShowDemoWindow();

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    mController->keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    mController->mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    mController->mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    mController->wheelMoved(event);
}
