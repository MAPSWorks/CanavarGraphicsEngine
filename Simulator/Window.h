#ifndef WINDOW_H
#define WINDOW_H

#include <FirecrackerEffect.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWindow>

#include <imgui.h>
#include <QtImGui.h>

#include "AircraftController.h"

namespace Canavar {
namespace Engine {

class Controller;
class Node;
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

    Canavar::Engine::FirecrackerEffect *createEfect();

private:
    long long mPreviousTime;
    long long mCurrentTime;
    Canavar::Engine::Controller *mController;

    Aircraft *mAircraft;
    AircraftController *mAircraftController;
    Aircraft::PrimaryFlightData mPfd;

    Canavar::Engine::PerspectiveCamera *mFreeCamera;
    Canavar::Engine::PerspectiveCamera *mDummyCamera;
    Canavar::Engine::Model *mJet;
    Canavar::Engine::Node *mJetRoot;

    bool mSuccess;
};
#endif // WINDOW_H
