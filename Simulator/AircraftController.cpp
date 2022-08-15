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

    mElevator = qBound(-1.0, mElevator, 1.0);
    mAileron = qBound(-1.0, mAileron, 1.0);
    mRudder = qBound(-1.0, mRudder, 1.0);
    mThrottle = qBound(0.0, mThrottle, 1.0);

    emit command(Aircraft::Command::Elevator, mElevator);
    emit command(Aircraft::Command::Aileron, mAileron);
    emit command(Aircraft::Command::Rudder, mRudder);
    emit command(Aircraft::Command::Throttle, mThrottle);
}

double AircraftController::throttle() const
{
    return mThrottle;
}

bool AircraftController::holding() const
{
    return mAircraft->holding();
}

double AircraftController::rudder() const
{
    return mRudder;
}

double AircraftController::elevator() const
{
    return mElevator;
}

double AircraftController::aileron() const
{
    return mAileron;
}
