#ifndef LIGHT_H
#define LIGHT_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class Light : public Node
{
protected:
    explicit Light(QObject *parent = nullptr);
    virtual ~Light();

public:
    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    float ambient() const;
    void setAmbient(float newAmbient);

    float diffuse() const;
    void setDiffuse(float newDiffuse);

    float specular() const;
    void setSpecular(float newSpecular);

protected:
    QVector4D mColor;
    float mAmbient;
    float mDiffuse;
    float mSpecular;
};

} // namespace Engine
} // namespace Canavar
#endif // LIGHT_H
