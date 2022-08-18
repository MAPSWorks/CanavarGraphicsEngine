#include "Mesh.h"
#include "TexturedModelData.h"

Mesh::Mesh(QObject *parent)
    : QObject(parent)
{
    mShaderManager = ShaderManager::instance();
}

Mesh::~Mesh()
{
    mVertexArray.bind();
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
    mVertexArray.release();

    mVertexArray.destroy();
}

void Mesh::addVertex(const Vertex &vertex)
{
    mVertices << vertex;
}

void Mesh::addIndex(unsigned int index)
{
    mIndices << index;
}

int Mesh::materialIndex() const
{
    return mMaterialIndex;
}

void Mesh::setMaterialIndex(int newMaterialIndex)
{
    mMaterialIndex = newMaterialIndex;
}

bool Mesh::create()
{
    //qInfo() << "Creating VAO for mesh [" << mName << "] of model [" << parent()->objectName() << "]...";

    initializeOpenGLFunctions();

    mVertexArray.create();
    mVertexArray.bind();

    glGenBuffers(1, &mEBO);
    glGenBuffers(1, &mVBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,              // Size
                          GL_FLOAT,       // Type
                          GL_FALSE,       // Normalized
                          sizeof(Vertex), // Stride
                          (void *) 0      // Offset
    );

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    //Texture Cooords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    // Bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));
    // IDs
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, boneIDs));
    // Weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));

    mVertexArray.release();

    return true;
}

void Mesh::render()
{
    auto materials = mData->materials();
    auto textures = materials[mMaterialIndex]->textures();

    for (int i = 0; i < textures.size(); i++)
    {
        if (textures[i]->type() == Texture::Type::Diffuse)
            mShaderManager->setUniformValue("textureDiffuse", i);
        else if (textures[i]->type() == Texture::Type::Specular)
            mShaderManager->setUniformValue("textureSpecular", i);
        else if (textures[i]->type() == Texture::Type::Ambient)
            mShaderManager->setUniformValue("textureAmbient", i);
        else if (textures[i]->type() == Texture::Type::Height)
            mShaderManager->setUniformValue("textureHeight", i);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]->id());
    }

    mVertexArray.bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVertexArray.release();

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::renderWireframe()
{
    mVertexArray.bind();
    glDrawElements(GL_LINE_STRIP, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVertexArray.release();
}

void Mesh::renderNormals()
{
    mVertexArray.bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVertexArray.release();
}

const QString &Mesh::name() const
{
    return mName;
}

void Mesh::setName(const QString &newName)
{
    setObjectName(newName);
    mName = newName;
}

TexturedModelData *Mesh::data() const
{
    return mData;
}

void Mesh::setData(TexturedModelData *newData)
{
    mData = newData;
}
