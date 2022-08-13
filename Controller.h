#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Camera.h"
#include "RendererManager.h"
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
    void onMousePressed(CustomMouseEvent event);
    void onMouseReleased(CustomMouseEvent event);
    void onMouseMoved(CustomMouseEvent event);
    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onResized(int w, int h);
    void onMouseDoubleClicked(CustomMouseEvent event);
    void render(float ifps);

private:
    RendererManager *mRendererManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ModelManager *mModelManager;
    Window *mWindow;

    Camera *mCamera;
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

    QApplication *mApp;

    bool mMouseCaptured;
};

#endif // CONTROLLER_H
