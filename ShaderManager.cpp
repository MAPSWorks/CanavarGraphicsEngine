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

    // Basic
    {
        qInfo() << "BasicShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::BasicShader, shader);

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

        locations.insert("directionalLight.color", shader->uniformLocation("directionalLight.color"));
        locations.insert("directionalLight.direction", shader->uniformLocation("directionalLight.direction"));
        locations.insert("directionalLight.ambient", shader->uniformLocation("directionalLight.ambient"));
        locations.insert("directionalLight.diffuse", shader->uniformLocation("directionalLight.diffuse"));
        locations.insert("directionalLight.specular", shader->uniformLocation("directionalLight.specular"));

        locations.insert("numberOfPointLights", shader->uniformLocation("numberOfPointLights"));
        locations.insert("numberOfSpotLights", shader->uniformLocation("numberOfSpotLights"));

        for (int i = 0; i < 8; i++)
        {
            locations.insert("pointLights[" + QString::number(i) + "].color", shader->uniformLocation("pointLights[" + QString::number(i) + "].color"));
            locations.insert("pointLights[" + QString::number(i) + "].position", shader->uniformLocation("pointLights[" + QString::number(i) + "].position"));
            locations.insert("pointLights[" + QString::number(i) + "].ambient", shader->uniformLocation("pointLights[" + QString::number(i) + "].ambient"));
            locations.insert("pointLights[" + QString::number(i) + "].diffuse", shader->uniformLocation("pointLights[" + QString::number(i) + "].diffuse"));
            locations.insert("pointLights[" + QString::number(i) + "].specular", shader->uniformLocation("pointLights[" + QString::number(i) + "].specular"));
            locations.insert("pointLights[" + QString::number(i) + "].constant", shader->uniformLocation("pointLights[" + QString::number(i) + "].constant"));
            locations.insert("pointLights[" + QString::number(i) + "].linear", shader->uniformLocation("pointLights[" + QString::number(i) + "].linear"));
            locations.insert("pointLights[" + QString::number(i) + "].quadratic", shader->uniformLocation("pointLights[" + QString::number(i) + "].quadratic"));
        }

        for (int i = 0; i < 8; i++)
        {
            locations.insert("spotLights[" + QString::number(i) + "].color", shader->uniformLocation("spotLights[" + QString::number(i) + "].color"));
            locations.insert("spotLights[" + QString::number(i) + "].position", shader->uniformLocation("spotLights[" + QString::number(i) + "].position"));
            locations.insert("spotLights[" + QString::number(i) + "].direction", shader->uniformLocation("spotLights[" + QString::number(i) + "].direction"));
            locations.insert("spotLights[" + QString::number(i) + "].ambient", shader->uniformLocation("spotLights[" + QString::number(i) + "].ambient"));
            locations.insert("spotLights[" + QString::number(i) + "].diffuse", shader->uniformLocation("spotLights[" + QString::number(i) + "].diffuse"));
            locations.insert("spotLights[" + QString::number(i) + "].specular", shader->uniformLocation("spotLights[" + QString::number(i) + "].specular"));
            locations.insert("spotLights[" + QString::number(i) + "].constant", shader->uniformLocation("spotLights[" + QString::number(i) + "].constant"));
            locations.insert("spotLights[" + QString::number(i) + "].linear", shader->uniformLocation("spotLights[" + QString::number(i) + "].linear"));
            locations.insert("spotLights[" + QString::number(i) + "].quadratic", shader->uniformLocation("spotLights[" + QString::number(i) + "].quadratic"));
            locations.insert("spotLights[" + QString::number(i) + "].cutOffAngle", shader->uniformLocation("spotLights[" + QString::number(i) + "].cutOffAngle"));
            locations.insert("spotLights[" + QString::number(i) + "].outerCutOffAngle", shader->uniformLocation("spotLights[" + QString::number(i) + "].outerCutOffAngle"));
        }

        locations.insert("node.transformation", shader->uniformLocation("node.transformation"));
        locations.insert("node.color", shader->uniformLocation("node.color"));
        locations.insert("node.ambient", shader->uniformLocation("node.ambient"));
        locations.insert("node.diffuse", shader->uniformLocation("node.diffuse"));
        locations.insert("node.specular", shader->uniformLocation("node.specular"));
        locations.insert("node.shininess", shader->uniformLocation("node.shininess"));

        locations.insert("cameraPosition", shader->uniformLocation("cameraPosition"));
        locations.insert("viewMatrix", shader->uniformLocation("viewMatrix"));
        locations.insert("projectionMatrix", shader->uniformLocation("projectionMatrix"));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);

        shader->release();

        mLocations.insert(Shader::BasicShader, locations);

        qInfo() << "BasicShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // Textured Model
    {
        qInfo() << "TexturedModelShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::TexturedModelShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/TexturedModel.vert")))
        {
            qWarning() << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/TexturedModel.frag")))
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

        locations.insert("directionalLight.color", shader->uniformLocation("directionalLight.color"));
        locations.insert("directionalLight.direction", shader->uniformLocation("directionalLight.direction"));
        locations.insert("directionalLight.ambient", shader->uniformLocation("directionalLight.ambient"));
        locations.insert("directionalLight.diffuse", shader->uniformLocation("directionalLight.diffuse"));
        locations.insert("directionalLight.specular", shader->uniformLocation("directionalLight.specular"));

        locations.insert("numberOfPointLights", shader->uniformLocation("numberOfPointLights"));
        locations.insert("numberOfSpotLights", shader->uniformLocation("numberOfSpotLights"));

        for (int i = 0; i < 8; i++)
        {
            locations.insert("pointLights[" + QString::number(i) + "].color", shader->uniformLocation("pointLights[" + QString::number(i) + "].color"));
            locations.insert("pointLights[" + QString::number(i) + "].position", shader->uniformLocation("pointLights[" + QString::number(i) + "].position"));
            locations.insert("pointLights[" + QString::number(i) + "].ambient", shader->uniformLocation("pointLights[" + QString::number(i) + "].ambient"));
            locations.insert("pointLights[" + QString::number(i) + "].diffuse", shader->uniformLocation("pointLights[" + QString::number(i) + "].diffuse"));
            locations.insert("pointLights[" + QString::number(i) + "].specular", shader->uniformLocation("pointLights[" + QString::number(i) + "].specular"));
            locations.insert("pointLights[" + QString::number(i) + "].constant", shader->uniformLocation("pointLights[" + QString::number(i) + "].constant"));
            locations.insert("pointLights[" + QString::number(i) + "].linear", shader->uniformLocation("pointLights[" + QString::number(i) + "].linear"));
            locations.insert("pointLights[" + QString::number(i) + "].quadratic", shader->uniformLocation("pointLights[" + QString::number(i) + "].quadratic"));
        }

        for (int i = 0; i < 8; i++)
        {
            locations.insert("spotLights[" + QString::number(i) + "].color", shader->uniformLocation("spotLights[" + QString::number(i) + "].color"));
            locations.insert("spotLights[" + QString::number(i) + "].position", shader->uniformLocation("spotLights[" + QString::number(i) + "].position"));
            locations.insert("spotLights[" + QString::number(i) + "].direction", shader->uniformLocation("spotLights[" + QString::number(i) + "].direction"));
            locations.insert("spotLights[" + QString::number(i) + "].ambient", shader->uniformLocation("spotLights[" + QString::number(i) + "].ambient"));
            locations.insert("spotLights[" + QString::number(i) + "].diffuse", shader->uniformLocation("spotLights[" + QString::number(i) + "].diffuse"));
            locations.insert("spotLights[" + QString::number(i) + "].specular", shader->uniformLocation("spotLights[" + QString::number(i) + "].specular"));
            locations.insert("spotLights[" + QString::number(i) + "].constant", shader->uniformLocation("spotLights[" + QString::number(i) + "].constant"));
            locations.insert("spotLights[" + QString::number(i) + "].linear", shader->uniformLocation("spotLights[" + QString::number(i) + "].linear"));
            locations.insert("spotLights[" + QString::number(i) + "].quadratic", shader->uniformLocation("spotLights[" + QString::number(i) + "].quadratic"));
            locations.insert("spotLights[" + QString::number(i) + "].cutOffAngle", shader->uniformLocation("spotLights[" + QString::number(i) + "].cutOffAngle"));
            locations.insert("spotLights[" + QString::number(i) + "].outerCutOffAngle", shader->uniformLocation("spotLights[" + QString::number(i) + "].outerCutOffAngle"));
        }

        locations.insert("nodeMatrix", shader->uniformLocation("nodeMatrix"));
        locations.insert("nodeShininess", shader->uniformLocation("nodeShininess"));
        locations.insert("cameraPosition", shader->uniformLocation("cameraPosition"));
        locations.insert("viewMatrix", shader->uniformLocation("viewMatrix"));
        locations.insert("projectionMatrix", shader->uniformLocation("projectionMatrix"));
        locations.insert("textureDiffuse", shader->uniformLocation("textureDiffuse"));
        locations.insert("textureSpecular", shader->uniformLocation("textureSpecular"));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);
        shader->bindAttributeLocation("texture_coord", 2);
        shader->bindAttributeLocation("tangent", 3);
        shader->bindAttributeLocation("bitangent", 4);
        shader->bindAttributeLocation("ids", 5);
        shader->bindAttributeLocation("weights", 6);

        shader->release();

        mLocations.insert(Shader::TexturedModelShader, locations);

        qInfo() << "TexturedModelShader is initialized.";
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

        locations.insert("amplitude", shader->uniformLocation("amplitude"));
        locations.insert("freq", shader->uniformLocation("freq"));
        locations.insert("octaves", shader->uniformLocation("octaves"));
        locations.insert("power", shader->uniformLocation("power"));
        locations.insert("seed", shader->uniformLocation("seed"));
        locations.insert("clip_plane", shader->uniformLocation("clip_plane"));
        locations.insert("tessellation_multiplier", shader->uniformLocation("tessellation_multiplier"));

        locations.insert("directional_light.color", shader->uniformLocation("directional_light.color"));
        locations.insert("directional_light.direction", shader->uniformLocation("directional_light.direction"));
        locations.insert("directional_light.ambient", shader->uniformLocation("directional_light.ambient"));
        locations.insert("directional_light.diffuse", shader->uniformLocation("directional_light.diffuse"));
        locations.insert("directional_light.specular", shader->uniformLocation("directional_light.specular"));

        locations.insert("node_matrix", shader->uniformLocation("node_matrix"));
        locations.insert("camera_position", shader->uniformLocation("camera_position"));
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));

        shader->bindAttributeLocation("position", 0);
        shader->bindAttributeLocation("normal", 1);
        shader->bindAttributeLocation("texture_coord", 2);
        shader->bindAttributeLocation("offset_position", 3);

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
