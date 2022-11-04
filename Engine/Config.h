#ifndef CONFIG_H
#define CONFIG_H

#include "Common.h"

#include <QObject>

namespace Canavar {
namespace Engine {

class Config : public QObject
{
    Q_OBJECT
private:
    explicit Config(QObject *parent = nullptr);

public:
    static Config *instance();

    void load(const QString &configFile);

private:
    DEFINE_MEMBER_CONST(QString, ModelsRootFolder)
    DEFINE_MEMBER_CONST(QStringList, SupportedModelFormats)
    DEFINE_MEMBER_CONST(bool, NodeSelectionEnabled)
};

} // namespace Engine
} // namespace Canavar

#endif // CONFIG_H
