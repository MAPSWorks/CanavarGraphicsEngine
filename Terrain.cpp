#include "Terrain.h"
#include "RandomGenerator.h"

#include <QMatrix4x4>

Terrain::Terrain(QObject *parent)
    : QObject{parent}
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();

    mProperties.numberOfVerticesOnEdge = 4;
    mProperties.grids = 128;
    mProperties.width = 1024;

    reset();
}

void Terrain::create()
{
    const int n = mProperties.numberOfVerticesOnEdge;
    const float width = mProperties.width;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            float x = j * width / (n - 1) - width / 2.0f;
            float y = 0;
            float z = i * width / (n - 1) - width / 2.0f;
            float u = float(j) / (n - 1);
            float v = float(n - i - 1) / (n - 1);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);
            vertex.normal = QVector3D(0, 1, 0);
            vertex.texture = QVector2D(u, v);

            mVertices << vertex;
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            mIndices << n * i + j;
            mIndices << n * (i + 1) + j;
            mIndices << n * i + j + 1;

            mIndices << n * (i + 1) + j;
            mIndices << n * (i + 1) + j + 1;
            mIndices << n * i + j + 1;
        }
    }

    const QVector2D I = QVector2D(1, 0) * mProperties.width;
    const QVector2D J = QVector2D(0, 1) * mProperties.width;

    mGridPositions.resize(mProperties.grids * mProperties.grids);

    for (int i = 0; i < mProperties.grids; i++)
    {
        for (int j = 0; j < mProperties.grids; j++)
        {
            QVector2D pos = (float) (j - mProperties.grids / 2) * I + (float) (i - mProperties.grids / 2) * J;
            mGridPositions[j + i * mProperties.grids] = pos;
        }
    }

    initializeOpenGLFunctions();
    mVAO = new QOpenGLVertexArrayObject;
    mVAO->create();
    mVAO->bind();

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mGridPositions.size() * sizeof(QVector2D), mGridPositions.constData(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (void *) 0);
    glVertexAttribDivisor(3, 1);
    mVAO->release();

    mTextureSand = new Texture(Texture::Type::Custom2D, "Resources/Terrain/sand.jpg");
    mTextureGrass = new Texture(Texture::Type::Custom2D, "Resources/Terrain/grass0.jpg");
    mTextureSnow = new Texture(Texture::Type::Custom2D, "Resources/Terrain/snow0.jpg");
    mTextureRockDiffuse = new Texture(Texture::Type::Custom2D, "Resources/Terrain/rock0.jpg");
    mTextureRockNormal = new Texture(Texture::Type::Custom2D, "Resources/Terrain/rnormal.jpg");
    mTextureTerrain = new Texture(Texture::Type::Custom2D, "Resources/Terrain/terrain.jpg");
}

void Terrain::render()
{
    QVector2D currentTilePosition = getCurrentTilePosition();

    if (currentTilePosition != mPreviousTilePosition)
    {
        updatePositionVectors(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;
    }

    mVAO->bind();

    mShaderManager->setSampler("sand", 1, mTextureSand->id());
    mShaderManager->setSampler("grass", 2, mTextureGrass->id());
    mShaderManager->setSampler("terrainTexture", 3, mTextureTerrain->id());
    mShaderManager->setSampler("snow", 4, mTextureSnow->id());
    mShaderManager->setSampler("rock", 5, mTextureRockDiffuse->id());
    mShaderManager->setSampler("rockNormal", 6, mTextureRockNormal->id());

    glDrawElementsInstanced(GL_PATCHES, (mProperties.numberOfVerticesOnEdge - 1) * (mProperties.numberOfVerticesOnEdge - 1) * 2 * 3, GL_UNSIGNED_INT, 0, mGridPositions.size());

    mVAO->release();
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

QVector2D Terrain::getCurrentTilePosition() const
{
    const QVector3D cameraPosition = mCameraManager->activeCamera()->worldPosition();

    for (const auto &position : qAsConst(mGridPositions))
    {
        if (cameraPosition.x() > position.x() - mProperties.width / 2 && //
            cameraPosition.x() < position.x() + mProperties.width / 2 && //
            cameraPosition.z() > position.y() - mProperties.width / 2 && //
            cameraPosition.z() < position.y() + mProperties.width / 2)

            return position;
    }

    return QVector2D(0, 0);
}

void Terrain::updatePositionVectors(const QVector2D &translation)
{
    for (int i = 0; i < mGridPositions.size(); ++i)
        mGridPositions[i] += translation;

    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mGridPositions.size() * sizeof(QVector2D), mGridPositions.constData());
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
