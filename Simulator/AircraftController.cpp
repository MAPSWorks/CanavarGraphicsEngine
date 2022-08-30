#include "AircraftController.h"

AircraftController::AircraftController(Aircraft *aircraft, QObject *parent)
    : QObject(parent)
    , mAircraft(aircraft)
    , mAileron(0)
    , mElevator(0)
    , mRudder(0)
    , mThrottle(0)
{
    connect(this, &AircraftController::command, mAircraft, &Aircraft::onCommand, Qt::QueuedConnection);
    connect(
        mAircraft, &Aircraft::pfdChanged, this, [=](Aircraft::PrimaryFlightData pfd) { mPfd = pfd; }, Qt::QueuedConnection);
    connect(&mTimer, &QTimer::timeout, this, &AircraftController::tick);
}

void AircraftController::onKeyPressed(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), true);
}

void AircraftController::onKeyReleased(QKeyEvent *event)
{
    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void AircraftController::init()
{
    emit command(Aircraft::Command::Hold);

    mTimer.start(10);
}

void AircraftController::tick()
{
    if (mPressedKeys.value(Qt::Key_Up))
        mElevator += 0.025;
    else if (mPressedKeys.value(Qt::Key_Down))
        mElevator -= 0.025;
    else if (mElevator < -0.025)
        mElevator += 0.025;
    else if (mElevator > 0.025)
        mElevator -= 0.025;
    else
        mElevator = 0.0;

    if (mPressedKeys.value(Qt::Key_Left))
        mAileron -= 0.025;
    else if (mPressedKeys.value(Qt::Key_Right))
        mAileron += 0.025;
    else if (mAileron < -0.025)
        mAileron += 0.025;
    else if (mAileron > 0.025)
        mAileron -= 0.025;
    else
        mAileron = 0.0;

    if (mPressedKeys.value(Qt::Key_Z))
        mRudder += 0.025;
    else if (mPressedKeys.value(Qt::Key_C))
        mRudder -= 0.025;
    else if (mRudder < -0.025)
        mRudder += 0.025;
    else if (mRudder > 0.025)
        mRudder -= 0.025;
    else
        mRudder = 0.0;

    if (mPressedKeys.value(Qt::Key_Plus))
        mThrottle += 0.01;

    if (mPressedKeys.value(Qt::Key_Minus))
        mThrottle -= 0.01;

    mElevator = qBound(-1.0f, mElevator, 1.0f);
    mAileron = qBound(-1.0f, mAileron, 1.0f);
    mRudder = qBound(-1.0f, mRudder, 1.0f);
    mThrottle = qBound(0.0f, mThrottle, 1.0f);

    emit command(Aircraft::Command::Elevator, mElevator);
    emit command(Aircraft::Command::Aileron, mAileron);
    emit command(Aircraft::Command::Rudder, mRudder);
    emit command(Aircraft::Command::Throttle, mThrottle);
}

void AircraftController::update(float)
{
    mRootJetNode->setWorldRotation(mPfd.rotation);
    mRootJetNode->setWorldPosition(mPfd.position);

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
}

void AircraftController::setRootJetNode(Node *newRootJetNode)
{
    mRootJetNode = newRootJetNode;
}

void AircraftController::setJet(Model *newJet)
{
    mJet = newJet;
}

void AircraftController::drawGUI()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Simulator");

    ImGui::BeginDisabled();
    ImGui::SliderFloat("Elevator", &mElevator, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Aileron", &mAileron, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Rudder", &mRudder, -1.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Throttle", &mThrottle, 0.0f, 1.0f, "%.3f");
    ImGui::EndDisabled();

    if (ImGui::Button("Init Running"))
        emit command(Aircraft::Command::InitRunning);

    if (ImGui::Button("Hold"))
        emit command(Aircraft::Command::Hold);

    if (ImGui::Button("Resume"))
        emit command(Aircraft::Command::Resume);

    ImGui::Text("Airspeed:    %.2f knots", mPfd.airspeed);
    ImGui::Text("Latitude:    %.6f °", mPfd.latitude);
    ImGui::Text("Longitude:   %.6f °", mPfd.longitude);
    ImGui::Text("Altitude:    %.2f meters", mPfd.altitude);
    ImGui::Text("Roll:        %.1f °", mPfd.roll);
    ImGui::Text("Pitch:       %.1f °", mPfd.pitch);
    ImGui::Text("Heading:     %.1f °", mPfd.heading);

    ImGui::End();
}
