#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class Shader;

class ShaderManager : public QObject
{
    Q_OBJECT
public:
    explicit ShaderManager(QObject *parent = nullptr);

    enum class ShaderType { //
        None,
        ModelShader,
        WireframeShader,
        NormalsShader,
        SkyBoxShader,
        TerrainShader,
        ScreenShader,
        NozzleEffectShader,
        NozzleBlurShader,
        SkyShader,
        MotionBlurShader,
        VolumetricCloudsShader,
        WeatherShader,
        PerlinWorleyShader,
        WorleyShader,
        PostProcessingShader,
    };

    Shader *getShader(ShaderType shader);

    bool init();
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

    static ShaderManager *instance();

private:
    ShaderType mActiveShader;
    QMap<ShaderType, Shader *> mShaders;
};

#endif // SHADERMANAGER_H
