#include "Sky.h"
#include "LightManager.h"
#include "RandomGenerator.h"

Sky::Sky(QObject *parent)
    : QObject{parent}
    , mSkyColorTop(0.5, 0.7, 0.8)
    , mSkyColorBottom(0.9, 0.9, 0.95)
    , mCloudColor(0.2f, 0.2f, 0.2f)
    , mCloudFlareColor(0.4, 0.3, 0.15)
    , mCloudSpeed(250.f)
    , mCoverage(0.35f)
    , mCrispiness(40.0f)
    , mCurliness(0.1f)
    , mDensity(0.02f)
    , mAbsorption(0.0035f)
    , mEarthRadius(600000.0f)
    , mSphereInnerRadius(5000.0f)
    , mSphereOuterRadius(17000.0f)
    , mPerlinFrequency(0.8f)
    , mEnablePower(false)
    , mSeed(0, 0, 0)
    , mSkyFramebuffer(nullptr)
    , mTimeElapsed(0.0f)

{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mHaze = Haze::instance();
    mQuad = Quad::instance();

    mOutputTextures.fragColor = new Texture(1600, 900);
    mOutputTextures.bloom = new Texture(1600, 900);
    mOutputTextures.alphaness = new Texture(1600, 900);
    mOutputTextures.cloudDistance = new Texture(1600, 900);

    mSkyFramebufferFormat.setSamples(0);
    mSkyFramebufferFormat.addColorAttachment(0, //
                                             FramebufferFormat::TextureTarget::TEXTURE_2D,
                                             FramebufferFormat::TextureInternalFormat::RGBA8,
                                             FramebufferFormat::TexturePixelFormat::RGBA,
                                             FramebufferFormat::TextureDataType::UNSIGNED_BYTE);
    mSkyFramebufferFormat.setWidth(1600);
    mSkyFramebufferFormat.setHeight(900);

    mSkyFramebuffer = new Framebuffer(mSkyFramebufferFormat);

    initializeOpenGLFunctions();
    generateMaps();
}

Sky *Sky::instance()
{
    static Sky sky;
    return &sky;
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

    if (mSkyFramebuffer)
        mSkyFramebuffer->deleteLater();

    mOutputTextures.fragColor = new Texture(mWidth, mHeight);
    mOutputTextures.bloom = new Texture(mWidth, mHeight);
    mOutputTextures.alphaness = new Texture(mWidth, mHeight);
    mOutputTextures.cloudDistance = new Texture(mWidth, mHeight);

    mSkyFramebufferFormat.setWidth(mWidth);
    mSkyFramebufferFormat.setHeight(mHeight);
    mSkyFramebuffer = new Framebuffer(mSkyFramebufferFormat);
}

void Sky::render(const RenderSettings &settings)
{
    glDepthFunc(GL_LEQUAL);
    mShaderManager->bind(ShaderManager::ShaderType::SkyPostProcessingShader);
    mShaderManager->setSampler("skyTexture", 0, mOutputTextures.fragColor->id());
    mShaderManager->setUniformValue("width", mWidth);
    mShaderManager->setUniformValue("height", mHeight);
    mQuad->render();
    mShaderManager->release();
    glDepthFunc(GL_LESS);
}

