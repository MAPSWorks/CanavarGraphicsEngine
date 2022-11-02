#ifndef HAZE_H
#define HAZE_H

#include "Common.h"
#include "Node.h"

namespace Canavar {
namespace Engine {

class Haze : public Node
{
    Q_OBJECT
private:
    Haze(const QString &uuid = QString());

public:
    static Haze *instance();

    DEFINE_MEMBER(QVector3D, Color)
    DEFINE_MEMBER(bool, Enabled)
    DEFINE_MEMBER(float, Density)
    DEFINE_MEMBER(float, Gradient)
};

} // namespace Engine
} // namespace Canavar

#endif // HAZE_H
