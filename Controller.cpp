#include "Controller.h"
#include "FreeCamera.h"
#include "Light.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mPressedButton(Qt::NoButton)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mModelManager = ModelManager::instance();
    mWindow = new Window;

    connect(mWindow, &Window::wheelMoved, this, &Controller::onWheelMoved);
    connect(mWindow, &Window::mousePressed, this, &Controller::onMousePressed);
    connect(mWindow, &Window::mouseReleased, this, &Controller::onMouseReleased);
    connect(mWindow, &Window::mouseMoved, this, &Controller::onMouseMoved);
    connect(mWindow, &Window::keyPressed, this, &Controller::onKeyPressed);
    connect(mWindow, &Window::keyReleased, this, &Controller::onKeyReleased);
    connect(mWindow, &Window::resized, this, &Controller::onResized);
    connect(mWindow, &Window::init, this, &Controller::init);
    connect(mWindow, &Window::render, this, &Controller::render);
    connect(mWindow, &Window::mouseDoubleClicked, this, &Controller::onMouseDoubleClicked);
}

void Controller::init()
{
    mRendererManager->init();

    mCamera = new FreeCamera;
    mCamera->setPosition(QVector3D(0, 10, 10));
    mCamera->setVerticalFov(60.0f);
    mCamera->setZNear(0.1f);
    mCamera->setZFar(10000.0f);
    mCameraManager->addCamera(mCamera);
    mCameraManager->setActiveCamera(mCamera);

    mLight = new Light;
    mLight->setPosition(QVector3D(5, 20, 35));
    mLightManager->addLight(mLight);
    mLightManager->setActiveLight(mLight);

    // Plane
    mPlane = new Model;
    mPlane->setType(Model::Plane);
    mPlane->setObjectName("Plane");
    mPlane->setPosition(QVector3D(0, 0, 0));
    mPlane->setVisible(true);
    mModelManager->addModel(mPlane);

    // Suzanne
    mSuzanne = new Model;
    mSuzanne->setType(Model::Suzanne);
    mSuzanne->setObjectName("Suzanne");
    mSuzanne->setPosition(QVector3D(0, 5, 0));
    mSuzanne->setVisible(true);
    mModelManager->addModel(mSuzanne);

    // Cube
    mCube = new Model;
    mCube->setType(Model::Cube);
    mCube->setObjectName("Cube");
    mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
    mCube->setPosition(QVector3D(2.5, 5, -5));
    mCube->setVisible(true);
    mModelManager->addModel(mCube);

    mBackpack = new TexturedModel("backpack");
    mBackpack->setPosition(QVector3D(-5, 5, -5));
    mModelManager->addTexturedModel(mBackpack);

    mCyborg = new TexturedModel("cyborg");
    mCyborg->setPosition(QVector3D(-5, 10, -5));
    mModelManager->addTexturedModel(mCyborg);

    mNanoSuit = new TexturedModel("nanosuit");
    mNanoSuit->setPosition(QVector3D(-5, 15, -5));
    mModelManager->addTexturedModel(mNanoSuit);

    mPlanet = new TexturedModel("planet");
    mPlanet->setPosition(QVector3D(15, 5, -5));
    mModelManager->addTexturedModel(mPlanet);

    mRock = new TexturedModel("rock");
    mRock->setPosition(QVector3D(15, 15, -5));
    mModelManager->addTexturedModel(mRock);
}

void Controller::run()
{
    // mWindow->showMaximized();
    // mWindow->showFullScreen();
    mWindow->resize(1280, 800);
    mWindow->show();
}

void Controller::onWheelMoved(QWheelEvent *)
{
    if (mWindow->imguiWantCapture())
        return;
}

void Controller::onMousePressed(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    mPressedButton = event->button();

    mCameraManager->onMousePressed(event);
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    mPressedButton = Qt::NoButton;

    mCameraManager->onMouseReleased(event);
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (mWindow->imguiWantCapture())
        return;

    mCameraManager->onMouseMoved(event);
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    mCameraManager->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);
}

void Controller::onResized(int w, int h)
{
    mCamera->setAspectRatio((float) (w) / h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *) {}

void Controller::render(float ifps)
{
    mCameraManager->update(ifps);
    mRendererManager->render(ifps);
}
