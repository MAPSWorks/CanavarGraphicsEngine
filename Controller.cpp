#include "Controller.h"
#include "FreeCamera.h"
#include "Helper.h"

#include <QApplication>
#include <QDebug>

Controller::Controller(QApplication *app, QObject *parent)
    : QObject(parent)
    , mPressedButton(Qt::NoButton)
    , mApp(app)
    , mMouseCaptured(false)
    , mImGuiWantsMouseCapture(false)
    , mSuccess(true)
{
    mWindow = new Window;

    connect(mWindow, &Window::wheelMoved, this, &Controller::onWheelMoved);
    connect(mWindow, &Window::mousePressed, this, &Controller::onMousePressed);
    connect(mWindow, &Window::mouseReleased, this, &Controller::onMouseReleased);
    connect(mWindow, &Window::mouseMoved, this, &Controller::onMouseMoved);
    connect(mWindow, &Window::keyPressed, this, &Controller::onKeyPressed);
    connect(mWindow, &Window::keyReleased, this, &Controller::onKeyReleased);
    connect(mWindow, &Window::resized, this, &Controller::resize);
    connect(mWindow, &Window::init, this, &Controller::init);
    connect(mWindow, &Window::render, this, &Controller::render);
    connect(mWindow, &Window::mouseDoubleClicked, this, &Controller::onMouseDoubleClicked);
}

Controller::~Controller()
{
    if (mSuccess)
    {
        qDebug() << thread() << "Controller is being deleted...";
        mAircraft->stop();
    }
}

void Controller::init()
{
    initializeOpenGLFunctions();

    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
    mNodeSelector = NodeSelector::instance();
    mRendererManager = RendererManager::instance();

    mManagers << mShaderManager;
    mManagers << mCameraManager;
    mManagers << mLightManager;
    mManagers << mNodeManager;
    mManagers << mNodeSelector;
    mManagers << mRendererManager;

    for (const auto &manager : qAsConst(mManagers))
    {
        if (!manager->init())
        {
            mSuccess = false;
            return;
        }
    }

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

    mFreeCamera = dynamic_cast<FreeCamera *>(mNodeManager->create(Node::NodeType::FreeCamera));
    mFreeCamera->setPosition(QVector3D(0, 200, 0));
    mFreeCamera->setVerticalFov(60.0f);
    mFreeCamera->setZNear(2.0f);
    mFreeCamera->setZFar(100000.0f);
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

    connect(mFreeCamera, &FreeCamera::animationDone, this, [=](PerspectiveCamera *newActiveCamera) { //
        mCameraManager->setActiveCamera(newActiveCamera);
    });

    connect(mNodeManager, &NodeManager::goToNode, this, [=](Node *node) {
        if (node)
        {
            PerspectiveCamera::Animation animation;
            animation.startingPosition = mCameraManager->activeCamera()->worldPosition();
            animation.startingRotation = mCameraManager->activeCamera()->worldRotation();
            animation.startingVerticalFov = mCameraManager->activeCamera()->verticalFov();

            animation.finalPosition = node->worldPosition() + node->worldRotation() * QVector3D(0, 0, 20);
            float yaw, pitch, roll;
            Helper::getEulerDegrees(node->worldRotation(), yaw, pitch, roll);

            animation.finalRotation = Helper::constructFromEulerDegrees(yaw, pitch, 0);
            animation.finalVerticalFov = mFreeCamera->verticalFov();
            animation.subject = nullptr;
            animation.duration = 1.5f;
            animation.animating = true;
            animation.saveFinalTransformation = true;
            animation.activeCameraAfterAnimation = mFreeCamera;
            mFreeCamera->animate(animation);
            mCameraManager->setActiveCamera(mFreeCamera);
        }
    });

    mDummyCamera = dynamic_cast<DummyCamera *>(mNodeManager->create(Node::NodeType::DummyCamera));

    mRootJetNode->addChild(mDummyCamera);
    mDummyCamera->setPosition(QVector3D(0, 5, 30));
    mDummyCamera->setVerticalFov(75.0f);
    mDummyCamera->setZNear(2.0f);
    mDummyCamera->setZFar(100000.0f);

    mBackpack = mNodeManager->create(Model::NodeType::Model, "Backpack");
    mBackpack->setPosition(QVector3D(0, 500, 0));

    mCyborg = mNodeManager->create(Model::NodeType::Model, "Cyborg");
    mCyborg->setPosition(QVector3D(0, 122, 15));

    mNanoSuit = mNodeManager->create(Model::NodeType::Model, "Nanosuit");
    mNanoSuit->setPosition(QVector3D(15, 122, 0));

    mPlanet = mNodeManager->create(Model::NodeType::Model, "Planet");
    mPlanet->setPosition(QVector3D(15, 122, 15));

    mNozzleParticles = mNodeManager->create(Node::NodeType::NozzleParticles);
    mNozzleParticles->setPosition(QVector3D(0, 0.14f, 11.5f));
    mRootJetNode->addChild(mNozzleParticles);
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
    if (!mSuccess)
        return;

    if (mImGuiWantsMouseCapture)
        return;
}

