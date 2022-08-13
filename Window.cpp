#include "Window.h"
#include "qmath.h"

#include <imgui.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSelectedIndex(0)
    , mMouseCaptured(false)

{
    mRendererManager = RendererManager::instance();
    mLightManager = LightManager::instance();
    mModelManager = ModelManager::instance();

    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSamples(16);
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
    emit resized(w, h);
}

void Window::paintGL()
{
    mActiveLight = mLightManager->activeLight();
    mModels = mModelManager->models();

    mSelectedModel = mModels.at(mSelectedIndex);

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit render(ifps);

    mImguiWantCapture = ImGui::GetIO().WantCaptureMouse;

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_MenuBar);

    // Light
    if (!ImGui::CollapsingHeader("Light"))
    {
        {
            ImGui::Text("Position:");
            QVector3D position = mActiveLight->position();
            float x = position.x();
            float y = position.y();
            float z = position.z();
            float r = position.length();
            float theta = qRadiansToDegrees(atan2(z, x));
            float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));
            ImGui::SliderFloat("r", &r, 1.0f, 100.0f, "%.1f");
            ImGui::SliderFloat("theta", &theta, -179.9f, 179.9f, "%.1f");
            ImGui::SliderFloat("phi", &phi, -89.9f, 89.9f, "%.1f");

            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            mActiveLight->setPosition(QVector3D(x, y, z));
        }

        {
            ImGui::Text("Shading Parameters:");
            float ambient = mActiveLight->ambient();
            float diffuse = mActiveLight->diffuse();
            float specular = mActiveLight->specular();

            if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setAmbient(ambient);

            if (ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setDiffuse(diffuse);

            if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setSpecular(specular);

            float color[4] = {mActiveLight->color().x(), mActiveLight->color().y(), mActiveLight->color().z(), mActiveLight->color().w()};

            if (ImGui::ColorEdit4("Color##Light", (float *) &color))
                mActiveLight->setColor(QVector4D(color[0], color[1], color[2], color[3]));
        }
    }

    ImGui::Spacing();

    // Models
    if (!ImGui::CollapsingHeader("Select a model"))
    {
        if (ImGui::BeginCombo("Models", mSelectedModel->objectName().toStdString().c_str()))
        {
            for (int i = 0; i < mModels.size(); ++i)
            {
                if (ImGui::Selectable(mModels.at(i)->objectName().toStdString().c_str()))
                    mSelectedIndex = i;
            }

            ImGui::EndCombo();
        }

        // Position
        {
            ImGui::Text("Position:");
            QVector3D position = mSelectedModel->position();
            float x = position.x();
            float y = position.y();
            float z = position.z();

            if (ImGui::SliderFloat("x##Model", &x, -20.0f, 20.0f, "%.3f"))
                mSelectedModel->setPosition(QVector3D(x, y, z));
            if (ImGui::SliderFloat("y##Model", &y, -20.0f, 20.0f, "%.3f"))
                mSelectedModel->setPosition(QVector3D(x, y, z));
            if (ImGui::SliderFloat("z##Model", &z, -20.0f, 20.0f, "%.3f"))
                mSelectedModel->setPosition(QVector3D(x, y, z));
        }

        // Shading Parameters
        {
            ImGui::Text("Shading Parameters:");
            float ambient = mSelectedModel->material().ambient();
            float diffuse = mSelectedModel->material().diffuse();
            float specular = mSelectedModel->material().specular();

            if (ImGui::SliderFloat("Ambient##Model", &ambient, 0.0f, 1.0f, "%.3f"))
                mSelectedModel->material().setAmbient(ambient);

            if (ImGui::SliderFloat("Diffuse##Model", &diffuse, 0.0f, 1.0f, "%.3f"))
                mSelectedModel->material().setDiffuse(diffuse);

            if (ImGui::SliderFloat("Specular##Model", &specular, 0.0f, 1.0f, "%.3f"))
                mSelectedModel->material().setSpecular(specular);

            float color[4] = {mSelectedModel->material().color().x(), //
                              mSelectedModel->material().color().y(),
                              mSelectedModel->material().color().z(),
                              mSelectedModel->material().color().w()};

            if (ImGui::ColorEdit4("Color##Model", (float *) &color))
                mSelectedModel->material().setColor(QVector4D(color[0], color[1], color[2], color[3]));
        }
    }

    ImGui::Spacing();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
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
    if (event->button() == Qt::LeftButton)
    {
        if (!mImguiWantCapture)
        {
            mMouseCaptured = !mMouseCaptured;
            mMouseGrabPosition = QPoint(event->localPos().x(), event->localPos().y());
            emit mouseCaptured(mMouseCaptured);
        }
    }

    CustomMouseEvent customEvent;
    customEvent.setMouseEvent(event);

    emit mousePressed(customEvent);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    CustomMouseEvent customEvent;
    customEvent.setMouseEvent(event);

    emit mouseReleased(customEvent);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    CustomMouseEvent customEvent;
    customEvent.setMouseEvent(event);
    customEvent.setMouseGrabPosition(mMouseGrabPosition);

    if (mMouseCaptured)
    {
        if (event->pos().x() == mMouseGrabPosition.x() && event->pos().y() == mMouseGrabPosition.y())
        {
            // DO NOT EMIT SIGNAL HERE
        } else
        {
            emit mouseMoved(customEvent);
        }

        cursor().setPos(mapToGlobal(mMouseGrabPosition));
    }
}

void Window::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);
}

void Window::mouseDoubleClickEvent(QMouseEvent *event)
{
    CustomMouseEvent customEvent;
    customEvent.setMouseEvent(event);
    emit mouseDoubleClicked(customEvent);
}

bool Window::imguiWantCapture() const
{
    return mImguiWantCapture;
}
