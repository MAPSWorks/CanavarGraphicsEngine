#include "Controller.h"
#include "FreeCamera.h"

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

    mAircraft = new Aircraft;
    mAircraftController = new AircraftController(mAircraft);
    mAircraftController->init();
    mAircraft->init();

    mWindow->setAircraftController(mAircraftController);

    connect(
        mAircraft, &Aircraft::pfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);

    connect(mAircraft, &Aircraft::pfdChanged, mWindow, &Window::onPfdChanged, Qt::QueuedConnection);

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
    connect(mWindow, &Window::command, mAircraft, &Aircraft::onCommand);

    mFreeCamera = new FreeCamera;
    mFreeCamera->setPosition(QVector3D(0, 10, 10));
    mFreeCamera->setVerticalFov(60.0f);
    mFreeCamera->setZNear(0.1f);
    mFreeCamera->setZFar(1000000.0f);
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

Controller::~Controller()
{
    qDebug() << thread() << "Controller is being deleted...";
    mAircraft->stop();
}

void Controller::init()
{
    mRendererManager->init();

    mDummyCamera = new DummyCamera;
    mDummyCamera->setVerticalFov(80.0f);
    mDummyCamera->setZNear(0.1f);
    mDummyCamera->setZFar(1000000.0f);
    mCameraManager->addCamera(mDummyCamera);

    mSun = new DirectionalLight;
    mSun->setDirection(QVector3D(1, -1, 1));
    mLightManager->setDirectionalLight(mSun);

    // Plane
    mPlane = new Model("Plane");
    mPlane->setPosition(QVector3D(0, 100, 0));
    mPlane->setScale(QVector3D(1.0f, 1.0f, 1.0f));
    //mNodeManager->addNode(mPlane);

    mCube = new Model("Cube");
    //mNodeManager->addNode(mCube);

    mBackpack = new Model("Backpack");
    mBackpack->setPosition(QVector3D(-5, 5, -5));
    mNodeManager->addNode(mBackpack);

    mCyborg = new Model("Cyborg");
    mCyborg->setPosition(QVector3D(-5, 10, -5));
    mNodeManager->addNode(mCyborg);

    mNanoSuit = new Model("Nanosuit");
    mNanoSuit->setPosition(QVector3D(-5, 15, -5));
    mNodeManager->addNode(mNanoSuit);

    mPlanet = new Model("Planet");
    mPlanet->setPosition(QVector3D(15, 5, -5));
    mNodeManager->addNode(mPlanet);

    mRock = new Model("Rock");
    mRock->setPosition(QVector3D(15, 15, -5));
    mNodeManager->addNode(mRock);

    mJet = new Model("f16c");

    mRootJetNode = new Node;
    mRootJetNode->setName("JET_ROOT_NODE");
    mRootJetNode->addChild(mJet);
    mNodeManager->addNode(mRootJetNode);

    mDummyCamera->setPosition(QVector3D(0, 5, 30));
    mRootJetNode->addChild(mDummyCamera);
}

void Controller::run()
{
    // mWindow->showMaximized();
    // mWindow->showFullScreen();
    mWindow->resize(1600, 900);
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

    mAircraftController->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);

    mAircraftController->onKeyReleased(event);
}

void Controller::onResized(int w, int h)
{
    mCameraManager->setAspectRatio((float) (w) / h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *) {}

void Controller::render(float ifps)
{
    mRootJetNode->setRotation(mPfd.rotation);
    mRootJetNode->setPosition(mPfd.position);

    // Rudder
    {
        QVector3D p0 = QVector3D(0, 3.0193, 10.3473);
        QVector3D p1 = QVector3D(0, 7.742, 13.4306);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.rudderPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_40", t2 * t1 * t0);
    }

    // Left elevator
    {
        QVector3D p0 = QVector3D(-2.6, 0.4204, 8.4395);
        QVector3D p1 = QVector3D(-6.8575, -0.4848, 11.7923);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.elevatorPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_36", t2 * t1 * t0);
    }

    // Right elevator
    {
        QVector3D p0 = QVector3D(2.6, 0.4204, 8.4395);
        QVector3D p1 = QVector3D(6.8575, -0.4848, 11.7923);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.elevatorPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_18", t2 * t1 * t0);
    }

    // Left aileron
    {
        QVector3D p0 = QVector3D(-2.6074, 0.3266, 3.4115);
        QVector3D p1 = QVector3D(-8.7629, -0.2083, 4.333);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.leftAileronPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_22", t2 * t1 * t0);
    }

    // Right aileron
    {
        QVector3D p0 = QVector3D(2.6072, 0.3266, 3.4115);
        QVector3D p1 = QVector3D(8.7623, 0.1772, 4.3218);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.rightAileronPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_20", t2 * t1 * t0);
    }

    mCameraManager->update(ifps);
    mRendererManager->render(ifps);
}
