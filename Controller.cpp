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

    mFreeCamera = dynamic_cast<FreeCamera *>(mNodeManager->create(Node::NodeType::FreeCamera));
    mFreeCamera->setPosition(QVector3D(0, 10, 10));
    mFreeCamera->setVerticalFov(60.0f);
    mFreeCamera->setZNear(0.1f);
    mFreeCamera->setZFar(1000000.0f);
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

    mDummyCamera = dynamic_cast<DummyCamera *>(mNodeManager->create(Node::NodeType::DummyCamera));
    mDummyCamera->setVerticalFov(80.0f);
    mDummyCamera->setZNear(0.1f);
    mDummyCamera->setZFar(1000000.0f);

    mSun = dynamic_cast<DirectionalLight *>(mNodeManager->create(Node::NodeType::DirectionalLight));
    mSun->setDirection(QVector3D(1, -1, 1));

    // Plane
    mPlane = mNodeManager->create(Model::NodeType::Model, "Plane");
    mPlane->setPosition(QVector3D(0, 100, 0));
    mPlane->setScale(QVector3D(1.0f, 1.0f, 1.0f));
    mPlane->setVisible(false);

    mCube = mNodeManager->create(Model::NodeType::Model, "Cube");
    mCube->setVisible(false);

    mBackpack = mNodeManager->create(Model::NodeType::Model, "Backpack");
    mBackpack->setPosition(QVector3D(-5, 5, -5));

    mCyborg = mNodeManager->create(Model::NodeType::Model, "Cyborg");
    mCyborg->setPosition(QVector3D(-5, 10, -5));

    mNanoSuit = mNodeManager->create(Model::NodeType::Model, "Nanosuit");
    mNanoSuit->setPosition(QVector3D(-5, 15, -5));

    mPlanet = mNodeManager->create(Model::NodeType::Model, "Planet");
    mPlanet->setPosition(QVector3D(15, 5, -5));

    mJet = dynamic_cast<Model *>(mNodeManager->create(Model::NodeType::Model, "f16c"));

    mRootJetNode = mNodeManager->create(Model::NodeType::DummyNode, "JET_ROOT_NODE");
    mRootJetNode->addChild(mJet);

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
    mRendererManager->resize(w, h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *) {}

void Controller::render(float ifps)
{
    mRootJetNode->setRotation(mPfd.rotation);
    mRootJetNode->setPosition(mPfd.position);

    // Rudder
    {
        QVector3D p0 = QVector3D(0.0f, 3.0193f, 10.3473f);
        QVector3D p1 = QVector3D(0.0f, 7.742f, 13.4306f);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.rudderPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_40", t2 * t1 * t0);
    }

    // Left elevator
    {
        QVector3D p0 = QVector3D(-2.6f, 0.4204f, 8.4395f);
        QVector3D p1 = QVector3D(-6.8575f, -0.4848f, 11.7923f);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.elevatorPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_36", t2 * t1 * t0);
    }

    // Right elevator
    {
        QVector3D p0 = QVector3D(2.6f, 0.4204f, 8.4395f);
        QVector3D p1 = QVector3D(6.8575f, -0.4848f, 11.7923f);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, mPfd.elevatorPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_18", t2 * t1 * t0);
    }

    // Left aileron
    {
        QVector3D p0 = QVector3D(-2.6074f, 0.3266f, 3.4115f);
        QVector3D p1 = QVector3D(-8.7629f, -0.2083f, 4.333f);
        QVector3D axis = (p0 - p1).normalized();
        QMatrix4x4 t0, t1, t2;

        t0.translate(-p0);
        t1.rotate(QQuaternion::fromAxisAndAngle(axis, -mPfd.leftAileronPos));
        t2.translate(p0);
        mJet->setMeshTransformation("Object_22", t2 * t1 * t0);
    }

    // Right aileron
    {
        QVector3D p0 = QVector3D(2.6072f, 0.3266f, 3.4115f);
        QVector3D p1 = QVector3D(8.7623f, 0.1772f, 4.3218f);
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
