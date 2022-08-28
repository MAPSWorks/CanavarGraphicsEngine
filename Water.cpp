#include "Water.h"
#include "LightManager.h"

Water::Water(QObject *parent)
    : QObject(parent)
    , mWaterHeight(120.0f)
    , mWaveSpeed(0.25f)
    , mTimeElapsed(0.0f)
    , mReflectionFramebuffer(nullptr)
    , mRefractionFramebuffer(nullptr)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    mFramebufferFormat.setSamples(0);
    mFramebufferFormat.addColorAttachment(0, //
                                          FramebufferFormat::TextureTarget::TEXTURE_2D,
                                          FramebufferFormat::TextureInternalFormat::RGBA8,
                                          FramebufferFormat::TexturePixelFormat::RGBA,
                                          FramebufferFormat::TextureDataType::UNSIGNED_BYTE);
    mFramebufferFormat.setWidth(1600);
    mFramebufferFormat.setHeight(900);

    initializeOpenGLFunctions();
    createFramebuffers();
}

Water *Water::instance()
{
    static Water water;
    return &water;
}

void Water::create()
{
    mTileGenerator = new TileGenerator(3, 128, 1024.0f);
    mTileGenerator->generate();
}

void Water::render(float ifps)
{
    QVector2D currentTilePosition = mTileGenerator->whichTile(mCameraManager->activeCamera()->worldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->translateTiles(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;
    }

    mTimeElapsed += ifps;

    mTransformation.setColumn(3, QVector4D(0, mWaterHeight, 0, 1));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_MULTISAMPLE);

    mShaderManager->bind(ShaderManager::ShaderType::WaterShader);
    mShaderManager->setUniformValue("nodeMatrix", mTransformation);
    mShaderManager->setUniformValue("cameraPosition", mCameraManager->activeCamera()->worldPosition());
    mShaderManager->setUniformValue("VP", mCameraManager->activeCamera()->getVP());
    mShaderManager->setUniformValue("lightColor", mLightManager->directionalLight()->color().toVector3D());
    mShaderManager->setUniformValue("lightDirection", mLightManager->directionalLight()->direction());
    mShaderManager->setUniformValue("moveFactor", mWaveSpeed * mTimeElapsed);
    mShaderManager->setSampler("reflectionTex", 0, mReflectionFramebuffer->texture());
    mShaderManager->setSampler("refractionTex", 1, mRefractionFramebuffer->texture());
    mTileGenerator->render(Primitive::Triangles);
    mShaderManager->release();

    glDisable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
}

void Water::resize(int width, int height)
{
    mFramebufferFormat.setWidth(width);
    mFramebufferFormat.setHeight(height);
    deleteFramebuffers();
    createFramebuffers();
}

void Water::drawGUI()
{
    if (!ImGui::CollapsingHeader("Water##Water"))
    {
        ImGui::SliderFloat("Water Height##Water", &mWaterHeight, -1000, 1000);
        ImGui::SliderFloat("Wave Speed##Water", &mWaveSpeed, 0, 10);
    }
}

void Water::createFramebuffers()
{
    mReflectionFramebuffer = new Framebuffer(mFramebufferFormat);
    mRefractionFramebuffer = new Framebuffer(mFramebufferFormat);
}

void Water::deleteFramebuffers()
{
    if (mReflectionFramebuffer)
        mReflectionFramebuffer->deleteLater();

    if (mRefractionFramebuffer)
        mRefractionFramebuffer->deleteLater();
}

Framebuffer *Water::refractionFramebuffer() const
{
    return mRefractionFramebuffer;
}

Framebuffer *Water::reflectionFramebuffer() const
{
    return mReflectionFramebuffer;
}

float Water::waterHeight() const
{
    return mWaterHeight;
}

void Water::setHaze(Haze *newHaze)
{
    mHaze = newHaze;
}
