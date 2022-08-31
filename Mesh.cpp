#include "Mesh.h"
#include "CameraManager.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ShaderManager.h"
#include "Water.h"

Mesh::Mesh(QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mSelectedVertexIndex(-1)
{
    mShaderManager = ShaderManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
    mHaze = Haze::instance();
    mWater = Water::instance();
}

Mesh::~Mesh()
{
    mVAO->destroy();
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
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

    // Vertex Rendering for Node Selector Setup
    mVertexModelTransformation.scale(0.05f, 0.05f, 0.05f);

    mVerticesVAO = new QOpenGLVertexArrayObject;
    mVerticesVAO->create();
    mVerticesVAO->bind();

    glGenBuffers(1, &mVerticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    mVerticesVAO->release();

    return true;
}

void Mesh::render(Model *model, const RenderSettings &settings)
{
    mCamera = mCameraManager->activeCamera();
    mSun = mLightManager->directionalLight();
    mClosePointLights = Helper::getClosePointLights(mLightManager->pointLights(), model);
    mCloseSpotLights = Helper::getCloseSpotLights(mLightManager->spotLights(), model);

    if (settings.renderFor == RenderFor::NodeSelectorMeshes)
    {
        mShaderManager->bind(ShaderManager::ShaderType::NodeSelectionMeshesShader);
        mShaderManager->setUniformValue("MVP", mCamera->getVP() * model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("nodeIndex", model->index());
        mShaderManager->setUniformValue("meshIndex", mIndex);
        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();
        mShaderManager->release();

    } else if (settings.renderFor == RenderFor::NodeSelectorVertices)
    {
        // For depth info
        mShaderManager->bind(ShaderManager::ShaderType::NodeSelectionMeshesShader);
        mShaderManager->setUniformValue("MVP", mCamera->getVP() * model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("nodeIndex", model->index());
        mShaderManager->setUniformValue("meshIndex", mIndex);
        mVAO->bind();
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        mVAO->release();
        mShaderManager->release();

        // Now render cubes
        mShaderManager->bind(ShaderManager::ShaderType::NodeSelectionVerticesShader);
        mShaderManager->setUniformValue("MVP", mCamera->getVP() * model->worldTransformation() * model->getMeshTransformation(mName));
        mShaderManager->setUniformValue("vertexModelTransformation", mVertexModelTransformation);
        mVerticesVAO->bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mVertices.size());
        mVerticesVAO->release();
        mShaderManager->release();
    } else
    {
        if (mSelected && mNodeManager->vertexSelectionEnabled())
        {
            // Render vertices as small cubes
            mShaderManager->bind(ShaderManager::ShaderType::VertexRendererShader);
            mShaderManager->setUniformValue("MVP", mCamera->getVP() * model->worldTransformation() * model->getMeshTransformation(mName));
            mShaderManager->setUniformValue("vertexModelTransformation", mVertexModelTransformation);
            mShaderManager->setUniformValue("selectedVertex", mSelectedVertexIndex);
            mVerticesVAO->bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, mVertices.size());
            mVerticesVAO->release();
            mShaderManager->release();
        }

        if ((int) settings.renderFor <= 1)
            glEnable(GL_CLIP_DISTANCE0);

        if (settings.renderModels)
        {
            mShaderManager->bind(ShaderManager::ShaderType::ModelShader);
            mShaderManager->setUniformValue("modelSelected", model->selected());
            mShaderManager->setUniformValue("meshSelected", mSelected);
            mShaderManager->setUniformValue("node.transformation", model->worldTransformation() * model->getMeshTransformation(mName));
            mShaderManager->setUniformValue("node.normalMatrix", model->normalMatrix());
            mShaderManager->setUniformValue("node.color", model->material().color);
            mShaderManager->setUniformValue("node.ambient", model->material().ambient);
            mShaderManager->setUniformValue("node.diffuse", model->material().diffuse);
            mShaderManager->setUniformValue("node.specular", model->material().specular);
            mShaderManager->setUniformValue("node.shininess", model->material().shininess);
            mShaderManager->setUniformValue("clipPlane", QVector4D(0, 1, 0, -mWater->waterHeight()) * (int) settings.renderFor);
            mShaderManager->setUniformValue("useBlinnShading", settings.useBlinnShading);
            mShaderManager->setUniformValue("VP", mCamera->getVP());
            mShaderManager->setUniformValue("cameraPosition", mCamera->worldPosition());
            mShaderManager->setUniformValue("directionalLight.direction", mSun->direction());
            mShaderManager->setUniformValue("directionalLight.color", mSun->color());
            mShaderManager->setUniformValue("directionalLight.ambient", mSun->ambient());
            mShaderManager->setUniformValue("directionalLight.diffuse", mSun->diffuse());
            mShaderManager->setUniformValue("directionalLight.specular", mSun->specular());
            mShaderManager->setUniformValue("haze.enabled", mHaze->enabled());
            mShaderManager->setUniformValue("haze.color", mHaze->color());
            mShaderManager->setUniformValue("haze.density", mHaze->density());
            mShaderManager->setUniformValue("haze.gradient", mHaze->gradient());
            pointLights();
            spotLights();
            materials();

            mVAO->bind();
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
            mVAO->release();
            mShaderManager->release();
        }

        glDisable(GL_CLIP_DISTANCE0);

        if (settings.renderWireframe)
        {
            mShaderManager->bind(ShaderManager::ShaderType::WireframeShader);
            mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
            mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            mVAO->bind();
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
            mVAO->release();
            mShaderManager->release();
        }

        if (settings.renderNormals)
        {
            mShaderManager->bind(ShaderManager::ShaderType::NormalsShader);
            mShaderManager->setUniformValue("projectionMatrix", mCamera->projection());
            mShaderManager->setUniformValue("viewMatrix", mCamera->worldTransformation());
            mShaderManager->setUniformValue("nodeMatrix", model->worldTransformation());
            mVAO->bind();
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
            mVAO->release();
            mShaderManager->release();
        }
    }
}

void Mesh::pointLights()
{
    mShaderManager->setUniformValue("numberOfPointLights", mClosePointLights.size());

    for (int i = 0; i < mClosePointLights.size(); i++)
    {
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].color", mClosePointLights[i]->color());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].position", mClosePointLights[i]->position());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].ambient", mClosePointLights[i]->ambient());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].diffuse", mClosePointLights[i]->diffuse());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].specular", mClosePointLights[i]->specular());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].constant", mClosePointLights[i]->constant());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].linear", mClosePointLights[i]->linear());
        mShaderManager->setUniformValue("pointLights[" + QString::number(i) + "].quadratic", mClosePointLights[i]->quadratic());
    }
}

