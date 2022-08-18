#include "Terrain.h"

#include <QMatrix4x4>

Terrain::Terrain(QObject *parent)
    : QObject{parent}
{
    mShaderManager = ShaderManager::instance();

    mProperties.resolution = 3;
    mProperties.grids = 101;
    mProperties.width = 1000;
    mProperties.octaves = 13;
    mProperties.frequency = 0.01f;
    mProperties.tessellationMultiplier = 1.0f;
    mProperties.amplitude = 20.0f;
    mProperties.power = 3.0f;
    mProperties.grassCoverage = 0.15f;
    mProperties.seed = QVector3D(1, 1, 1);

    mMaterial.ambient = 0.25f;
    mMaterial.diffuse = 0.75f;
    mMaterial.shininess = 4.0f;
    mMaterial.specular = 0.0f;
}

void Terrain::create()
{
    const int resolution = mProperties.resolution;
    const int width = mProperties.width;

    const int nPoints = resolution * resolution;
    const int size = nPoints * 3 + nPoints * 3 + nPoints * 2;
    float *vertices = new float[size];

    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            float x = j * (float) width / (resolution - 1) - width / 2.0;
            float y = 0.0;
            float z = -i * (float) width / (resolution - 1) + width / 2.0;

            vertices[(i + j * resolution) * 8] = x;
            vertices[(i + j * resolution) * 8 + 1] = y;
            vertices[(i + j * resolution) * 8 + 2] = z;

            float x_n = 0.0;
            float y_n = 1.0;
            float z_n = 0.0;

            vertices[(i + j * resolution) * 8 + 3] = x_n;
            vertices[(i + j * resolution) * 8 + 4] = y_n;
            vertices[(i + j * resolution) * 8 + 5] = z_n;

            vertices[(i + j * resolution) * 8 + 6] = (float) j / (resolution - 1);
            vertices[(i + j * resolution) * 8 + 7] = (float) (resolution - i - 1) / (resolution - 1);
        }
    }

    const int nTris = (resolution - 1) * (resolution - 1) * 2;
    int *trisIndices = new int[nTris * 3];

    for (int i = 0; i < nTris; i++)
    {
        int trisPerRow = 2 * (resolution - 1);
        for (int j = 0; j < trisPerRow; j++)
        {
            if (!(i % 2))
            {
                int k = i * 3;
                int triIndex = i % trisPerRow;

                int row = i / trisPerRow;
                int col = triIndex / 2;
                trisIndices[k] = row * resolution + col;
                trisIndices[k + 1] = ++row * resolution + col;
                trisIndices[k + 2] = --row * resolution + ++col;
            } else
            {
                int k = i * 3;
                int triIndex = i % trisPerRow;

                int row = i / trisPerRow;
                int col = triIndex / 2;
                trisIndices[k] = row * resolution + ++col;
                trisIndices[k + 1] = ++row * resolution + --col;
                trisIndices[k + 2] = row * resolution + ++col;
            }
        }
    }

    QVector2D I = QVector2D(1, 0) * mProperties.width;
    QVector2D J = QVector2D(0, 1) * mProperties.width;

    mPositions.resize(mProperties.grids * mProperties.grids);

    for (int i = 0; i < mProperties.grids; i++)
    {
        for (int j = 0; j < mProperties.grids; j++)
        {
            QVector2D pos = (float) (j - mProperties.grids / 2) * I + (float) (i - mProperties.grids / 2) * J;
            mPositions[j + i * mProperties.grids] = pos;
        }
    }

    initializeOpenGLFunctions();
    mVAO.create();
    mVAO.bind();
    glGenBuffers(1, &mEBO);
    glGenBuffers(1, &mVBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTris * 3 * sizeof(unsigned int), trisIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    mVAO.release();

    // PBO
    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(QVector2D), mPositions.constData(), GL_STATIC_DRAW);

    mVAO.bind();
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (void *) 0);
    glVertexAttribDivisor(3, 1);
    mVAO.release();

    mTextureSand = new Texture(Texture::Type::None, "Resources/Terrain/sand.jpg");
    mTextureGrass = new Texture(Texture::Type::None, "Resources/Terrain/grass0.jpg");
    mTextureSnow = new Texture(Texture::Type::None, "Resources/Terrain/snow0.jpg");
    mTextureRockDiffuse = new Texture(Texture::Type::None, "Resources/Terrain/rock0.jpg");
    mTextureRockNormal = new Texture(Texture::Type::None, "Resources/Terrain/rnormal.jpg");
    mTextureTerrain = new Texture(Texture::Type::None, "Resources/Terrain/terrain.jpg");

    mTextureSand->create();
    mTextureGrass->create();
    mTextureSnow->create();
    mTextureRockDiffuse->create();
    mTextureRockNormal->create();
    mTextureTerrain->create();

    delete[] vertices;
    delete[] trisIndices;
}

void Terrain::render()
{
    mVAO.bind();

    mShaderManager->setUniformValue("sand", 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, mTextureSand->id());

    mShaderManager->setUniformValue("grass", 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, mTextureGrass->id());

    mShaderManager->setUniformValue("terrainTexture", 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, mTextureTerrain->id());

    mShaderManager->setUniformValue("snow", 4);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, mTextureSnow->id());

    mShaderManager->setUniformValue("rock", 5);
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, mTextureRockDiffuse->id());

    mShaderManager->setUniformValue("rockNormal", 6);
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_2D, mTextureRockNormal->id());

    glDrawElementsInstanced(GL_PATCHES, (mProperties.resolution - 1) * (mProperties.resolution - 1) * 2 * 3, GL_UNSIGNED_INT, 0, mPositions.size());

    mVAO.release();

    glActiveTexture(GL_TEXTURE0);
}

QMatrix4x4 Terrain::transformation() const
{
    QMatrix4x4 transformation = QMatrix4x4();
    transformation.scale(QVector3D(1, 0, 1));
    return transformation;
}

const Terrain::Properties &Terrain::properties() const
{
    return mProperties;
}

void Terrain::setProperties(const Properties &newProperties)
{
    mProperties = newProperties;
}

const Material &Terrain::material() const
{
    return mMaterial;
}

void Terrain::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}
