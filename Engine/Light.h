#ifndef LIGHT_H
#define LIGHT_H

#include "Node.h"

namespace Canavar {
namespace Engine {

class Light : public Node
{
protected:
    explicit Light(QObject *parent = nullptr);

    DECLARE_MEMBER(QString, ModelName)

    DECLARE_MEMBER(QVector4D, Color)
    DECLARE_MEMBER(float, Ambient)
    DECLARE_MEMBER(float, Diffuse)
    DECLARE_MEMBER(float, Specular)

    DECLARE_MEMBER(QVector4D, ModelColor)
    DECLARE_MEMBER(float, ModelAmbient)
    DECLARE_MEMBER(float, ModelDiffuse)
    DECLARE_MEMBER(float, ModelSpecular)
    DECLARE_MEMBER(float, ModelShininess)
};

} // namespace Engine
} // namespace Canavar
#endif // LIGHT_H
