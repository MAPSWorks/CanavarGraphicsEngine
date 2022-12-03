#include "ShaderManager.h"
#include "Shader.h"

Canavar::Engine::ShaderManager::ShaderManager()
    : Manager()
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

        if (!shader->init())
            return false;
    }

    // Model Textured Shader
    {
        Shader *shader = new Shader(ShaderType::ModelTexturedShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ModelTextured.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ModelTextured.frag");

        if (!shader->init())
            return false;
    }

    // Sky Shader
    {
        Shader *shader = new Shader(ShaderType::SkyShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");

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

        if (!shader->init())
            return false;
    }

    // Blur Shader
    {
        Shader *shader = new Shader(ShaderType::BlurShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Blur.frag");

        if (!shader->init())
            return false;
    }

    // Post Process Shader
    {
        Shader *shader = new Shader(ShaderType::PostProcessShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/PostProcess.frag");

        if (!shader->init())
            return false;
    }

    // Screen Shader
    {
        Shader *shader = new Shader(ShaderType::ScreenShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");

        if (!shader->init())
            return false;
    }

    // Node Info Shader
    {
        Shader *shader = new Shader(ShaderType::NodeInfoShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NodeInfo.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NodeInfo.frag");

        if (!shader->init())
            return false;
    }

    // Nozzle Effect Shader
    {
        Shader *shader = new Shader(ShaderType::NozzleEffectShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");

        if (!shader->init())
            return false;
    }

    // Firecracker Effect Shader
    {
        Shader *shader = new Shader(ShaderType::FirecrackerEffectShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/FirecrackerEffect.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/FirecrackerEffect.frag");

        if (!shader->init())
            return false;
    }

    // Basic Shader
    {
        Shader *shader = new Shader(ShaderType::BasicShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Basic.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Basic.frag");

        if (!shader->init())
            return false;
    }

    // Mesh Vertex Renderer Shader
    {
        Shader *shader = new Shader(ShaderType::MeshVertexRendererShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/MeshVertexRenderer.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/MeshVertexRenderer.frag");

        if (!shader->init())
            return false;
    }

    // Node Info Shader
    {
        Shader *shader = new Shader(ShaderType::VertexInfoShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/MeshVertexRenderer.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NodeInfo.frag");

        if (!shader->init())
            return false;
    }

    // Line Strip Shader
    {
        Shader *shader = new Shader(ShaderType::LineStripShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LineStrip.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LineStrip.frag");

        if (!shader->init())
            return false;
    }

    // Raycaster Shader
    {
        Shader *shader = new Shader(ShaderType::RaycasterShader);
        mShaders.insert(shader->type(), shader);

        shader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Raycaster.vert");
        shader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Raycaster.frag");

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
