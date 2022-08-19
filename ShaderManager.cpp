#include "ShaderManager.h"
#include "Helper.h"

ShaderManager::ShaderManager(QObject *parent)
    : QObject{parent}
    , mActiveShader(Shader::None)
{}

QOpenGLShaderProgram *ShaderManager::getShader(Shader shader)
{
    return mPrograms.value(shader);
}

bool ShaderManager::init()
{
    initializeOpenGLFunctions();

    // Textured Model
    {
        qInfo() << "ModelShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::ModelShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Model.vert")))
        {
            qWarning() << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Model.frag")))
        {
            qWarning() << "Could not load fragment shader.";
            return false;
        }

        if (!shader->link())
        {
            qWarning() << "Could not link shader program.";
            return false;
        }

        if (!shader->bind())
        {
            qWarning() << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        QStringList uniforms;
        uniforms << "directionalLight.color"
                 << "directionalLight.direction"
                 << "directionalLight.ambient"
                 << "directionalLight.diffuse"
                 << "directionalLight.specular"
                 << "numberOfPointLights"
                 << "numberOfSpotLights"
                 << "node.transformation"
                 << "node.ambient"
                 << "node.diffuse"
                 << "node.specular"
                 << "node.shininess"
                 << "node.color"
                 << "textureDiffuse"
                 << "tetureSpecular"
                 << "textureAmbient"
                 << "textureHeight"
                 << "cameraPosition"
                 << "viewMatrix"
                 << "projectionMatrix"
                 << "useTexture"
                 << "fog.enabled"
                 << "fog.color"
                 << "fog.density"
                 << "fog.gradient";

        for (const auto &uniform : qAsConst(uniforms))
            locations.insert(uniform, shader->uniformLocation(uniform));

        uniforms.clear();
        uniforms << "pointLights[%1].color"
                 << "pointLights[%1].position"
                 << "pointLights[%1].ambient"
                 << "pointLights[%1].diffuse"
                 << "pointLights[%1].specular"
                 << "pointLights[%1].constant"
                 << "pointLights[%1].linear"
                 << "pointLights[%1].quadratic";

        for (const auto &uniform : qAsConst(uniforms))
            for (int i = 0; i < 8; i++)
                locations.insert(uniform.arg(QString::number(i)), shader->uniformLocation(uniform.arg(QString::number(i))));

        uniforms.clear();
        uniforms << "spotLights[%1].color"
                 << "spotLights[%1].position"
                 << "spotLights[%1].ambient"
                 << "spotLights[%1].diffuse"
                 << "spotLights[%1].specular"
                 << "spotLights[%1].constant"
                 << "spotLights[%1].linear"
                 << "spotLights[%1].quadratic"
                 << "spotLights[%1].cutOffAngle"
                 << "spotLights[%1].outerCutOffAngle";

        for (const auto &uniform : qAsConst(uniforms))
            for (int i = 0; i < 8; i++)
                locations.insert(uniform.arg(QString::number(i)), shader->uniformLocation(uniform.arg(QString::number(i))));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);
        shader->bindAttributeLocation("texture_coord", 2);
        shader->bindAttributeLocation("tangent", 3);
        shader->bindAttributeLocation("bitangent", 4);
        shader->bindAttributeLocation("ids", 5);
        shader->bindAttributeLocation("weights", 6);

        shader->release();

        mLocations.insert(Shader::ModelShader, locations);

        qInfo() << "ModelShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // Wireframe
    {
        qInfo() << "WireframeShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::WireframeShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Wireframe.vert")))
        {
            qWarning() << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Geometry, Helper::getBytes(":/Resources/Shaders/Wireframe.geom")))
        {
            qWarning() << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Wireframe.frag")))
        {
            qWarning() << "Could not load fragment shader.";
            return false;
        }

        if (!shader->link())
        {
            qWarning() << "Could not link shader program.";
            return false;
        }

        if (!shader->bind())
        {
            qWarning() << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("nodeMatrix", shader->uniformLocation("nodeMatrix"));
        locations.insert("viewMatrix", shader->uniformLocation("viewMatrix"));
        locations.insert("projectionMatrix", shader->uniformLocation("projectionMatrix"));

        shader->bindAttributeLocation("position", 0);

        shader->release();

        mLocations.insert(Shader::WireframeShader, locations);

        qInfo() << "WireframeShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // Normals
    {
        qInfo() << "NormalsShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::NormalsShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Normals.vert")))
        {
            qWarning() << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Geometry, Helper::getBytes(":/Resources/Shaders/Normals.geom")))
        {
            qWarning() << "Could not load geometry shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Normals.frag")))
        {
            qWarning() << "Could not load fragment shader.";
            return false;
        }

        if (!shader->link())
        {
            qWarning() << "Could not link shader program.";
            return false;
        }

        if (!shader->bind())
        {
            qWarning() << "Could not bind shader program.";
            return false;
        }

        QMap<QString, GLuint> locations;

        locations.insert("nodeMatrix", shader->uniformLocation("nodeMatrix"));
        locations.insert("viewMatrix", shader->uniformLocation("viewMatrix"));
        locations.insert("projectionMatrix", shader->uniformLocation("projectionMatrix"));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);

        shader->release();

        mLocations.insert(Shader::NormalsShader, locations);

        qInfo() << "NormalsShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // SkyBoxShader
    {
        qInfo() << "SkyBoxShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::SkyBoxShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/SkyBox.vert")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/SkyBox.frag")))
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

        locations.insert("skybox", shader->uniformLocation("skybox"));
        locations.insert("viewMatrix", shader->uniformLocation("viewMatrix"));
        locations.insert("projectionMatrix", shader->uniformLocation("projectionMatrix"));

        shader->bindAttributeLocation("position", 0);

        shader->release();

        mLocations.insert(Shader::SkyBoxShader, locations);

        qInfo() << "SkyBoxShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // TerrainShader
    {
        qInfo() << "TerrainShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;

        mPrograms.insert(Shader::TerrainShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Terrain.vert")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::TessellationControl, Helper::getBytes(":/Resources/Shaders/Terrain.tcs")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load tessellation control shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::TessellationEvaluation, Helper::getBytes(":/Resources/Shaders/Terrain.tes")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load tessellation evaluation shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Terrain.frag")))
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

        QStringList uniforms;
        uniforms << "terrain.amplitude"
                 << "terrain.frequency"
                 << "terrain.octaves"
                 << "terrain.power"
                 << "terrain.seed"
                 << "terrain.clipPlane"
                 << "terrain.tessellationMultiplier"
                 << "terrain.grassCoverage"
                 << "terrain.ambient"
                 << "terrain.diffuse"
                 << "terrain.shininess"
                 << "terrain.specular"
                 << "fog.enabled"
                 << "fog.color"
                 << "fog.density"
                 << "fog.gradient"
                 << "directionalLight.color"
                 << "directionalLight.direction"
                 << "directionalLight.ambient"
                 << "directionalLight.diffuse"
                 << "directionalLight.specular"
                 << "waterHeight"
                 << "cameraPosition"
                 << "nodeMatrix"
                 << "viewMatrix"
                 << "projectionMatrix"
                 << "sand"
                 << "grass"
                 << "terrainTexture"
                 << "snow"
                 << "rock"
                 << "rockNormal";

        for (const auto &uniform : qAsConst(uniforms))
            locations.insert(uniform, shader->uniformLocation(uniform));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);
        shader->bindAttributeLocation("textureCoord", 2);
        shader->bindAttributeLocation("offsetPosition", 3);

        shader->release();

        mLocations.insert(Shader::TerrainShader, locations);

        qInfo() << "TerrainShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
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

void ShaderManager::setUniformValue(const QString &name, unsigned int value)
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
