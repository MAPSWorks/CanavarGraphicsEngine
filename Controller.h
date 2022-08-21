#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "DummyCamera.h"
#include "FreeCamera.h"
#include "Model.h"
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

    FreeCamera *mFreeCamera;
    DummyCamera *mDummyCamera;

    DirectionalLight *mSun;
    Qt::MouseButton mPressedButton;

    Node *mPlane;
    Node *mCone;
    Node *mBackpack;
    Node *mCyborg;
    Node *mNanoSuit;
    Node *mPlanet;
    Node *mRock;
    Model *mJet;
    Node *mRootJetNode;

    QApplication *mApp;

    bool mMouseCaptured;

    Aircraft *mAircraft;
    AircraftController *mAircraftController;
    Aircraft::PrimaryFlightData mPfd;

    NozzleEffect *mNozzleEffect;

    bool mImGuiWantCapture;
};

#endif // CONTROLLER_H
