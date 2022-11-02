#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "Common.h"
#include "Manager.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

namespace Canavar {
namespace Engine {

class Shader;

class ShaderManager : public Manager
{
    Q_OBJECT

private:
    explicit ShaderManager(QObject *parent = nullptr);

public:
    static ShaderManager *instance();

    Shader *getShader(ShaderType shader);

    bool init() override;

    bool bind(ShaderType shader);
    void release();

    void setUniformValue(const QString &name, int value);
    void setUniformValue(const QString &name, unsigned int value);
    void setUniformValue(const QString &name, float value);
    void setUniformValue(const QString &name, const QVector3D &value);
    void setUniformValue(const QString &name, const QVector4D &value);
    void setUniformValue(const QString &name, const QMatrix4x4 &value);
    void setUniformValue(const QString &name, const QMatrix3x3 &value);
    void setUniformValueArray(const QString &name, const QVector<QVector3D> &values);
    void setSampler(const QString &name, unsigned int unit, unsigned int id, GLenum target = GL_TEXTURE_2D);

private:
    ShaderType mActiveShader;
    QMap<ShaderType, Shader *> mShaders;
};

} // namespace Engine
} // namespace Canavar

#endif // SHADERMANAGER_H
