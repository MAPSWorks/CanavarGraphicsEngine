#include "Window.h"

#include <QDateTime>
#include <QKeyEvent>

#include <QDebug>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

{
    QSurfaceFormat format;
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

    QtImGui::initialize(this);

    emit init();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

    emit resized(w, h);
}

void Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit render(ifps);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    emit keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);
}

void Window::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDoubleClicked(event);
}
