#include "Sky.h"

Sky::Sky(QObject *parent)
    : QObject{parent}
    , mSkyColorTop(0.5, 0.7, 0.8, 1)
    , mSkyColorBottom(0.9, 0.9, 0.95, 1)
{}

const QVector4D &Sky::skyColorTop() const
{
    return mSkyColorTop;
}

const QVector4D &Sky::skyColorBottom() const
{
    return mSkyColorBottom;
}
