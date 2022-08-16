#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light
{
public:
    explicit PointLight(QObject *parent = nullptr);

    float constant() const;
    void setConstant(float newConstant);

    float linear() const;
    void setLinear(float newLinear);

    float quadratic() const;
    void setQuadratic(float newQuadratic);

protected:
    float mConstant;
    float mLinear;
    float mQuadratic;
};

#endif // POINTLIGHT_H
