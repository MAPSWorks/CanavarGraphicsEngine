#include "Mesh.h"
#include "CameraManager.h"
#include "Common.h"
#include "LightManager.h"
#include "Model.h"
#include "ShaderManager.h"

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
    auto sun = mLightManager->sun();

    if (auto model = dynamic_cast<Model *>(parameters.node))
    {
        mShaderManager->bind(ShaderType::ModelShader);

        mShaderManager->setUniformValue("M", model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("N", model->worldTransformation().normalMatrix());
        mShaderManager->setUniformValue("VP", camera->getViewProjectionMatrix());
        mShaderManager->setUniformValue("color", model->color() * sun->color());
        mShaderManager->setUniformValue("ambient", model->ambient() * sun->ambient());
        mShaderManager->setUniformValue("diffuse", model->diffuse() * sun->diffuse());
        mShaderManager->setUniformValue("specular", model->specular() * sun->specular());
        mShaderManager->setUniformValue("shininess", model->shininess());
        mShaderManager->setUniformValue("cameraPos", camera->worldPosition());
        mShaderManager->setUniformValue("sunDir", -sun->direction().normalized());

        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();

        mShaderManager->release();
    }
}