void Mesh::spotLights()
{
    mShaderManager->setUniformValue("numberOfSpotLights", mCloseSpotLights.size());

    for (int i = 0; i < mCloseSpotLights.size(); i++)
    {
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].color", mCloseSpotLights[i]->color());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].position", mCloseSpotLights[i]->position());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].ambient", mCloseSpotLights[i]->ambient());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].diffuse", mCloseSpotLights[i]->diffuse());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].specular", mCloseSpotLights[i]->specular());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].constant", mCloseSpotLights[i]->constant());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].linear", mCloseSpotLights[i]->linear());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].quadratic", mCloseSpotLights[i]->quadratic());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].direction", mCloseSpotLights[i]->direction());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].cutOffAngle", mCloseSpotLights[i]->cutOffAngle());
        mShaderManager->setUniformValue("spotLights[" + QString::number(i) + "].outerCutOffAngle", mCloseSpotLights[i]->outerCutOffAngle());
    }
}

void Mesh::materials()
{
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
}

int Mesh::selectedVertexIndex() const
{
    return mSelectedVertexIndex;
}

void Mesh::setSelectedVertexIndex(int newSelectedVertexIndex)
{
    mSelectedVertexIndex = newSelectedVertexIndex;
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

unsigned int Mesh::index() const
{
    return mIndex;
}

void Mesh::setIndex(unsigned int newIndex)
{
    mIndex = newIndex;
}

bool Mesh::selected() const
{
    return mSelected;
}

void Mesh::setSelected(bool newSelected)
{
    mSelected = newSelected;
    mSelectedVertexIndex = -1;
}

void Mesh::drawGUI()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Info");
    ImGui::Text("Index: %d", mIndex);
    ImGui::Text("Number of vertices: %d", mVertices.size());
}

void Mesh::drawGUIForVertex()
{
    if (mSelectedVertexIndex != -1 && mSelectedVertexIndex < mIndices.size())
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Vertex Info");
        ImGui::Text("Selected vertex index: %d", mSelectedVertexIndex);
        ImGui::Text("Selected vertex position: (%.3f, %.3f, %.3f)", //
                    mVertices[mSelectedVertexIndex].position[0],
                    mVertices[mSelectedVertexIndex].position[1],
                    mVertices[mSelectedVertexIndex].position[2]);
    }
}

const float Mesh::CUBE_VERTICES[108] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f,
                                        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
                                        -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                                        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,
                                        1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f};
