#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "PointLight.h"

class SpotLight : public PointLight
{
private:
    friend class NodeManager;
    explicit SpotLight(QObject *parent = nullptr);
    virtual ~SpotLight();

public:
    const QVector3D &direction() const;
    void setDirection(const QVector3D &newDirection);

    float cutOffAngle() const;
    void setCutOffAngle(float newCutOffAngle);

    float outerCutOffAngle() const;
    void setOuterCutOffAngle(float newOuterCutOffAngle);

    virtual void drawGui() override;

private:
    QVector3D mDirection;
    float mCutOffAngle;
    float mOuterCutOffAngle;
};

#endif // SPOTLIGHT_H
