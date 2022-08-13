#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "DummyCamera.h"
#include "FreeCamera.h"
#include "RendererManager.h"
#include "TexturedModel.h"
#include "Window.h"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QApplication *app, QObject *parent = nullptr);
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

    Light *mLight;

    Model *mPlane;
    Model *mSuzanne;
    Model *mCube;

    Qt::MouseButton mPressedButton;

    TexturedModel *mBackpack;
    TexturedModel *mCyborg;
    TexturedModel *mNanoSuit;
    TexturedModel *mPlanet;
    TexturedModel *mRock;
    TexturedModel *mF16;

    QApplication *mApp;

    bool mMouseCaptured;
};

#endif // CONTROLLER_H
