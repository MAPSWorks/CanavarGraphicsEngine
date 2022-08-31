#include "Terrain.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "RandomGenerator.h"
#include "ShaderManager.h"
#include "Water.h"

#include <QMatrix4x4>

Terrain::Terrain(QObject *parent)
    : QObject(parent)
{
    initializeOpenGLFunctions();

    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mHaze = Haze::instance();
    mWater = Water::instance();

    mTileGenerator = new TileGenerator(3, 128, 1024.0f);

    mTextureSand = new Texture(Texture::Type::Texture2D, "Resources/Terrain/sand.jpg");
    mTextureGrass = new Texture(Texture::Type::Texture2D, "Resources/Terrain/grass0.jpg");
    mTextureSnow = new Texture(Texture::Type::Texture2D, "Resources/Terrain/snow0.jpg");
    mTextureRockDiffuse = new Texture(Texture::Type::Texture2D, "Resources/Terrain/rock0.jpg");
    mTextureRockNormal = new Texture(Texture::Type::Texture2D, "Resources/Terrain/rnormal.jpg");
    mTextureTerrain = new Texture(Texture::Type::Texture2D, "Resources/Terrain/terrain.jpg");

    mTransformation.scale(QVector3D(1, 0, 1));
    reset();
}

void Terrain::render(const RenderSettings &settings)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();

    QVector2D currentTilePosition = mTileGenerator->whichTile(mCameraManager->activeCamera()->worldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->translateTiles(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;
    }

    if ((int) settings.renderFor <= 1)
        glEnable(GL_CLIP_DISTANCE0);

    mShaderManager->bind(ShaderManager::ShaderType::TerrainShader);
    mShaderManager->setUniformValue("clipPlane", QVector4D(0, 1, 0, -mWater->waterHeight()) * (int) settings.renderFor);
    mShaderManager->setUniformValue("VP", mCamera->getVP());
    mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
    mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
    mShaderManager->setUniformValue("directionalLight.color", mSun->color());
    mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
    mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
    mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
    mShaderManager->setUniformValue("nodeMatrix", mTransformation);
    mShaderManager->setUniformValue("terrain.amplitude", mProperties.amplitude);
    mShaderManager->setUniformValue("terrain.seed", mProperties.seed);
    mShaderManager->setUniformValue("terrain.octaves", mProperties.octaves);
    mShaderManager->setUniformValue("terrain.frequency", mProperties.frequency);
    mShaderManager->setUniformValue("terrain.tessellationMultiplier", mProperties.tessellationMultiplier);
    mShaderManager->setUniformValue("terrain.power", mProperties.power);
    mShaderManager->setUniformValue("terrain.grassCoverage", mProperties.grassCoverage);
    mShaderManager->setUniformValue("terrain.ambient", mMaterial.ambient);
    mShaderManager->setUniformValue("terrain.diffuse", mMaterial.diffuse);
    mShaderManager->setUniformValue("terrain.shininess", mMaterial.shininess);
    mShaderManager->setUniformValue("terrain.specular", mMaterial.specular);
    mShaderManager->setUniformValue("haze.enabled", settings.renderFor == RenderFor::Refraction ? false : mHaze->enabled());
    mShaderManager->setUniformValue("haze.color", mHaze->color());
    mShaderManager->setUniformValue("haze.density", mHaze->density());
    mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());
    mShaderManager->setUniformValue("waterHeight", mWater->waterHeight());
    mShaderManager->setSampler("sand", 1, mTextureSand->id());
    mShaderManager->setSampler("grass", 2, mTextureGrass->id());
    mShaderManager->setSampler("terrainTexture", 3, mTextureTerrain->id());
    mShaderManager->setSampler("snow", 4, mTextureSnow->id());
    mShaderManager->setSampler("rock", 5, mTextureRockDiffuse->id());
    mShaderManager->setSampler("rockNormal", 6, mTextureRockNormal->id());
    mTileGenerator->render(GL_PATCHES);
    mShaderManager->release();

    //glDisable(GL_CLIP_DISTANCE0);
}

void Terrain::reset()
{
    mProperties.octaves = 13;
    mProperties.frequency = 0.01f;
    mProperties.tessellationMultiplier = 1.0f;
    mProperties.amplitude = 20.0f;
    mProperties.power = 3.0f;
    mProperties.grassCoverage = 0.45f;
    mProperties.seed = QVector3D(1, 1, 1);

    mMaterial.ambient = 0.55f;
    mMaterial.diffuse = 0.75f;
    mMaterial.shininess = 4.0f;
    mMaterial.specular = 0.05f;
}

void Terrain::drawGUI()
{
    if (!ImGui::CollapsingHeader("Terrain"))
    {
        ImGui::SliderFloat("Amplitude##Terrain", &mProperties.amplitude, 0.0f, 50.0f, "%.3f");
        ImGui::SliderInt("Octaves##Terrain", &mProperties.octaves, 1, 20);
        ImGui::SliderFloat("Power##Terrain", &mProperties.power, 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Tessellation Multiplier##Terrain", &mProperties.tessellationMultiplier, 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Grass Coverage##Terrain", &mProperties.grassCoverage, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Ambient##Terrain", &mMaterial.ambient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Terrain", &mMaterial.diffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Terrain", &mMaterial.specular, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Terrain", &mMaterial.shininess, 0.1f, 128.0f, "%.3f");

        if (ImGui::Button("Generate Seed##Terrain"))
            mProperties.seed = RandomGenerator::getRandomVec3();

        if (ImGui::Button("Reset##Terrain"))
            reset();
    }
}

Terrain *Terrain::instance()
{
    static Terrain instance;
    return &instance;
}
