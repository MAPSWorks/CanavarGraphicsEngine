#include "Controller.h"
#include "FreeCamera.h"
#include "Light.h"

#include <QApplication>
#include <QDebug>

Controller::Controller(QApplication *app, QObject *parent)
    : QObject(parent)
    , mPressedButton(Qt::NoButton)
    , mApp(app)
    , mMouseCaptured(false)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
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

    mFreeCamera = new FreeCamera;
    mFreeCamera->setPosition(QVector3D(0, 10, 10));
    mFreeCamera->setVerticalFov(60.0f);
    mFreeCamera->setZNear(0.1f);
    mFreeCamera->setZFar(10000.0f);
    mCameraManager->addCamera(mFreeCamera);
    mCameraManager->setActiveCamera(mFreeCamera);

    connect(mFreeCamera, &FreeCamera::mouseGrabbed, this, [=](bool grabbed) {
        mMouseCaptured = grabbed;

        if (mMouseCaptured)
            mApp->setOverrideCursor(QCursor(Qt::BlankCursor));
        else
            mApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
    });

    connect(mFreeCamera, &FreeCamera::setCursorPosition, this, [=](QPoint position) { //
        mWindow->cursor().setPos(mWindow->mapToGlobal(position));
    });
}

void Controller::init()
{
    mRendererManager->init();

    mDummyCamera = new DummyCamera;
    mDummyCamera->setPosition(QVector3D(0, 0, 3));
    mDummyCamera->setVerticalFov(80.0f);
    mDummyCamera->setZNear(0.1f);
    mDummyCamera->setZFar(10000.0f);
    mCameraManager->addCamera(mDummyCamera);

    mLight = new Light;
    mLight->setPosition(QVector3D(5, 20, 35));
    mLightManager->addLight(mLight);
    mLightManager->setActiveLight(mLight);

    // Plane
    mPlane = new Model;
    mPlane->setType(Model::Plane);
    mPlane->setName("Plane");
    mPlane->setPosition(QVector3D(0, 0, 0));
    mPlane->setScale(QVector3D(10.0f, 10.0f, 10.0f));
    mNodeManager->addNode(mPlane);

    // Suzanne
    mSuzanne = new Model;
    mSuzanne->setType(Model::Suzanne);
    mSuzanne->setName("Suzanne");
    mSuzanne->setPosition(QVector3D(0, 0, 0));

    mSuzanne->addChild(mDummyCamera);

    // Cube
    mCube = new Model;
    mCube->setType(Model::Cube);
    mCube->setName("Cube");
    mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
    mCube->setPosition(QVector3D(0, 0, 0));
    mNodeManager->addNode(mCube);
    mCube->addChild(mSuzanne);

    mBackpack = new TexturedModel("backpack");
    mBackpack->setPosition(QVector3D(-5, 5, -5));
    mBackpack->setShininess(1.0f);
    mNodeManager->addNode(mBackpack);

    mCyborg = new TexturedModel("cyborg");
    mCyborg->setPosition(QVector3D(-5, 10, -5));
    mNodeManager->addNode(mCyborg);

    mNanoSuit = new TexturedModel("nanosuit");
    mNanoSuit->setPosition(QVector3D(-5, 15, -5));
    mNodeManager->addNode(mNanoSuit);

    mPlanet = new TexturedModel("planet");
    mPlanet->setPosition(QVector3D(15, 5, -5));
    mNodeManager->addNode(mPlanet);

    mRock = new TexturedModel("rock");
    mRock->setPosition(QVector3D(15, 15, -5));
    mNodeManager->addNode(mRock);

    mF16 = new TexturedModel("f16");
    mF16->setPosition(QVector3D(15, 15, 15));
    mNodeManager->addNode(mF16);
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
    if (event->key() == Qt::Key_1)
    {
        mCameraManager->setActiveCamera(mFreeCamera);

        if (mFreeCamera->getMouseGrabbed())
            mApp->setOverrideCursor(QCursor(Qt::BlankCursor));
        else
            mApp->setOverrideCursor(QCursor(Qt::ArrowCursor));

    } else if (event->key() == Qt::Key_2)
    {
        mApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        mCameraManager->setActiveCamera(mDummyCamera);
    } else
    {
        mCameraManager->onKeyPressed(event);
    }
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);
}

void Controller::onResized(int w, int h)
{
    mCameraManager->setAspectRatio((float) (w) / h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *) {}

void Controller::render(float ifps)
{
    mCameraManager->update(ifps);
    mRendererManager->render(ifps);
}
