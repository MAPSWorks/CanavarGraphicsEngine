#include "Sky.h"
#include "LightManager.h"

Sky::Sky(QObject *parent)
    : QObject{parent}
    , mSkyColorTop(0.5, 0.7, 0.8)
    , mSkyColorBottom(0.9, 0.9, 0.95)
    , mCloudSpeed(450.f)
    , mCoverage(0.45f)
    , mCrispiness(0.1f)
    , mDensity(0.02f)
    , mAbsorption(0.35f)
    , mEarthRadius(600000.0f)
    , mSphereInnerRadius(5000.0f)
    , mSphereOuterRadius(17000.0f)
    , mPerlinFrequency(0.8f)
    , mEnablePower(false)
    , mSeed(0, 0, 0)
    , mCloudColorTop(1, 0, 0)
    , mCloudColorBottom(1, 0, 0)
    , mSkyBoxFramebuffer(nullptr)
    , mTimeElapsed(0.0f)

{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    mQuad = new Quad;
    mQuad->create();

    initializeOpenGLFunctions();

    // Perlin - Worley
    qInfo() << "Generating Perlin Worley 3D Texture 128x128x128...";
    mShaderManager->bind(ShaderManager::ShaderType::PerlinWorleyShader);
    mInputTextures.perlin = new Texture(128, 128, 128);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mInputTextures.perlin->id());
    glBindImageTexture(0, mInputTextures.perlin->id(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
    mShaderManager->setUniformValue("outVolTex", 0);
    glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
    glGenerateMipmap(GL_TEXTURE_3D);
    mShaderManager->release();
    qInfo() << "Perlin Worley 3D Texture 128x128x128 is generated.";

    // Worley
    qInfo() << "Generating Worley 3D Texture 32x32x32...";
    mShaderManager->bind(ShaderManager::ShaderType::WorleyShader);
    mInputTextures.worley = new Texture(32, 32, 32);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mInputTextures.worley->id());
    glBindImageTexture(0, mInputTextures.worley->id(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
    mShaderManager->setUniformValue("outVolTex", 0);
    glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
    glGenerateMipmap(GL_TEXTURE_3D);
    mShaderManager->release();
    qInfo() << "Worley 3D Texture 32x32x32 is generated.";

    // Weather
    qInfo() << "Generating Weather 2D Texture 1024x1024...";
    mShaderManager->bind(ShaderManager::ShaderType::WeatherShader);
    mInputTextures.weather = new Texture(1024, 1024);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mInputTextures.weather->id());
    glBindImageTexture(0, mInputTextures.weather->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    mShaderManager->setUniformValue("outWeatherTex", 0);
    mShaderManager->setUniformValue("seed", mSeed);
    mShaderManager->setUniformValue("perlinFrequency", mPerlinFrequency);
    glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    qInfo() << "Weather 2D Texture 1024x1024 is generated.";

    mOutputTextures.fragColor = new Texture(1600, 900);
    mOutputTextures.bloom = new Texture(1600, 900);
    mOutputTextures.alphaness = new Texture(1600, 900);
    mOutputTextures.cloudDistance = new Texture(1600, 900);

    mSkyBoxFramebuffer = new Framebuffer(1600, 900);
}

void Sky::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    if (mOutputTextures.fragColor)
        mOutputTextures.fragColor->deleteLater();

    if (mOutputTextures.bloom)
        mOutputTextures.bloom->deleteLater();

    if (mOutputTextures.alphaness)
        mOutputTextures.alphaness->deleteLater();

    if (mOutputTextures.cloudDistance)
        mOutputTextures.cloudDistance->deleteLater();

    if (mSkyBoxFramebuffer)
        mSkyBoxFramebuffer->deleteLater();

    mOutputTextures.fragColor = new Texture(mWidth, mHeight);
    mOutputTextures.bloom = new Texture(mWidth, mHeight);
    mOutputTextures.alphaness = new Texture(mWidth, mHeight);
    mOutputTextures.cloudDistance = new Texture(mWidth, mHeight);

    mSkyBoxFramebuffer = new Framebuffer(mWidth, mHeight);
}

void Sky::render(float ifps)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    mTimeElapsed += ifps;

    // Sky
    mSkyBoxFramebuffer->bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mShaderManager->bind(ShaderManager::ShaderType::SkyShader);
    mShaderManager->setUniformValue("skyColorTop", mSkyColorTop);
    mShaderManager->setUniformValue("skyColorBottom", mSkyColorBottom);
    mShaderManager->setUniformValue("lightDirection", mSun->direction());
    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mShaderManager->setUniformValue("inverseProjectionMatrix", mCamera->projection().inverted());
    mShaderManager->setUniformValue("inverseViewMatrix", mCamera->worldTransformation().inverted());
    mQuad->render();
    mShaderManager->release();

    // Clouds

    mShaderManager->bind(ShaderManager::ShaderType::VolumetricCloudsShader);

    glBindImageTexture(0, mOutputTextures.fragColor->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mOutputTextures.bloom->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(2, mOutputTextures.alphaness->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(3, mOutputTextures.cloudDistance->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mShaderManager->setUniformValue("time", mTimeElapsed);
    mShaderManager->setUniformValue("inverseProjectionMatrix", mCamera->projection().inverted());
    mShaderManager->setUniformValue("inverseViewMatrix", mCamera->worldTransformation().inverted());
    mShaderManager->setUniformValue("inverseVP", mCamera->worldTransformation().inverted() * mCamera->projection().inverted());
    mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
    mShaderManager->setUniformValue("FOV", mCamera->verticalFov());
    mShaderManager->setUniformValue("lightDirection", mSun->direction());
    mShaderManager->setUniformValue("lightColor", mSun->color());

    mShaderManager->setUniformValue("coverageMultiplier", mCoverage);
    mShaderManager->setUniformValue("cloudSpeed", mCloudSpeed);
    mShaderManager->setUniformValue("crispiness", mCrispiness);
    mShaderManager->setUniformValue("curliness", mCurliness);
    mShaderManager->setUniformValue("absorption", mAbsorption);
    mShaderManager->setUniformValue("densityFactor", mDensity);

    mShaderManager->setUniformValue("earthRadius", mEarthRadius);
    mShaderManager->setUniformValue("sphereInnerRadius", mSphereInnerRadius);
    mShaderManager->setUniformValue("sphereOuterRadius", mSphereOuterRadius);

    mShaderManager->setUniformValue("cloudColorTop", mCloudColorTop);
    mShaderManager->setUniformValue("cloudColorBottom", mCloudColorBottom);

    mShaderManager->setUniformValue("skyColorTop", mSkyColorTop);
    mShaderManager->setUniformValue("skyColorBottom", mSkyColorBottom);

    mShaderManager->setUniformValue("skyColorTop", mSkyColorTop);
    mShaderManager->setUniformValue("skyColorBottom", mSkyColorBottom);

    // Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mInputTextures.perlin->id());
    mShaderManager->setUniformValue("cloud", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, mInputTextures.worley->id());
    mShaderManager->setUniformValue("worley32", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mInputTextures.weather->id());
    mShaderManager->setUniformValue("weatherTex", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mSkyBoxFramebuffer->texture());
    mShaderManager->setUniformValue("sky", 3);

    glDispatchCompute(INT_CEIL(mWidth, 16), INT_CEIL(mHeight, 16), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    mShaderManager->release();

    glEnable(GL_DEPTH_TEST);
}

void Sky::drawGui() {}

void Sky::setSun(DirectionalLight *newSun)
{
    mSun = newSun;
}

const Sky::OutputTextureSet &Sky::outputTextures() const
{
    return mOutputTextures;
}

Framebuffer *Sky::skyBoxFramebuffer() const
{
    return mSkyBoxFramebuffer;
}
