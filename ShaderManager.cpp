#include "ShaderManager.h"
#include "Helper.h"

ShaderManager::ShaderManager(QObject *parent)
    : QObject{parent}
    , mActiveShader(Shader::None)
{}

bool ShaderManager::init()
{
    initializeOpenGLFunctions();

    // Basic
    {
        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::Basic, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Basic.vert")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Basic.frag")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
            return false;
        }

        if (!shader->link())
        {
            qWarning() << Q_FUNC_INFO << "Could not link shader program.";
            return false;
        }

        if (!shader->bind())
        {
            qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("light.color", shader->uniformLocation("light.color"));
        locations.insert("light.position", shader->uniformLocation("light.position"));
        locations.insert("light.ambient", shader->uniformLocation("light.ambient"));
        locations.insert("light.diffuse", shader->uniformLocation("light.diffuse"));
        locations.insert("light.specular", shader->uniformLocation("light.specular"));

        locations.insert("node.transformation", shader->uniformLocation("node.transformation"));
        locations.insert("node.color", shader->uniformLocation("node.color"));
        locations.insert("node.ambient", shader->uniformLocation("node.ambient"));
        locations.insert("node.diffuse", shader->uniformLocation("node.diffuse"));
        locations.insert("node.specular", shader->uniformLocation("node.specular"));
        locations.insert("node.shininess", shader->uniformLocation("node.shininess"));

        locations.insert("camera_position", shader->uniformLocation("camera_position"));
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);

        shader->release();

        mLocations.insert(Shader::Basic, locations);

        qInfo() << Q_FUNC_INFO << "BasicShader is initialized.";
        qInfo() << Q_FUNC_INFO << "Uniform locations are:" << locations;
    }
    return true;
}

bool ShaderManager::bind(Shader shader)
{
    mActiveShader = shader;
    return mPrograms.value(mActiveShader)->bind();
}

void ShaderManager::release()
{
    mPrograms.value(mActiveShader)->release();
}

void ShaderManager::setUniformValue(const QString &name, int value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValue(const QString &name, float value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValue(const QString &name, const QVector3D &value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValue(const QString &name, const QVector4D &value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValue(const QString &name, const QMatrix3x3 &value)
{
    mPrograms.value(mActiveShader)->setUniformValue(mLocations.value(mActiveShader).value(name), value);
}

void ShaderManager::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mPrograms.value(mActiveShader)->setUniformValueArray(mLocations.value(mActiveShader).value(name), values.constData(), values.size());
}

ShaderManager *ShaderManager::instance()
{
    static ShaderManager instance;

    return &instance;
}
