#include "Controller.h"
#include "FreeCamera.h"

#include <QApplication>
#include <QDebug>

Controller::Controller(QApplication *app, QObject *parent)
    : QObject(parent)
    , mPressedButton(Qt::NoButton)
    , mApp(app)
    , mMouseCaptured(false)
    , mImGuiWantCapture(false)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
    mWindow = new Window;

    mJet = dynamic_cast<Model *>(mNodeManager->create(Model::NodeType::Model, "f16c"));

    mRootJetNode = mNodeManager->create(Model::NodeType::DummyNode, "JET_ROOT_NODE");
    mRootJetNode->addChild(mJet);

    mAircraft = new Aircraft;
    mAircraftController = new AircraftController(mAircraft);
    mAircraftController->init();
    mAircraft->init();
    mAircraftController->setJet(mJet);
    mAircraftController->setRootJetNode(mRootJetNode);

    connect(
        mAircraft, &Aircraft::pfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);

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
    initializeOpenGLFunctions();

    mRendererManager->init();

    mDummyCamera = dynamic_cast<DummyCamera *>(mNodeManager->create(Node::NodeType::DummyCamera));
    mDummyCamera->setPosition(QVector3D(0, 5, 30));
    mDummyCamera->setVerticalFov(80.0f);
    mDummyCamera->setZNear(0.1f);
    mDummyCamera->setZFar(1000000.0f);
    mRootJetNode->addChild(mDummyCamera);

    mSun = dynamic_cast<DirectionalLight *>(mNodeManager->create(Node::NodeType::DirectionalLight));
    mSun->setDirection(QVector3D(1, -1, 1));

    mPlane = mNodeManager->create(Model::NodeType::Model, "Plane");
    mPlane->setPosition(QVector3D(0, 100, 0));
    mPlane->setScale(QVector3D(1.0f, 1.0f, 1.0f));
    mPlane->setVisible(false);

    mBackpack = mNodeManager->create(Model::NodeType::Model, "Backpack");
    mBackpack->setPosition(QVector3D(-5, 5, -5));

    mCyborg = mNodeManager->create(Model::NodeType::Model, "Cyborg");
    mCyborg->setPosition(QVector3D(-5, 10, -5));

    mNanoSuit = mNodeManager->create(Model::NodeType::Model, "Nanosuit");
    mNanoSuit->setPosition(QVector3D(-5, 15, -5));

    mPlanet = mNodeManager->create(Model::NodeType::Model, "Planet");
    mPlanet->setPosition(QVector3D(15, 5, -5));

    mNozzleEffect = new NozzleEffect;
    mNozzleEffect->setPosition(QVector3D(0, 0.14f, 11.5f));
    mNozzleEffect->create();
    mRootJetNode->addChild(mNozzleEffect);

    mRendererManager->setNozzleEffect(mNozzleEffect);

    mNodeManager->create(Node::NodeType::PointLight);
    mNodeManager->create(Node::NodeType::PointLight);

    mNodeManager->create(Node::NodeType::SpotLight);
    mNodeManager->create(Node::NodeType::SpotLight);
    mNodeManager->create(Node::NodeType::SpotLight);
}

void Controller::run()
{
    //    mWindow->showMaximized();
    //    mWindow->showFullScreen();
    mWindow->resize(1600, 900);
    mWindow->show();
}

void Controller::onWheelMoved(QWheelEvent *)
{
    if (mImGuiWantCapture)
        return;
}

void Controller::onMousePressed(QMouseEvent *event)
{
    if (mImGuiWantCapture)
        return;

    mPressedButton = event->button();

    mCameraManager->onMousePressed(event);
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (mImGuiWantCapture)
        return;

    mPressedButton = Qt::NoButton;

    mCameraManager->onMouseReleased(event);
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (mImGuiWantCapture)
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
    mAircraftController->update(ifps);
    mCameraManager->update(ifps);
    mRendererManager->render(ifps);

    mImGuiWantCapture = ImGui::GetIO().WantCaptureMouse;

    QtImGui::newFrame();
    mRendererManager->drawGui();
    mNodeManager->drawGui();
    mAircraftController->drawGui();

    glViewport(0, 0, mWindow->width(), mWindow->height());
    ImGui::Render();
    QtImGui::render();
}
