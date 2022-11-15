#include "LineStrip.h"

Canavar::Engine::LineStrip::LineStrip(QObject *parent)
    : QObject(parent)
    , mColor(1, 0, 0, 1)
{}

void Canavar::Engine::LineStrip::appendPoint(const QVector3D &point)
{
    mPoints << point;
}

void Canavar::Engine::LineStrip::insertPoint(int index, const QVector3D &point)
{
    mPoints.insert(index, point);
}

void Canavar::Engine::LineStrip::removePoint(int index)
{
    mPoints.removeAt(index);
}

void Canavar::Engine::LineStrip::removeAllPoints()
{
    mPoints.clear();
}

const QVector<QVector3D> &Canavar::Engine::LineStrip::points() const
{
    return mPoints;
}
