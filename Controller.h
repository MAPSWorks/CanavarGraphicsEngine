#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "DummyCamera.h"
#include "FreeCamera.h"
#include "Model.h"
#include "NodeSelector.h"
#include "RendererManager.h"
#include "Simulator/Aircraft.h"
#include "Simulator/AircraftController.h"

#include <QObject>

#include <imgui.h>
#include <QtImGui.h>

class Window;

class Controller : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    void init();
    void mouseDoubleClicked(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void wheelMoved(QWheelEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int w, int h);
    void render(float ifps);

    void setWindow(Window *newWindow);

private:
    ShaderManager *mShaderManager;
    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    NodeManager *mNodeManager;

    Window *mWindow;
    NodeSelector *mNodeSelector;

    FreeCamera *mFreeCamera;
    DummyCamera *mDummyCamera;

    Qt::MouseButton mPressedButton;

    Node *mBackpack;
    Node *mCyborg;
    Node *mNanoSuit;
    Node *mPlanet;
    Model *mJet;
    Node *mRootJetNode;
    Node *mNozzleParticles;

    QApplication *mApp;

    bool mMouseCaptured;

    Aircraft *mAircraft;
    AircraftController *mAircraftController;
    Aircraft::PrimaryFlightData mPfd;

    bool mImGuiWantsMouseCapture;
    bool mImGuiWantsKeyboardCapture;

    bool mSuccess;

    QVector<Manager *> mManagers;
};

#endif // CONTROLLER_H
