#include "Model.h"

Model::Model(const QString &modelName, QObject *parent)
    : Node(parent)
    , mModelName(modelName)
{
    mName = mModelName;
    mNodeType = Node::NodeType::Model;
}

Model::~Model() {}

const QString &Model::modelName() const
{
    return mModelName;
}

const Model::Material &Model::material() const
{
    return mMaterial;
}

void Model::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}

QMatrix4x4 Model::getMeshTransformation(const QString &meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Model::setMeshTransformation(const QString &meshName, const QMatrix4x4 &transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}

const QMap<QString, QMatrix4x4> &Model::meshTransformations() const
{
    return mMeshTransformations;
}

void Model::drawGUI()
{
    Node::drawGUI();

    // Shading Parameters
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        ImGui::SliderFloat("Ambient##Model", &mMaterial.ambient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Model", &mMaterial.diffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Model", &mMaterial.specular, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Model", &mMaterial.shininess, 1.0f, 128.0f, "%.3f");
        ImGui::ColorEdit4("Color##Model", (float *) &mMaterial.color);
    }
}