void Controller::onMousePressed(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    if (mImGuiWantsMouseCapture)
        return;

    mPressedButton = event->button();

    mCameraManager->onMousePressed(event);
    mNodeSelector->onMousePressed(event);
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    mPressedButton = Qt::NoButton;

    mCameraManager->onMouseReleased(event);
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (!mSuccess)
        return;

    if (mImGuiWantsMouseCapture)
        return;

    mCameraManager->onMouseMoved(event);
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    if (mImGuiWantsKeyboardCapture)
        return;

    if (event->key() == Qt::Key_1) // Free Camera
    {
        if (mCameraManager->activeCamera()->animation().animating)
            return;

        if (mCameraManager->activeCamera() != mFreeCamera)
        {
            PerspectiveCamera::Animation animation;
            animation.startingPosition = mCameraManager->activeCamera()->worldPosition();
            animation.startingRotation = mCameraManager->activeCamera()->worldRotation();
            animation.startingVerticalFov = mCameraManager->activeCamera()->verticalFov();

            animation.finalPosition = mFreeCamera->worldPosition();
            animation.finalRotation = mFreeCamera->worldRotation();
            animation.finalVerticalFov = mFreeCamera->verticalFov();

            animation.subject = nullptr;
            animation.duration = 1.5f;
            animation.animating = true;
            animation.saveFinalTransformation = true;
            animation.activeCameraAfterAnimation = mFreeCamera;
            mFreeCamera->animate(animation);
            mCameraManager->setActiveCamera(mFreeCamera);

            if (mFreeCamera->getMouseGrabbed())
                mApp->setOverrideCursor(QCursor(Qt::BlankCursor));
            else
                mApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        }

    } else if (event->key() == Qt::Key_2) // Dummy Camera
    {
        if (mCameraManager->activeCamera()->animation().animating)
            return;

        if (mCameraManager->activeCamera() != mDummyCamera)
        {
            PerspectiveCamera::Animation animation;
            animation.startingPosition = mCameraManager->activeCamera()->worldPosition();
            animation.startingRotation = mCameraManager->activeCamera()->worldRotation();
            animation.startingVerticalFov = mCameraManager->activeCamera()->verticalFov();

            animation.subject = mDummyCamera;

            animation.duration = 1.5f;
            animation.animating = true;
            animation.saveFinalTransformation = false;
            animation.activeCameraAfterAnimation = mDummyCamera;
            mFreeCamera->animate(animation);
            mCameraManager->setActiveCamera(mFreeCamera);

            mApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
        }

    } else
    {
        mCameraManager->onKeyPressed(event);
    }

    mAircraftController->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    if (!mSuccess)
        return;

    mCameraManager->onKeyReleased(event);
    mAircraftController->onKeyReleased(event);
}

void Controller::resize(int w, int h)
{
    if (!mSuccess)
        return;

    mCameraManager->resize(w, h);
    mRendererManager->resize(w, h);
    mNodeSelector->resize(w, h);
}

void Controller::onMouseDoubleClicked(QMouseEvent *)
{
    if (!mSuccess)
        return;
}

void Controller::render(float ifps)
{
    if (!mSuccess)
        return;

    mAircraftController->update(ifps);
    mCameraManager->update(ifps);
    mRendererManager->render(ifps);

    mImGuiWantsMouseCapture = ImGui::GetIO().WantCaptureMouse;
    mImGuiWantsKeyboardCapture = ImGui::GetIO().WantCaptureKeyboard;

    QtImGui::newFrame();
    mRendererManager->drawGUI();
    mNodeManager->drawGUI();
    mAircraftController->drawGUI();

    //ImGui::ShowDemoWindow();

    glViewport(0, 0, mWindow->width(), mWindow->height());
    ImGui::Render();
    QtImGui::render();
}
