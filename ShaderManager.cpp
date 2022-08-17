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

        locations.insert("directional_light.color", shader->uniformLocation("directional_light.color"));
        locations.insert("directional_light.direction", shader->uniformLocation("directional_light.direction"));
        locations.insert("directional_light.ambient", shader->uniformLocation("directional_light.ambient"));
        locations.insert("directional_light.diffuse", shader->uniformLocation("directional_light.diffuse"));
        locations.insert("directional_light.specular", shader->uniformLocation("directional_light.specular"));

        locations.insert("number_of_point_lights", shader->uniformLocation("number_of_point_lights"));
        locations.insert("number_of_spot_lights", shader->uniformLocation("number_of_spot_lights"));

        for (int i = 0; i < 8; i++)
        {
            locations.insert("point_lights[" + QString::number(i) + "].color", shader->uniformLocation("point_lights[" + QString::number(i) + "].color"));
            locations.insert("point_lights[" + QString::number(i) + "].position", shader->uniformLocation("point_lights[" + QString::number(i) + "].position"));
            locations.insert("point_lights[" + QString::number(i) + "].ambient", shader->uniformLocation("point_lights[" + QString::number(i) + "].ambient"));
            locations.insert("point_lights[" + QString::number(i) + "].diffuse", shader->uniformLocation("point_lights[" + QString::number(i) + "].diffuse"));
            locations.insert("point_lights[" + QString::number(i) + "].specular", shader->uniformLocation("point_lights[" + QString::number(i) + "].specular"));
            locations.insert("point_lights[" + QString::number(i) + "].constant", shader->uniformLocation("point_lights[" + QString::number(i) + "].constant"));
            locations.insert("point_lights[" + QString::number(i) + "].linear", shader->uniformLocation("point_lights[" + QString::number(i) + "].linear"));
            locations.insert("point_lights[" + QString::number(i) + "].quadratic", shader->uniformLocation("point_lights[" + QString::number(i) + "].quadratic"));
        }

        for (int i = 0; i < 8; i++)
        {
            locations.insert("spot_lights[" + QString::number(i) + "].color", shader->uniformLocation("spot_lights[" + QString::number(i) + "].color"));
            locations.insert("spot_lights[" + QString::number(i) + "].position", shader->uniformLocation("spot_lights[" + QString::number(i) + "].position"));
            locations.insert("spot_lights[" + QString::number(i) + "].direction", shader->uniformLocation("spot_lights[" + QString::number(i) + "].direction"));
            locations.insert("spot_lights[" + QString::number(i) + "].ambient", shader->uniformLocation("spot_lights[" + QString::number(i) + "].ambient"));
            locations.insert("spot_lights[" + QString::number(i) + "].diffuse", shader->uniformLocation("spot_lights[" + QString::number(i) + "].diffuse"));
            locations.insert("spot_lights[" + QString::number(i) + "].specular", shader->uniformLocation("spot_lights[" + QString::number(i) + "].specular"));
            locations.insert("spot_lights[" + QString::number(i) + "].constant", shader->uniformLocation("spot_lights[" + QString::number(i) + "].constant"));
            locations.insert("spot_lights[" + QString::number(i) + "].linear", shader->uniformLocation("spot_lights[" + QString::number(i) + "].linear"));
            locations.insert("spot_lights[" + QString::number(i) + "].quadratic", shader->uniformLocation("spot_lights[" + QString::number(i) + "].quadratic"));
            locations.insert("spot_lights[" + QString::number(i) + "].cut_off_angle", shader->uniformLocation("spot_lights[" + QString::number(i) + "].cut_off_angle"));
            locations.insert("spot_lights[" + QString::number(i) + "].outer_cut_off_angle", shader->uniformLocation("spot_lights[" + QString::number(i) + "].outer_cut_off_angle"));
        }

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

        locations.insert("directional_light.color", shader->uniformLocation("directional_light.color"));
        locations.insert("directional_light.direction", shader->uniformLocation("directional_light.direction"));
        locations.insert("directional_light.ambient", shader->uniformLocation("directional_light.ambient"));
        locations.insert("directional_light.diffuse", shader->uniformLocation("directional_light.diffuse"));
        locations.insert("directional_light.specular", shader->uniformLocation("directional_light.specular"));

        locations.insert("number_of_point_lights", shader->uniformLocation("number_of_point_lights"));
        locations.insert("number_of_spot_lights", shader->uniformLocation("number_of_spot_lights"));

        for (int i = 0; i < 8; i++)
        {
            locations.insert("point_lights[" + QString::number(i) + "].color", shader->uniformLocation("point_lights[" + QString::number(i) + "].color"));
            locations.insert("point_lights[" + QString::number(i) + "].position", shader->uniformLocation("point_lights[" + QString::number(i) + "].position"));
            locations.insert("point_lights[" + QString::number(i) + "].ambient", shader->uniformLocation("point_lights[" + QString::number(i) + "].ambient"));
            locations.insert("point_lights[" + QString::number(i) + "].diffuse", shader->uniformLocation("point_lights[" + QString::number(i) + "].diffuse"));
            locations.insert("point_lights[" + QString::number(i) + "].specular", shader->uniformLocation("point_lights[" + QString::number(i) + "].specular"));
            locations.insert("point_lights[" + QString::number(i) + "].constant", shader->uniformLocation("point_lights[" + QString::number(i) + "].constant"));
            locations.insert("point_lights[" + QString::number(i) + "].linear", shader->uniformLocation("point_lights[" + QString::number(i) + "].linear"));
            locations.insert("point_lights[" + QString::number(i) + "].quadratic", shader->uniformLocation("point_lights[" + QString::number(i) + "].quadratic"));
        }

        for (int i = 0; i < 8; i++)
        {
            locations.insert("spot_lights[" + QString::number(i) + "].color", shader->uniformLocation("spot_lights[" + QString::number(i) + "].color"));
            locations.insert("spot_lights[" + QString::number(i) + "].position", shader->uniformLocation("spot_lights[" + QString::number(i) + "].position"));
            locations.insert("spot_lights[" + QString::number(i) + "].direction", shader->uniformLocation("spot_lights[" + QString::number(i) + "].direction"));
            locations.insert("spot_lights[" + QString::number(i) + "].ambient", shader->uniformLocation("spot_lights[" + QString::number(i) + "].ambient"));
            locations.insert("spot_lights[" + QString::number(i) + "].diffuse", shader->uniformLocation("spot_lights[" + QString::number(i) + "].diffuse"));
            locations.insert("spot_lights[" + QString::number(i) + "].specular", shader->uniformLocation("spot_lights[" + QString::number(i) + "].specular"));
            locations.insert("spot_lights[" + QString::number(i) + "].constant", shader->uniformLocation("spot_lights[" + QString::number(i) + "].constant"));
            locations.insert("spot_lights[" + QString::number(i) + "].linear", shader->uniformLocation("spot_lights[" + QString::number(i) + "].linear"));
            locations.insert("spot_lights[" + QString::number(i) + "].quadratic", shader->uniformLocation("spot_lights[" + QString::number(i) + "].quadratic"));
            locations.insert("spot_lights[" + QString::number(i) + "].cut_off_angle", shader->uniformLocation("spot_lights[" + QString::number(i) + "].cut_off_angle"));
            locations.insert("spot_lights[" + QString::number(i) + "].outer_cut_off_angle", shader->uniformLocation("spot_lights[" + QString::number(i) + "].outer_cut_off_angle"));
        }

        locations.insert("node_transformation", shader->uniformLocation("node_transformation"));
        locations.insert("node_shininess", shader->uniformLocation("node_shininess"));
        locations.insert("camera_position", shader->uniformLocation("camera_position"));
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));
        locations.insert("texture_diffuse", shader->uniformLocation("texture_diffuse"));
        locations.insert("texture_specular", shader->uniformLocation("texture_specular"));

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

        locations.insert("node_matrix", shader->uniformLocation("node_matrix"));
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));

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

        locations.insert("node_matrix", shader->uniformLocation("node_matrix"));
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));

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
        locations.insert("view_matrix", shader->uniformLocation("view_matrix"));
        locations.insert("projection_matrix", shader->uniformLocation("projection_matrix"));

        shader->bindAttributeLocation("position", 0);

        shader->release();

        mLocations.insert(Shader::SkyBoxShader, locations);

        qInfo() << "SkyBoxShader is initialized.";
        qInfo() << "Uniform locations are:" << locations;
    }

    // SimpleTerrainShader
    {
        qInfo() << "SimpleTerrainShader is initializing...";

        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram;
        mPrograms.insert(Shader::SimpleTerrainShader, shader);

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/SimpleTerrain.vert")))
        {
            qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
            return false;
        }

        if (!shader->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/SimpleTerrain.frag")))
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

        shader->release();

        mLocations.insert(Shader::SimpleTerrainShader, locations);

        qInfo() << "SimpleTerrainShader is initialized.";
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
