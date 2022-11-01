#include "Shader.h"
#include "Helper.h"

#include <QDebug>

Canavar::Engine::Shader::Shader(ShaderType type, QObject *parent)
    : QObject(parent)
    , mType(type)
{}

bool Canavar::Engine::Shader::init()
{
    initializeOpenGLFunctions();

    mShaderName = mPaths.value(QOpenGLShader::Vertex).sliced(mPaths.value(QOpenGLShader::Vertex).lastIndexOf("/") + 1);

    qInfo() << mShaderName << "is initializing... ";

    mProgram = new QOpenGLShaderProgram;

    auto types = mPaths.keys();

    for (auto type : qAsConst(types))
    {
        if (!mProgram->addShaderFromSourceCode(type, Helper::getBytes(mPaths[type])))
        {
            qWarning() << Q_FUNC_INFO << "Could not load" << getShaderTypeString(type);
            mProgram->deleteLater();
            return false;
        }
    }

    if (!mProgram->link())
    {
        qWarning() << Q_FUNC_INFO << "Could not link shader program.";
        mProgram->deleteLater();
        return false;
    }

    if (!mProgram->bind())
    {
        qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
        mProgram->deleteLater();
        return false;
    }

    for (const auto &uniform : qAsConst(mUniforms))
    {
        mLocations.insert(uniform, mProgram->uniformLocation(uniform));
    }

    auto uniformArrays = mUniformArrays.keys();
    for (const auto &uniformArray : qAsConst(uniformArrays))
    {
        int size = mUniformArrays[uniformArray];

        for (int i = 0; i < size; ++i)
        {
            QString name = uniformArray.arg(QString::number(i));
            mLocations.insert(name, mProgram->uniformLocation(name));
        }
    }

    for (int i = 0; i < mAttributes.size(); ++i)
    {
        mProgram->bindAttributeLocation(mAttributes[i], i);
    }

    mProgram->release();

    qInfo() << mShaderName << "is initialized.";
    qInfo() << "Uniform locations are:" << mLocations;

    return true;
}

bool Canavar::Engine::Shader::bind()
{
    return mProgram->bind();
}

void Canavar::Engine::Shader::release()
{
    mProgram->release();
}

void Canavar::Engine::Shader::addPath(QOpenGLShader::ShaderTypeBit type, const QString &path)
{
    mPaths.insert(type, path);
}

void Canavar::Engine::Shader::addUniform(const QString &uniform)
{
    mUniforms << uniform;
}

void Canavar::Engine::Shader::addUniforms(const QStringList &uniforms)
{
    mUniforms << uniforms;
}

void Canavar::Engine::Shader::setUniformArray(const QString &uniform, int size)
{
    mUniformArrays.insert(uniform, size);
}

void Canavar::Engine::Shader::addAttribute(const QString &attribute)
{
    mAttributes << attribute;
}

void Canavar::Engine::Shader::addAttributes(const QStringList &attributes)
{
    mAttributes << attributes;
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, int value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, unsigned int value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, float value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, const QVector3D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, const QVector4D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValue(const QString &name, const QMatrix3x3 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void Canavar::Engine::Shader::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mProgram->setUniformValueArray(mLocations[name], values.constData(), values.size());
}

void Canavar::Engine::Shader::setSampler(const QString &name, unsigned int unit, unsigned int id, GLenum target)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, id);
    setUniformValue(name, unit);
}

Canavar::Engine::ShaderType Canavar::Engine::Shader::type() const
{
    return mType;
}

QString Canavar::Engine::Shader::getShaderTypeString(QOpenGLShader::ShaderTypeBit type)
{
    switch (type)
    {
    case QOpenGLShader::Vertex:
        return "Vertex Shader";
    case QOpenGLShader::Fragment:
        return "Fragment Shader";
    case QOpenGLShader::Geometry:
        return "Geometry Shader";
    case QOpenGLShader::TessellationControl:
        return "Tessellation Control Shader";
    case QOpenGLShader::TessellationEvaluation:
        return "Tesselation Evaluation Shader";
    case QOpenGLShader::Compute:
        return "Compute Shader";
    default:
        return "Unknown Shader";
    }
}
