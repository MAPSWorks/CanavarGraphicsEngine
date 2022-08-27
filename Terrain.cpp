#include "Terrain.h"
#include "RandomGenerator.h"

#include <QMatrix4x4>

Terrain::Terrain(QObject *parent)
    : QObject(parent)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();

    reset();
}

void Terrain::create()
{
    mTileGenerator = new TileGenerator(3, 128, 1024.0f);
    mTileGenerator->generate();

    mTextureSand = new Texture(Texture::Type::Texture2D, "Resources/Terrain/sand.jpg");
    mTextureGrass = new Texture(Texture::Type::Texture2D, "Resources/Terrain/grass0.jpg");
    mTextureSnow = new Texture(Texture::Type::Texture2D, "Resources/Terrain/snow0.jpg");
    mTextureRockDiffuse = new Texture(Texture::Type::Texture2D, "Resources/Terrain/rock0.jpg");
    mTextureRockNormal = new Texture(Texture::Type::Texture2D, "Resources/Terrain/rnormal.jpg");
    mTextureTerrain = new Texture(Texture::Type::Texture2D, "Resources/Terrain/terrain.jpg");
}

void Terrain::render()
{
    QVector2D currentTilePosition = mTileGenerator->whichTile(mCameraManager->activeCamera()->worldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->translateTiles(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;
    }

    mShaderManager->setSampler("sand", 1, mTextureSand->id());
    mShaderManager->setSampler("grass", 2, mTextureGrass->id());
    mShaderManager->setSampler("terrainTexture", 3, mTextureTerrain->id());
    mShaderManager->setSampler("snow", 4, mTextureSnow->id());
    mShaderManager->setSampler("rock", 5, mTextureRockDiffuse->id());
    mShaderManager->setSampler("rockNormal", 6, mTextureRockNormal->id());

    mTileGenerator->render(TileGenerator::Primitive::Patches);
}

QMatrix4x4 Terrain::transformation() const
{
    QMatrix4x4 transformation = QMatrix4x4();
    transformation.scale(QVector3D(1, 0, 1));
    return transformation;
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

void Terrain::drawGui()
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

const Model::Material &Terrain::material() const
{
    return mMaterial;
}

void Terrain::setMaterial(const Model::Material &newMaterial)
{
    mMaterial = newMaterial;
}

const Terrain::Properties &Terrain::properties() const
{
    return mProperties;
}

void Terrain::setProperties(const Properties &newProperties)
{
    mProperties = newProperties;
}
