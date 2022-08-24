#include "RandomGenerator.h"

#include <QVector3D>

RandomGenerator::RandomGenerator(QObject *parent)
    : QObject(parent)
{}

float RandomGenerator::getRandomFloat(float bound)
{
    return mGenerator.bounded(bound);
}

QVector3D RandomGenerator::getRandomVec3(float x, float y, float z)
{
    return QVector3D(getRandomFloat(x), getRandomFloat(y), getRandomFloat(z));
}

QRandomGenerator RandomGenerator::mGenerator = QRandomGenerator::securelySeeded();
