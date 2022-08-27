#include "Mesh.h"
#include "ModelData.h"

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

void Mesh::render(Model *model)
{
    mShaderManager->setUniformValue("node.transformation", model->worldTransformation() * model->getMeshTransformation(mName));
    mShaderManager->setUniformValue("node.normalMatrix", model->normalMatrix());
    mShaderManager->setUniformValue("node.color", model->material().color);
    mShaderManager->setUniformValue("node.ambient", model->material().ambient);
    mShaderManager->setUniformValue("node.diffuse", model->material().diffuse);
    mShaderManager->setUniformValue("node.specular", model->material().specular);
    mShaderManager->setUniformValue("node.shininess", model->material().shininess);

    auto materials = mData->materials();
    auto ambientTexture = materials[mMaterialIndex]->getFirstTexture(Texture::Type::Ambient);
    auto diffuseTexture = materials[mMaterialIndex]->getFirstTexture(Texture::Type::Diffuse);
    auto specularTexture = materials[mMaterialIndex]->getFirstTexture(Texture::Type::Specular);
    auto normalTexture = materials[mMaterialIndex]->getFirstTexture(Texture::Type::Normal);

    mShaderManager->setUniformValue("useTexture", false);
    mShaderManager->setUniformValue("useTextureAmbient", false);
    mShaderManager->setUniformValue("useTextureDiffuse", false);
    mShaderManager->setUniformValue("useTextureSpecular", false);
    mShaderManager->setUniformValue("useTextureNormal", false);
    mShaderManager->setUniformValue("useTexture", materials[mMaterialIndex]->textures().size() != 0);

    if (ambientTexture)
    {
        mShaderManager->setSampler("textureAmbient", 1, ambientTexture->id());
        mShaderManager->setUniformValue("useTextureAmbient", true);

    } else
    {
        if (diffuseTexture)
        {
            mShaderManager->setSampler("textureAmbient", 2, diffuseTexture->id());
            mShaderManager->setUniformValue("useTextureAmbient", true);
        }
    }

    if (diffuseTexture)
    {
        mShaderManager->setSampler("textureDiffuse", 3, diffuseTexture->id());
        mShaderManager->setUniformValue("useTextureDiffuse", true);
    }

    if (specularTexture)
    {
        mShaderManager->setSampler("textureSpecular", 4, specularTexture->id());
        mShaderManager->setUniformValue("useTextureSepcular", true);
    }

    if (normalTexture)
    {
        mShaderManager->setSampler("textureNormal", 5, normalTexture->id());
        mShaderManager->setUniformValue("useTextureNormal", true);
    }

    mVertexArray.bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVertexArray.release();
}

void Mesh::renderForNodeSelector(Model *model)
{
    mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation() * model->getMeshTransformation(mName));
    mVertexArray.bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVertexArray.release();
}

void Mesh::render(Primitive primitive)
{
    mVertexArray.bind();
    glDrawElements((unsigned int) primitive, mIndices.size(), GL_UNSIGNED_INT, 0);
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

ModelData *Mesh::data() const
{
    return mData;
}

void Mesh::setData(ModelData *newData)
{
    mData = newData;
}

const Mesh::AABB &Mesh::getAABB() const
{
    return mAABB;
}

void Mesh::setAABB(const Mesh::AABB &newAABB)
{
    mAABB = newAABB;
}
