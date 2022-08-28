#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "DummyCamera.h"
#include "FreeCamera.h"
#include "Model.h"
#include "NodeSelector.h"
#include "RendererManager.h"
#include "Simulator/Aircraft.h"
#include "Simulator/AircraftController.h"
#include "Window.h"

#include <QObject>

#include <imgui.h>
#include <QtImGui.h>

class Controller : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Controller(QApplication *app, QObject *parent = nullptr);
    ~Controller();
    void run();

private slots:
    void init();
    void onWheelMoved(QWheelEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onResized(int w, int h);
    void onMouseDoubleClicked(QMouseEvent *event);
    void render(float ifps);

private:
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

    bool mImGuiWantCapture;
    bool mResizeFlag;
};

#endif // CONTROLLER_H
