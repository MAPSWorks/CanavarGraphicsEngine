#ifndef SUN_H
#define SUN_H

#include "Light.h"

namespace Canavar {
namespace Engine {

class Sun : public Light
{
private:
    Sun();

protected:
    friend class NodeManager;
    virtual void toJson(QJsonObject &object) override;
    virtual void fromJson(const QJsonObject &object) override;

public:
    static Sun *instance();

    const QVector3D &getDirection() const;
    QVector3D &getDirection_nonConst();

    void setDirection(const QVector3D &newDirection);

    bool getEnabled() const;
    void setEnabled(bool newEnabled);

protected:
    QVector3D mDirection;
    bool mEnabled;
};

} // namespace Engine
} // namespace Canavar

#endif // SUN_H
