#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "PointLight.h"

class SpotLight : public PointLight
{
public:
    explicit SpotLight(QObject *parent = nullptr);

    const QVector3D &direction() const;
    void setDirection(const QVector3D &newDirection);

    float cutOffAngle() const;
    void setCutOffAngle(float newCutOffAngle);

    float outerCutOffAngle() const;
    void setOuterCutOffAngle(float newOuterCutOffAngle);

private:
    QVector3D mDirection;
    float mCutOffAngle;
    float mOuterCutOffAngle;
};

#endif // SPOTLIGHT_H
