#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLWindow>

#include <imgui.h>
#include <QtImGui.h>

namespace Canavar {
namespace Engine {

class Controller;
class Camera;
class Node;
class NodeManager;
class Sun;
class Model;
class PerspectiveCamera;

} // namespace Engine
} // namespace Canavar

class Window : public QOpenGLWindow, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    Window(QWindow *parent = nullptr);

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

    void drawGui(Canavar::Engine::Node *node);
    void drawGui(Canavar::Engine::Model *model);
    void drawGui(Canavar::Engine::PerspectiveCamera *camera);
    void drawGui(Canavar::Engine::Sun *sun);

private:
    long long mPreviousTime;
    long long mCurrentTime;
    Canavar::Engine::Controller *mController;

    Canavar::Engine::Sun *mSun;
    Canavar::Engine::Camera *mCamera;
    Canavar::Engine::Node *mJet;
    Canavar::Engine::Node *mPlane;

    Canavar::Engine::Node *mSelectedNode;
};
#endif // WINDOW_H
