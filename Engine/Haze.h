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
    explicit Haze(QObject *parent = nullptr);

public:
    static Haze *instance();

    DECLARE_MEMBER(QVector3D, Color)
    DECLARE_MEMBER(bool, Enabled)
    DECLARE_MEMBER(float, Density)
    DECLARE_MEMBER(float, Gradient)
};

} // namespace Engine
} // namespace Canavar

#endif // HAZE_H
