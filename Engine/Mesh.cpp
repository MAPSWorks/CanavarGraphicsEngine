#include "Mesh.h"
#include "CameraManager.h"
#include "Common.h"
#include "Helper.h"
#include "Model.h"
#include "ShaderManager.h"

Canavar::Engine::Mesh::Mesh(QObject *parent)
    : QObject(parent)
    , mVAO(nullptr)
    , mMaterial(nullptr)
{
    mShaderManager = ShaderManager::instance();
}

Canavar::Engine::Mesh::~Mesh()
{
    // TODO
}

void Canavar::Engine::Mesh::addVertex(const Vertex &vertex)
{
    mVertices << vertex;
}

void Canavar::Engine::Mesh::addIndex(unsigned int index)
{
    mIndices << index;
}

void Canavar::Engine::Mesh::setMaterial(Material *material)
{
    mMaterial = material;
}

void Canavar::Engine::Mesh::create()
{
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

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    //Texture Cooords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    // Tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // IDs
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, boneIDs));
    glEnableVertexAttribArray(5);

    // Weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, weights));
    glEnableVertexAttribArray(6);

    mVAO->release();
}

void Canavar::Engine::Mesh::render(const RenderParameters &parameters)
{
    auto model = parameters.model;

    bool useTexture = mMaterial->getNumberOfTextures() != 0;

    if (useTexture)
    {
        auto textureAmbient = mMaterial->get(Material::TextureType::Ambient);
        auto textureDiffuse = mMaterial->get(Material::TextureType::Diffuse);
        auto textureSpecular = mMaterial->get(Material::TextureType::Specular);
        auto textureNormal = mMaterial->get(Material::TextureType::Normal);

        mShaderManager->bind(ShaderType::ModelTexturedShader);
        mShaderManager->setUniformValue("N", model->worldTransformation().normalMatrix());
        mShaderManager->setUniformValue("useTextureAmbient", textureAmbient != nullptr || textureDiffuse != nullptr);
        mShaderManager->setUniformValue("useTextureDiffuse", textureDiffuse != nullptr);
        mShaderManager->setUniformValue("useTextureSpecular", textureSpecular != nullptr);
        mShaderManager->setUniformValue("useTextureNormal", textureNormal != nullptr);

        if (textureAmbient)
            mShaderManager->setSampler("textureAmbient", 0, textureAmbient->textureId());
        else if (textureDiffuse)
            mShaderManager->setSampler("textureAmbient", 0, textureDiffuse->textureId()); // Use diffuse texture if there is no ambient texture

        if (textureDiffuse)
            mShaderManager->setSampler("textureDiffuse", 1, textureDiffuse->textureId());

        if (textureSpecular)
            mShaderManager->setSampler("textureSpecular", 2, textureSpecular->textureId());

        if (textureNormal)
            mShaderManager->setSampler("textureNormal", 3, textureNormal->textureId());

    } else
    {
        mShaderManager->bind(ShaderType::ModelColoredShader);
        mShaderManager->setUniformValue("model.color", model->getColor());
    }

    // Common uniforms
    mShaderManager->setUniformValue("M", model->worldTransformation() * model->getMeshTransformation(mName));
    mShaderManager->setUniformValue("model.shininess", model->getShininess());
    mShaderManager->setUniformValue("model.ambient", model->getAmbient());
    mShaderManager->setUniformValue("model.diffuse", model->getDiffuse());
    mShaderManager->setUniformValue("model.specular", model->getSpecular());

    mVAO->bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVAO->release();

    mShaderManager->release();
}
