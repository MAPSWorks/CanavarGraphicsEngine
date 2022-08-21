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

void AircraftController::drawGui()
{
    // Simulator
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
    }
}
