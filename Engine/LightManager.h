#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Manager.h"
#include "Sun.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class LightManager : public Manager
{
    Q_OBJECT

private:
    friend class Canavar::Engine::NodeManager;

    explicit LightManager(QObject *parent = nullptr);

    void setSun(Sun *sun);

public:
    static LightManager *instance();

    bool init() override;

    Sun *sun() const;

private:
    Sun *mSun;
};

} // namespace Engine
} // namespace Canavar

#endif // LIGHTMANAGER_H
