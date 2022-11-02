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
    // Model Colored Shader
    {
        Shader *shader = new Shader(ShaderType::ModelColoredShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ModelColored.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ModelColored.frag");

        QStringList uniforms;
        uniforms << "M"
                 << "VP"
                 << "sun.color"
                 << "sun.ambient"
                 << "sun.diffuse"
                 << "sun.specular"
                 << "sun.direction"
                 << "model.color"
                 << "model.overlayColor"
                 << "model.overlayColorFactor"
                 << "model.meshOverlayColor"
                 << "model.meshOverlayColorFactor"
                 << "model.ambient"
                 << "model.diffuse"
                 << "model.specular"
                 << "model.shininess"
                 << "haze.enabled"
                 << "haze.color"
                 << "haze.density"
                 << "haze.gradient"
                 << "cameraPos"
                 << "numberOfPointLights";

        shader->addUniforms(uniforms);

        shader->setUniformArray("pointLights[%1].color", 8);
        shader->setUniformArray("pointLights[%1].position", 8);
        shader->setUniformArray("pointLights[%1].ambient", 8);
        shader->setUniformArray("pointLights[%1].diffuse", 8);
        shader->setUniformArray("pointLights[%1].specular", 8);
        shader->setUniformArray("pointLights[%1].constant", 8);
        shader->setUniformArray("pointLights[%1].linear", 8);
        shader->setUniformArray("pointLights[%1].quadratic", 8);

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

    // Model Textured Shader
    {
        Shader *shader = new Shader(ShaderType::ModelTexturedShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ModelTextured.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ModelTextured.frag");

        QStringList uniforms;
        uniforms << "M"
                 << "N"
                 << "VP"
                 << "sun.color"
                 << "sun.ambient"
                 << "sun.diffuse"
                 << "sun.specular"
                 << "sun.direction"
                 << "model.overlayColor"
                 << "model.overlayColorFactor"
                 << "model.meshOverlayColor"
                 << "model.meshOverlayColorFactor"
                 << "model.ambient"
                 << "model.diffuse"
                 << "model.specular"
                 << "model.shininess"
                 << "cameraPos"
                 << "useTextureNormal"
                 << "useTextureSpecular"
                 << "useTextureDiffuse"
                 << "useTextureAmbient"
                 << "textureNormal"
                 << "textureSpecular"
                 << "textureDiffuse"
                 << "textureAmbient"
                 << "haze.enabled"
                 << "haze.color"
                 << "haze.density"
                 << "haze.gradient"
                 << "numberOfPointLights";

        shader->addUniforms(uniforms);

        shader->setUniformArray("pointLights[%1].color", 8);
        shader->setUniformArray("pointLights[%1].position", 8);
        shader->setUniformArray("pointLights[%1].ambient", 8);
        shader->setUniformArray("pointLights[%1].diffuse", 8);
        shader->setUniformArray("pointLights[%1].specular", 8);
        shader->setUniformArray("pointLights[%1].constant", 8);
        shader->setUniformArray("pointLights[%1].linear", 8);
        shader->setUniformArray("pointLights[%1].quadratic", 8);

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

    // Sky Shader
    {
        Shader *shader = new Shader(ShaderType::SkyShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");

        shader->addUniform("IVP"); // Inverse view-projection matrix
        shader->addUniform("A");
        shader->addUniform("B");
        shader->addUniform("C");
        shader->addUniform("D");
        shader->addUniform("E");
        shader->addUniform("F");
        shader->addUniform("G");
        shader->addUniform("H");
        shader->addUniform("I");
        shader->addUniform("Z");
        shader->addUniform("sunDir");

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
                 << "sun.color"
                 << "sun.direction"
                 << "sun.ambient"
                 << "sun.diffuse"
                 << "sun.specular"
                 << "waterHeight"
                 << "cameraPos"
                 << "M"
                 << "VP"
                 << "sand"
                 << "grass"
                 << "terrainTexture"
                 << "snow"
                 << "rock"
                 << "rockNormal"
                 << "numberOfPointLights";

        shader->addUniforms(uniforms);

        shader->setUniformArray("pointLights[%1].color", 8);
        shader->setUniformArray("pointLights[%1].position", 8);
        shader->setUniformArray("pointLights[%1].ambient", 8);
        shader->setUniformArray("pointLights[%1].diffuse", 8);
        shader->setUniformArray("pointLights[%1].specular", 8);
        shader->setUniformArray("pointLights[%1].constant", 8);
        shader->setUniformArray("pointLights[%1].linear", 8);
        shader->setUniformArray("pointLights[%1].quadratic", 8);

        shader->addAttribute("position");
        shader->addAttribute("normal");
        shader->addAttribute("textureCoord");
        shader->addAttribute("offsetPosition");

        if (!shader->init())
            return false;
    }

    // Blur Shader
    {
        Shader *shader = new Shader(ShaderType::BlurShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Blur.frag");

        shader->addUniform("screenTexture");
        shader->addUniform("horizontal");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

        if (!shader->init())
            return false;
    }

    // Post Process Shader
    {
        Shader *shader = new Shader(ShaderType::PostProcessShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/PostProcess.frag");

        shader->addUniform("sceneTexture");
        shader->addUniform("bloomBlurTexture");
        shader->addUniform("exposure");
        shader->addUniform("gamma");

        shader->addAttribute("position");
        shader->addAttribute("textureCoords");

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

    // Node Info Shader
    {
        Shader *shader = new Shader(ShaderType::NodeInfoShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NodeInfo.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NodeInfo.frag");

        shader->addUniform("MVP");
        shader->addUniform("nodeID");
        shader->addUniform("meshID");

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

    // Nozzle Effect Shader
    {
        Shader *shader = new Shader(ShaderType::NozzleEffectShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");

        shader->addUniform("MVP");
        shader->addUniform("scale");
        shader->addUniform("speed");
        shader->addUniform("maxRadius");
        shader->addUniform("maxDistance");

        shader->addAttribute("vertexPosition");
        shader->addAttribute("initialPosition");
        shader->addAttribute("direction");
        shader->addAttribute("life");

        if (!shader->init())
            return false;
    }

    // Firecracker Effect Shader
    {
        Shader *shader = new Shader(ShaderType::FirecrackerEffectShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/FirecrackerEffect.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/FirecrackerEffect.frag");

        shader->addUniform("MVP");
        shader->addUniform("scale");
        shader->addUniform("maxLife");

        shader->addAttribute("cubeVertexPosition");
        shader->addAttribute("position");
        shader->addAttribute("life");
        shader->addAttribute("deadAfter");

        if (!shader->init())
            return false;
    }

    // Basic Shader
    {
        Shader *shader = new Shader(ShaderType::BasicShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Basic.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Basic.frag");

        shader->addUniform("MVP");
        shader->addUniform("color");

        shader->addAttribute("position");

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
