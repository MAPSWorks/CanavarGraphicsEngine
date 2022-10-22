#include "Terrain.h"
#include "CameraManager.h"
#include "Haze.h"
#include "LightManager.h"
#include "ShaderManager.h"

#include "Helper.h"
#include "Sun.h"

#include <QMatrix4x4>

Canavar::Engine::Terrain::Terrain(QObject *parent)
    : Node(parent)
{
    mType = Node::NodeType::Terrain;
    mName = "Terrain";

    initializeOpenGLFunctions();

    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();

    reset();

    mTileGenerator = new TileGenerator(3, 128, 1024.0f);

    mTextures.insert("Sand", Helper::createTexture("Resources/Terrain/sand.jpg"));
    mTextures.insert("Grass", Helper::createTexture("Resources/Terrain/grass0.jpg"));
    mTextures.insert("Snow", Helper::createTexture("Resources/Terrain/snow0.jpg"));
    mTextures.insert("RockDiffuse", Helper::createTexture("Resources/Terrain/rock0.jpg"));
    mTextures.insert("RockNormal", Helper::createTexture("Resources/Terrain/rnormal.jpg"));
    mTextures.insert("Terrain", Helper::createTexture("Resources/Terrain/terrain.jpg"));

    setScale(QVector3D(1, 0, 1));
}

void Canavar::Engine::Terrain::render(const RenderParameters &)
{
    auto camera = mCameraManager->activeCamera();

    QVector2D currentTilePosition = mTileGenerator->whichTile(camera->worldPosition());
    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->translateTiles(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;
    }

    mShaderManager->bind(ShaderType::TerrainShader);
    mShaderManager->setUniformValue("VP", camera->getViewProjectionMatrix());
    mShaderManager->setUniformValue("cameraPosition", camera->worldPosition());
    mShaderManager->setUniformValue("sun.direction", -Sun::instance()->getDirection().normalized());
    mShaderManager->setUniformValue("sun.color", Sun::instance()->getColor());
    mShaderManager->setUniformValue("sun.ambient", Sun::instance()->getAmbient());
    mShaderManager->setUniformValue("sun.diffuse", Sun::instance()->getDiffuse());
    mShaderManager->setUniformValue("sun.specular", Sun::instance()->getSpecular());
    mShaderManager->setUniformValue("M", worldTransformation());
    mShaderManager->setUniformValue("terrain.amplitude", mAmplitude);
    mShaderManager->setUniformValue("terrain.seed", mSeed);
    mShaderManager->setUniformValue("terrain.octaves", mOctaves);
    mShaderManager->setUniformValue("terrain.frequency", mFrequency);
    mShaderManager->setUniformValue("terrain.tessellationMultiplier", mTessellationMultiplier);
    mShaderManager->setUniformValue("terrain.power", mPower);
    mShaderManager->setUniformValue("terrain.grassCoverage", mGrassCoverage);
    mShaderManager->setUniformValue("terrain.ambient", mAmbient);
    mShaderManager->setUniformValue("terrain.diffuse", mDiffuse);
    mShaderManager->setUniformValue("terrain.shininess", mShininess);
    mShaderManager->setUniformValue("terrain.specular", mSpecular);
    mShaderManager->setUniformValue("haze.enabled", Haze::instance()->getEnabled());
    mShaderManager->setUniformValue("haze.color", Haze::instance()->getColor());
    mShaderManager->setUniformValue("haze.density", Haze::instance()->getDensity());
    mShaderManager->setUniformValue("haze.gradient", Haze::instance()->getGradient());
    mShaderManager->setUniformValue("waterHeight", -1000.0f);
    mShaderManager->setSampler("sand", 1, mTextures.value("Sand")->textureId());
    mShaderManager->setSampler("grass", 2, mTextures.value("Grass")->textureId());
    mShaderManager->setSampler("terrainTexture", 3, mTextures.value("Terrain")->textureId());
    mShaderManager->setSampler("snow", 4, mTextures.value("Snow")->textureId());
    mShaderManager->setSampler("rock", 5, mTextures.value("RockDiffuse")->textureId());
    mShaderManager->setSampler("rockNormal", 6, mTextures.value("RockNormal")->textureId());

    auto closePointLights = Helper::getClosePointLights(mLightManager->getPointLights(), camera->worldPosition(), 8);

    mShaderManager->setUniformValue("numberOfPointLights", (int) closePointLights.size());

    for (int i = 0; i < closePointLights.size(); i++)
    {
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].color", closePointLights[i]->getColor());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].position", closePointLights[i]->worldPosition());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].ambient", closePointLights[i]->getAmbient());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].diffuse", closePointLights[i]->getDiffuse());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].specular", closePointLights[i]->getSpecular());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].constant", closePointLights[i]->getConstant());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].linear", closePointLights[i]->getLinear());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].quadratic", closePointLights[i]->getQuadratic());
    }

    mTileGenerator->render(GL_PATCHES);
    mShaderManager->release();
}

void Canavar::Engine::Terrain::reset()
{
    mOctaves = 13;
    mFrequency = 0.01f;
    mTessellationMultiplier = 1.0f;
    mAmplitude = 20.0f;
    mPower = 3.0f;
    mGrassCoverage = 0.45f;
    mSeed = QVector3D(1, 1, 1);

    mAmbient = 0.5f;
    mDiffuse = 0.6f;
    mShininess = 8.0f;
    mSpecular = 0.05f;
}

Canavar::Engine::Terrain *Canavar::Engine::Terrain::instance()
{
    static Terrain instance;
    return &instance;
}
