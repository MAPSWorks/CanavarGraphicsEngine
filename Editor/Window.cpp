#include "Window.h"

#include <Controller.h>
#include <Gui.h>
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
    format.setSamples(1);
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
        NodeManager::instance()->createNode(Node::NodeType::Model, "f16c")->setWorldPosition(QVector3D(0, 100, 0));

        NodeManager::instance()->createNode(Node::NodeType::Model, "Nanosuit")->setWorldPosition(QVector3D(0, 10, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Cyborg")->setWorldPosition(QVector3D(0, 30, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Planet")->setWorldPosition(QVector3D(0, 40, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Suzanne")->setWorldPosition(QVector3D(0, 50, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Backpack")->setWorldPosition(QVector3D(0, 60, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Backpack")->setWorldPosition(QVector3D(0, 70, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Backpack")->setWorldPosition(QVector3D(0, 80, 0));
        NodeManager::instance()->createNode(Node::NodeType::Model, "Backpack")->setWorldPosition(QVector3D(0, 90, 0));

        mCamera = dynamic_cast<PerspectiveCamera *>(NodeManager::instance()->createNode(Node::NodeType::FreeCamera));
        CameraManager::instance()->setActiveCamera(mCamera);

        mCamera->setWorldPosition(QVector3D(0, 5, 0));

        auto *pointLight = dynamic_cast<PointLight *>(NodeManager::instance()->createNode(Node::NodeType::PointLight));
        pointLight->setRenderable(true);
        pointLight->setColor(QVector4D(1, 1, 1, 0.5));
    }
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, w * devicePixelRatioF(), h * devicePixelRatioF());

    mController->resize(w * devicePixelRatioF(), h * devicePixelRatioF());
}

void Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mController->render(ifps);

    QtImGui::newFrame();

    Gui::draw();

    glViewport(0, 0, width() * devicePixelRatioF(), height() * devicePixelRatioF());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    mController->keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->keyReleased(event);
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
