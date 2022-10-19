#include "ShaderManager.h"
#include "Shader.h"

Canavar::Engine::ShaderManager::ShaderManager(QObject *parent)
    : Manager(parent)
    , mActiveShader(ShaderType::None)
{}

Canavar::Engine::Shader *Canavar::Engine::ShaderManager::getShader(ShaderType shader)
{
    return mShaders.value(shader);
}

bool Canavar::Engine::ShaderManager::init()
{
    // Model Shader
    {
        Shader *shader = new Shader(ShaderType::ModelShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Model.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Model.frag");

        QStringList uniforms;
        uniforms << "M"
                 << "N"
                 << "VP"
                 << "color"
                 << "ambient"
                 << "diffuse"
                 << "specular"
                 << "shininess"
                 << "cameraPos"
                 << "sunDir";

        shader->addUniforms(uniforms);

        shader->addAttribute("position");
        shader->addAttribute("normal");
        shader->addAttribute("textureCoords");
        shader->addAttribute("tangent");
        shader->addAttribute("bitangent");
        shader->addAttribute("ids");
        shader->addAttribute("weights");

        if (!shader->init())
            return false;
    }

    return true;
}

bool Canavar::Engine::ShaderManager::bind(ShaderType shader)
{
    mActiveShader = shader;
    return mShaders.value(mActiveShader)->bind();
}

void Canavar::Engine::ShaderManager::release()
{
    mShaders.value(mActiveShader)->release();
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, int value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, unsigned int value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, float value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, const QVector3D &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, const QVector4D &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValue(const QString &name, const QMatrix3x3 &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mShaders.value(mActiveShader)->setUniformValueArray(name, values);
}

void Canavar::Engine::ShaderManager::setSampler(const QString &name, unsigned int unit, unsigned int id, GLenum target)
{
    mShaders.value(mActiveShader)->setSampler(name, unit, id, target);
}

Canavar::Engine::ShaderManager *Canavar::Engine::ShaderManager::instance()
{
    static ShaderManager instance;

    return &instance;
}
