#include "Window.h"

#include "../Engine/CameraManager.h"
#include "../Engine/Controller.h"
#include "../Engine/Helper.h"
#include "../Engine/Model.h"
#include "../Engine/Node.h"
#include "../Engine/NodeManager.h"
#include "../Engine/PerspectiveCamera.h"
#include "../Engine/Sky.h"
#include "../Engine/Sun.h"

#include <QDateTime>
#include <QKeyEvent>
#include <QVector3D>

#include <QDebug>

using namespace Canavar::Engine;

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSelectedNode(nullptr)

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
        //NodeManager::instance()->createNode(Node::NodeType::Model, "Plane")->setScale(QVector3D(1, 1, 1));
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

    // Nodes
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nodes");

    auto &nodes = NodeManager::instance()->nodes();

    QString preview = mSelectedNode ? mSelectedNode->name() : "-";
    if (ImGui::BeginCombo("Select a node", preview.toStdString().c_str()))
    {
        for (int i = 0; i < nodes.size(); ++i)
            if (ImGui::Selectable(nodes[i]->name().toStdString().c_str()))
                mSelectedNode = nodes[i];

        ImGui::EndCombo();
    }

    if (mSelectedNode)
    {
        ImGui::Text("Type: %d", mSelectedNode->type());
        ImGui::Text("Parent: 0x%p", mSelectedNode->parent());

        if (mSelectedNode->parent())
            ImGui::Text("Parent Name: %s", mSelectedNode->parent()->name().toStdString().c_str());

        switch (mSelectedNode->type())
        {
        case Canavar::Engine::Node::NodeType::DummyNode:
            drawGui(mSelectedNode);
            break;
        case Canavar::Engine::Node::NodeType::FreeCamera:
        case Canavar::Engine::Node::NodeType::DummyCamera:
            drawGui(dynamic_cast<PerspectiveCamera *>(mSelectedNode));
            drawGui(mSelectedNode);
            break;
        case Canavar::Engine::Node::NodeType::Sky:
            drawGui(dynamic_cast<Sky *>(mSelectedNode));
            break;
        case Canavar::Engine::Node::NodeType::Sun:
            drawGui(dynamic_cast<Sun *>(mSelectedNode));
            break;
        case Canavar::Engine::Node::NodeType::Model:
            drawGui(mSelectedNode);
            drawGui(dynamic_cast<Model *>(mSelectedNode));
            break;
        default:
            break;
        }
    }

    ImGui::End();

    glViewport(0, 0, width(), height());
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

void Window::drawGui(Canavar::Engine::Node *node)
{
    // Position
    if (!ImGui::CollapsingHeader("Position##Node"))
    {
        float x = node->position().x();
        float y = node->position().y();
        float z = node->position().z();

        if (ImGui::DragFloat("x##NodePosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodePosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodePosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
    }

    // Rotation
    if (ImGui::CollapsingHeader("Rotation##Node"))
    {
        float yaw, pitch, roll;

        Canavar::Engine::Helper::getEulerDegrees(node->rotation(), yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##Node"))
    {
        QVector3D position = node->worldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::DragFloat("x##NodeWorldPosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeWorldPosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeWorldPosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (ImGui::CollapsingHeader("World Rotation##Node"))
    {
        QQuaternion rotation = node->worldRotation();
        float yaw, pitch, roll;

        Helper::getEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // Scale
    if (ImGui::CollapsingHeader("Scale##Node"))
    {
        QVector3D scale = node->scale();
        float x = scale.x();
        float y = scale.y();
        float z = scale.z();
        float all = qMin(x, qMin(y, z));

        if (ImGui::DragFloat("All##NodeScale", &all, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(all, all, all));
        if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
    }
}

void Window::drawGui(Canavar::Engine::Model *node)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        float ambient = node->ambient();
        float diffuse = node->diffuse();
        float specular = node->specular();
        float shininess = node->shininess();
        auto color = node->color();

        if (ImGui::SliderFloat("Ambient##Model", &ambient, 0.0f, 1.0f, "%.3f"))
            node->setAmbient(ambient);
        if (ImGui::SliderFloat("Diffuse##Model", &diffuse, 0.0f, 1.0f, "%.3f"))
            node->setDiffuse(diffuse);
        if (ImGui::SliderFloat("Specular##Model", &specular, 0.0f, 1.0f, "%.3f"))
            node->setSpecular(specular);
        if (ImGui::SliderFloat("Shininess##Model", &shininess, 1.0f, 128.0f, "%.3f"))
            node->setShininess(shininess);
        if (ImGui::ColorEdit4("Color##Model", (float *) &color))
            node->setColor(color);
    }
}

void Window::drawGui(Canavar::Engine::Sky *node)
{
    if (!ImGui::CollapsingHeader("Sky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &node->mAlbedo, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &node->mTurbidity, 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &node->mNormalizedSunY, 0.0f, 10.0f, "%.3f");
    }
}

void Window::drawGui(Canavar::Engine::Sun *sun)
{
    if (!ImGui::CollapsingHeader("Sun"))
    {
        ImGui::Text("Direction:");
        float x = sun->direction().x();
        float y = sun->direction().y();
        float z = sun->direction().z();
        float r = sun->direction().length();
        float theta = qRadiansToDegrees(atan2(z, x));
        float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

        if (qFuzzyCompare(abs(phi), 90.0f))
            theta = 0.0f;

        bool updated = false;

        if (ImGui::SliderFloat("Theta##DirectionalLight", &theta, -180.0f, 180.0f, "%.1f"))
            updated = true;

        if (ImGui::SliderFloat("Phi##DirectionalLight", &phi, -90.0f, 90.0f, "%.1f"))
            updated = true;

        if (updated)
        {
            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            sun->setDirection(QVector3D(x, y, z));
        }

        ImGui::Text("Shading Parameters:");
        float ambient = sun->ambient();
        float diffuse = sun->diffuse();
        float specular = sun->specular();
        auto color = sun->color();

        if (ImGui::SliderFloat("Ambient##Sun", &ambient, 0.0f, 1.0f, "%.3f"))
            sun->setAmbient(ambient);
        if (ImGui::SliderFloat("Diffuse##Sun", &diffuse, 0.0f, 1.0f, "%.3f"))
            sun->setDiffuse(diffuse);
        if (ImGui::SliderFloat("Specular##Sun", &specular, 0.0f, 1.0f, "%.3f"))
            sun->setSpecular(specular);
        if (ImGui::ColorEdit4("Color##Sun", (float *) &color))
            sun->setColor(color);
    }
}

void Window::drawGui(Canavar::Engine::PerspectiveCamera *camera)
{
    if (!ImGui::CollapsingHeader("Parameters##PerspectiveCamera"))
    {
        auto fov = camera->verticalFov();
        auto zNear = camera->zNear();
        auto zFar = camera->zFar();

        if (ImGui::SliderFloat("FOV##PerspectiveCamera", &fov, 1.0f, 120.0))
            camera->setVerticalFov(fov);
        if (ImGui::SliderFloat("Z-Near##PerspectiveCamera", &zNear, 0.1f, 100.0f))
            camera->setZNear(zNear);
        if (ImGui::SliderFloat("Z-Far##PerspectiveCamera", &zFar, 1000.0f, 1000000.0f))
            camera->setZFar(zFar);
    }
}
