#include "Window.h"
#include "qmath.h"

#include <imgui.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSelectedNode(nullptr)
{
    mRendererManager = RendererManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();

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
    mNodes = mNodeManager->nodes();

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

    // Nodes
    if (!ImGui::CollapsingHeader("Select a Node"))
    {
        QString preview = mSelectedNode ? mSelectedNode->name() : "-";
        if (ImGui::BeginCombo("Nodes", preview.toStdString().c_str()))
        {
            for (int i = 0; i < mNodes.size(); ++i)
                populateComboBox(mNodes[i]);

            ImGui::EndCombo();
        }

        // Position
        {
            ImGui::BeginDisabled(mSelectedNode == nullptr);

            ImGui::Text("Position:");
            QVector3D position = mSelectedNode ? mSelectedNode->position() : QVector3D();
            float x = position.x();
            float y = position.y();
            float z = position.z();

            if (ImGui::SliderFloat("x##Node", &x, -20.0f, 20.0f, "%.3f"))
                mSelectedNode->setPosition(QVector3D(x, y, z));
            if (ImGui::SliderFloat("y##Node", &y, -20.0f, 20.0f, "%.3f"))
                mSelectedNode->setPosition(QVector3D(x, y, z));
            if (ImGui::SliderFloat("z##Node", &z, -20.0f, 20.0f, "%.3f"))
                mSelectedNode->setPosition(QVector3D(x, y, z));

            ImGui::EndDisabled();
        }

        // Rotation
        {
            ImGui::BeginDisabled(mSelectedNode == nullptr);

            ImGui::Text("Rotation:");
            QQuaternion rotation = mSelectedNode ? mSelectedNode->rotation() : QQuaternion();
            QVector3D eulerAngles = rotation.toEulerAngles();
            float pitch = eulerAngles.x();
            float yaw = eulerAngles.y();
            float roll = eulerAngles.z();

            if (ImGui::SliderFloat("Pitch##Node", &pitch, -90.0f, 90.0f, "%.3f"))
                mSelectedNode->setRotation(QQuaternion::fromEulerAngles(QVector3D(pitch, yaw, roll)));
            if (ImGui::SliderFloat("Yaw##Node", &yaw, -179.999, 179.999, "%.3f"))
                mSelectedNode->setRotation(QQuaternion::fromEulerAngles(QVector3D(pitch, yaw, roll)));
            if (ImGui::SliderFloat("Roll##Node", &roll, -179.999, 179.999, "%.3f"))
                mSelectedNode->setRotation(QQuaternion::fromEulerAngles(QVector3D(pitch, yaw, roll)));

            ImGui::EndDisabled();
        }

        // Shading Parameters
        //        {

        //            ImGui::Text("Shading Parameters:");
        //            float ambient = mSelectedNode->material().ambient();
        //            float diffuse = mSelectedNode->material().diffuse();
        //            float specular = mSelectedNode->material().specular();

        //            if (ImGui::SliderFloat("Ambient##Model", &ambient, 0.0f, 1.0f, "%.3f"))
        //                mSelectedModel->material().setAmbient(ambient);

        //            if (ImGui::SliderFloat("Diffuse##Model", &diffuse, 0.0f, 1.0f, "%.3f"))
        //                mSelectedModel->material().setDiffuse(diffuse);

        //            if (ImGui::SliderFloat("Specular##Model", &specular, 0.0f, 1.0f, "%.3f"))
        //                mSelectedModel->material().setSpecular(specular);

        //            float color[4] = {mSelectedModel->material().color().x(), //
        //                              mSelectedModel->material().color().y(),
        //                              mSelectedModel->material().color().z(),
        //                              mSelectedModel->material().color().w()};

        //            if (ImGui::ColorEdit4("Color##Model", (float *) &color))
        //                mSelectedModel->material().setColor(QVector4D(color[0], color[1], color[2], color[3]));
        //        }
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

void Window::populateComboBox(Node *node)
{
    if (dynamic_cast<Light *>(node))
        return;

    if (dynamic_cast<Camera *>(node))
        return;

    if (ImGui::Selectable(node->name().toStdString().c_str()))
        mSelectedNode = node;

    for (auto child : node->children())
        populateComboBox(child);
}

bool Window::imguiWantCapture() const
{
    return mImguiWantCapture;
}
