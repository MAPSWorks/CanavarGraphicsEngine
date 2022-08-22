#include "ShaderManager.h"
#include "Shader.h"

ShaderManager::ShaderManager(QObject *parent)
    : QObject{parent}
    , mActiveShader(ShaderType::None)
{}

Shader *ShaderManager::getShader(ShaderType shader)
{
    return mShaders.value(shader);
}

bool ShaderManager::init()
{
    // Model Shader
    {
        Shader *shader = new Shader(ShaderType::ModelShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Model.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Model.frag");

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
                 << "cameraPosition"
                 << "VP"
                 << "haze.enabled"
                 << "haze.color"
                 << "haze.density"
                 << "haze.gradient"
                 << "textureAmbient"
                 << "textureDiffuse"
                 << "textureSpecular"
                 << "textureNormal"
                 << "useTexture"
                 << "useTextureAmbient"
                 << "useTextureDiffuse"
                 << "useTextureSpecular"
                 << "useTextureNormal"
                 << "useBlinnShading";

        shader->addUniforms(uniforms);
        shader->setUniformArray("pointLights[%1].color", 8);
        shader->setUniformArray("pointLights[%1].position", 8);
        shader->setUniformArray("pointLights[%1].ambient", 8);
        shader->setUniformArray("pointLights[%1].diffuse", 8);
        shader->setUniformArray("pointLights[%1].specular", 8);
        shader->setUniformArray("pointLights[%1].constant", 8);
        shader->setUniformArray("pointLights[%1].linear", 8);
        shader->setUniformArray("pointLights[%1].quadratic", 8);

        shader->setUniformArray("spotLights[%1].color", 8);
        shader->setUniformArray("spotLights[%1].position", 8);
        shader->setUniformArray("spotLights[%1].ambient", 8);
        shader->setUniformArray("spotLights[%1].diffuse", 8);
        shader->setUniformArray("spotLights[%1].specular", 8);
        shader->setUniformArray("spotLights[%1].constant", 8);
        shader->setUniformArray("spotLights[%1].linear", 8);
        shader->setUniformArray("spotLights[%1].quadratic", 8);
        shader->setUniformArray("spotLights[%1].cutOffAngle", 8);
        shader->setUniformArray("spotLights[%1].outerCutOffAngle", 8);

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

    // Wireframe Shader
    {
        Shader *shader = new Shader(ShaderType::WireframeShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Wireframe.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Wireframe.frag");
        shader->addPath(QOpenGLShader::Geometry, ":/Resources/Shaders/Wireframe.geom");

        shader->addUniform("nodeMatrix");
        shader->addUniform("viewMatrix");
        shader->addUniform("projectionMatrix");

        shader->addAttribute("position");

        if (!shader->init())
            return false;
    }

    // Normals Shader
    {
        Shader *shader = new Shader(ShaderType::NormalsShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Normals.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Normals.frag");
        shader->addPath(QOpenGLShader::Geometry, ":/Resources/Shaders/Normals.geom");

        shader->addUniform("nodeMatrix");
        shader->addUniform("viewMatrix");
        shader->addUniform("projectionMatrix");

        shader->addAttribute("position");
        shader->addAttribute("normal");

        if (!shader->init())
            return false;
    }

    // SkyBox Shader
    {
        Shader *shader = new Shader(ShaderType::SkyBoxShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/SkyBox.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/SkyBox.frag");

        shader->addUniform("skybox");
        shader->addUniform("viewMatrix");
        shader->addUniform("projectionMatrix");

        shader->addAttribute("position");

        if (!shader->init())
            return false;
    }

    // Terrain Shader
    {
        Shader *shader = new Shader(ShaderType::TerrainShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
        shader->addPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
        shader->addPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");

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
                 << "haze.enabled"
                 << "haze.color"
                 << "haze.density"
                 << "haze.gradient"
                 << "directionalLight.color"
                 << "directionalLight.direction"
                 << "directionalLight.ambient"
                 << "directionalLight.diffuse"
                 << "directionalLight.specular"
                 << "waterHeight"
                 << "cameraPosition"
                 << "nodeMatrix"
                 << "VP"
                 << "sand"
                 << "grass"
                 << "terrainTexture"
                 << "snow"
                 << "rock"
                 << "rockNormal";

        shader->addUniforms(uniforms);

        shader->addAttribute("position");
        shader->addAttribute("normal");
        shader->addAttribute("textureCoord");
        shader->addAttribute("offsetPosition");

        if (!shader->init())
            return false;
    }

    // Screen Shader
    {
        Shader *shader = new Shader(ShaderType::ScreenShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");

        shader->addUniform("screenTexture");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

        if (!shader->init())
            return false;
    }

    // Nozzle Particles Shader
    {
        Shader *shader = new Shader(ShaderType::NozzleEffectShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");

        shader->addUniform("screenTexture");
        shader->addUniform("MVP");
        shader->addUniform("velocity");
        shader->addUniform("radius");

        shader->addAttribute("vertexPosition");
        shader->addAttribute("initialPosition");
        shader->addAttribute("velocityDirection");
        shader->addAttribute("life");

        if (!shader->init())
            return false;
    }

    // Nozzle Blur Shader
    {
        Shader *shader = new Shader(ShaderType::NozzleBlurShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleBlur.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleBlur.frag");

        shader->addUniform("screenTexture");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

        if (!shader->init())
            return false;
    }

    // Sky Shader
    {
        Shader *shader = new Shader(ShaderType::SkyShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");

        shader->addUniform("skyColorTop");
        shader->addUniform("skyColorBottom");
        shader->addUniform("lightDirection");
        shader->addUniform("width");
        shader->addUniform("height");
        shader->addUniform("inverseProjectionMatrix");
        shader->addUniform("inverseViewMatrix");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

        if (!shader->init())
            return false;
    }

    // Motion Blur Shader
    {
        Shader *shader = new Shader(ShaderType::MotionBlurShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/MotionBlur.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/MotionBlur.frag");

        shader->addUniform("depthTexture");
        shader->addUniform("colorTexture");
        shader->addUniform("inverseVP");
        shader->addUniform("previousVP");
        shader->addUniform("width");
        shader->addUniform("height");
        shader->addUniform("samples");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

        if (!shader->init())
            return false;
    }

    return true;
}

bool ShaderManager::bind(ShaderType shader)
{
    mActiveShader = shader;
    return mShaders.value(mActiveShader)->bind();
}

void ShaderManager::release()
{
    mShaders.value(mActiveShader)->release();
}

void ShaderManager::setUniformValue(const QString &name, int value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, unsigned int value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, float value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, const QVector3D &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, const QVector4D &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValue(const QString &name, const QMatrix3x3 &value)
{
    mShaders.value(mActiveShader)->setUniformValue(name, value);
}

void ShaderManager::setUniformValueArray(const QString &name, const QVector<QVector3D> &values)
{
    mShaders.value(mActiveShader)->setUniformValueArray(name, values);
}

ShaderManager *ShaderManager::instance()
{
    static ShaderManager instance;

    return &instance;
}