void Sky::renderWeather(const RenderSettings &settings)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    mTimeElapsed += settings.ifps;

    glDisable(GL_MULTISAMPLE);

    mSkyFramebuffer->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Sky
    mShaderManager->bind(ShaderManager::ShaderType::SkyShader);
    mShaderManager->setUniformValue("skyColorTop", mSkyColorTop);
    mShaderManager->setUniformValue("skyColorBottom", mSkyColorBottom);
    mShaderManager->setUniformValue("lightDirection", mSun->direction());
    mShaderManager->setUniformValue("lightColor", mSun->color().toVector3D());
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
    mShaderManager->setUniformValue("lightDirection", -mSun->direction());
    mShaderManager->setUniformValue("lightColor", mSun->color().toVector3D());
    mShaderManager->setUniformValue("coverageMultiplier", mCoverage);
    mShaderManager->setUniformValue("cloudSpeed", mCloudSpeed);
    mShaderManager->setUniformValue("crispiness", mCrispiness);
    mShaderManager->setUniformValue("curliness", mCurliness);
    mShaderManager->setUniformValue("absorption", mAbsorption);
    mShaderManager->setUniformValue("densityFactor", mDensity);
    mShaderManager->setUniformValue("earthRadius", mEarthRadius);
    mShaderManager->setUniformValue("sphereInnerRadius", mSphereInnerRadius);
    mShaderManager->setUniformValue("sphereOuterRadius", mSphereOuterRadius);
    mShaderManager->setUniformValue("cloudColor", mCloudColor);
    mShaderManager->setUniformValue("cloudFlareColor", mCloudFlareColor);

    mShaderManager->setUniformValue("haze.enabled", mHaze->enabled());
    mShaderManager->setUniformValue("haze.color", mHaze->color());
    mShaderManager->setUniformValue("haze.density", mHaze->density());
    mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());

    // Textures
    mShaderManager->setSampler("perlin", 0, mInputTextures.perlin->id(), GL_TEXTURE_3D);
    mShaderManager->setSampler("worley", 1, mInputTextures.worley->id(), GL_TEXTURE_3D);
    mShaderManager->setSampler("weather", 2, mInputTextures.weather->id(), GL_TEXTURE_2D);
    mShaderManager->setSampler("sky", 3, mSkyFramebuffer->texture(), GL_TEXTURE_2D);

    glDispatchCompute(INT_CEIL(mWidth, 16), INT_CEIL(mHeight, 16), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    mShaderManager->release();

    glEnable(GL_MULTISAMPLE);
}

void Sky::drawGUI()
{
    // Render Settings
    if (!ImGui::CollapsingHeader("Sky##Sky"))
    {
        ImGui::Text("Clouds rendering");

        ImGui::SliderFloat("Coverage##Sky", &mCoverage, 0.0f, 1.0f);
        ImGui::SliderFloat("Speed##Sky", &mCloudSpeed, 0.0f, 5.0E3);
        ImGui::SliderFloat("Crispiness##Sky", &mCrispiness, 0.0f, 120.0f);
        ImGui::SliderFloat("Curliness##Sky", &mCurliness, 0.0f, 3.0f);
        ImGui::SliderFloat("Density##Sky", &mDensity, 0.0f, 0.1f);
        ImGui::SliderFloat("Light absorption##Sky", &mAbsorption, 0.0f, 0.01f, "%.6f");
        if (ImGui::SliderFloat("Clouds frequency##Sky", &mPerlinFrequency, 0.0f, 4.0f))
        {}

        ImGui::Checkbox("Enable sugar powder effect##Sky", &mEnablePower);

        ImGui::Text("Dome controls");
        ImGui::SliderFloat("Sky dome radius##Sky", &mEarthRadius, 10000.0f, 5000000.0f);
        ImGui::SliderFloat("Clouds bottom height##Sky", &mSphereInnerRadius, 1000.0f, 15000.0f);
        ImGui::SliderFloat("Clouds top height##Sky", &mSphereOuterRadius, 1000.0f, 40000.0f);

        ImGui::Text("Clouds colors");
        ImGui::ColorEdit3("Cloud color##Sky", (float *) &mCloudColor);
        ImGui::ColorEdit3("Cloud flare color##Sky", (float *) &mCloudFlareColor);

        if (ImGui::Button("Generate Seed##Sky"))
        {
            mSeed = RandomGenerator::getRandomVec3();
            generateMaps();
        }

        if (ImGui::Button("Reset##Sky"))
        {
            reset();
            generateMaps();
        }
    }
}

void Sky::reset()
{
    mSkyColorTop = QVector3D(0.5, 0.7, 0.8);
    mSkyColorBottom = QVector3D(0.9, 0.9, 0.95);
    mCloudColor = QVector3D(0.2f, 0.2f, 0.2f);
    mCloudFlareColor = QVector3D(0.4, 0.3, 0.15);
    mCloudSpeed = 450.f;
    mCoverage = 0.25f;
    mCrispiness = 40.0f;
    mCurliness = 0.1f;
    mDensity = 0.02f;
    mAbsorption = 0.0035f;
    mEarthRadius = 600000.0f;
    mSphereInnerRadius = 5000.0f;
    mSphereOuterRadius = 17000.0f;
    mPerlinFrequency = 0.8f;
    mEnablePower = false;
    mSeed = QVector3D(0, 0, 0);
    mTimeElapsed = 0.0f;
}

void Sky::generateMaps()
{
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
}
