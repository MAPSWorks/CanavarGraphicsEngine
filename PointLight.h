#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light
{
protected:
    friend class NodeManager;
    explicit PointLight(QObject *parent = nullptr);
    virtual ~PointLight();

public:
    float constant() const;
    void setConstant(float newConstant);

    float linear() const;
    void setLinear(float newLinear);

    float quadratic() const;
    void setQuadratic(float newQuadratic);

    virtual void drawGUI() override;

protected:
    float mConstant;
    float mLinear;
    float mQuadratic;
};

#endif // POINTLIGHT_H
