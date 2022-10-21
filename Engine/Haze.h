#ifndef HAZE_H
#define HAZE_H

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

public:
    QVector3D mColor;
    bool mEnabled;
    float mDensity;
    float mGradient;
};

} // namespace Engine
} // namespace Canavar

#endif // HAZE_H
