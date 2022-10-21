#include "Mesh.h"
#include "CameraManager.h"
#include "Common.h"
#include "LightManager.h"
#include "Model.h"
#include "ShaderManager.h"
#include "Sun.h"

Canavar::Engine::Mesh::Mesh(QObject *parent)
    : QObject(parent)
    , mVAO(nullptr)
    , mMaterial(nullptr)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
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

const Canavar::Engine::Mesh::AABB &Canavar::Engine::Mesh::aABB() const
{
    return mAABB;
}

void Canavar::Engine::Mesh::setAABB(const AABB &newAABB)
{
    mAABB = newAABB;
}

void Canavar::Engine::Mesh::setName(const QString &newName)
{
    mName = newName;
}

unsigned int Canavar::Engine::Mesh::id() const
{
    return mId;
}

void Canavar::Engine::Mesh::setId(unsigned int newId)
{
    mId = newId;
}

void Canavar::Engine::Mesh::render(const RenderParameters &parameters)
{
    auto camera = mCameraManager->activeCamera();
    auto model = parameters.model;
    auto sun = Canavar::Engine::Sun::instance();

    bool useTexture = mMaterial->getNumberOfTextures() != 0;

    if (useTexture)
    {
        auto textureAmbient = mMaterial->get(Material::TextureType::Ambient);
        auto textureDiffuse = mMaterial->get(Material::TextureType::Diffuse);
        auto textureSpecular = mMaterial->get(Material::TextureType::Specular);
        auto textureNormal = mMaterial->get(Material::TextureType::Normal);

        mShaderManager->bind(ShaderType::ModelTexturedShader);

        mShaderManager->setUniformValue("M", model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("N", model->worldTransformation().normalMatrix());
        mShaderManager->setUniformValue("VP", camera->getViewProjectionMatrix());

        mShaderManager->setUniformValue("useTextureAmbient", textureAmbient != nullptr || textureDiffuse != nullptr);
        mShaderManager->setUniformValue("useTextureDiffuse", textureDiffuse != nullptr);
        mShaderManager->setUniformValue("useTextureSpecular", textureSpecular != nullptr);
        mShaderManager->setUniformValue("useTextureNormal", textureNormal != nullptr);

        mShaderManager->setUniformValue("model.shininess", model->shininess());
        mShaderManager->setUniformValue("model.ambient", model->ambient());
        mShaderManager->setUniformValue("model.diffuse", model->diffuse());
        mShaderManager->setUniformValue("model.specular", model->specular());

        mShaderManager->setUniformValue("sun.direction", -sun->direction().normalized());
        mShaderManager->setUniformValue("sun.color", sun->color());
        mShaderManager->setUniformValue("sun.ambient", sun->ambient());
        mShaderManager->setUniformValue("sun.diffuse", sun->diffuse());
        mShaderManager->setUniformValue("sun.specular", sun->specular());

        mShaderManager->setUniformValue("cameraPos", camera->worldPosition());

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

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();

        mShaderManager->release();

    } else
    {
        mShaderManager->bind(ShaderType::ModelColoredShader);

        mShaderManager->setUniformValue("M", model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("N", model->worldTransformation().normalMatrix());
        mShaderManager->setUniformValue("VP", camera->getViewProjectionMatrix());

        mShaderManager->setUniformValue("model.color", model->color());
        mShaderManager->setUniformValue("model.ambient", model->ambient());
        mShaderManager->setUniformValue("model.diffuse", model->diffuse());
        mShaderManager->setUniformValue("model.specular", model->specular());
        mShaderManager->setUniformValue("model.shininess", model->shininess());

        mShaderManager->setUniformValue("sun.direction", -sun->direction().normalized());
        mShaderManager->setUniformValue("sun.color", sun->color());
        mShaderManager->setUniformValue("sun.ambient", sun->ambient());
        mShaderManager->setUniformValue("sun.diffuse", sun->diffuse());
        mShaderManager->setUniformValue("sun.specular", sun->specular());

        mShaderManager->setUniformValue("cameraPos", camera->worldPosition());

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();

        mShaderManager->release();
    }
}
