#ifndef NOZZLEEFFECT_H
#define NOZZLEEFFECT_H

#include "Model.h"

class NozzleEffect : public Model
{
public:
    explicit NozzleEffect(const QString &modelName, QObject *parent = nullptr);
    virtual ~NozzleEffect();

    float strength() const;
    void setStrength(float newStrength);

    int blurFactor() const;
    void setBlurFactor(int newBlurFactor);

private:
    float mStrength;
    int mBlurFactor;
};

#endif // NOZZLEEFFECT_H
