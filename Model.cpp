#include "Model.h"

Model::Model(const QString &modelName, QObject *parent)
    : Node(parent)
    , mModelName(modelName)
{
    mName = mModelName;
    mNodeType = Node::NodeType::Model;

    mMaterial.color = QVector4D(1, 1, 1, 1);
    mMaterial.ambient = 0.5f;
    mMaterial.diffuse = 0.75f;
    mMaterial.specular = 0.25f;
    mMaterial.shininess = 32.0f;
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

void Model::drawGui()
{
    Node::drawGui();

    // Shading Parameters
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        ImGui::SliderFloat("Ambient##Model", &mMaterial.ambient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Model", &mMaterial.diffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Model", &mMaterial.specular, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Model", &mMaterial.shininess, 1.0f, 128.0f, "%.3f");

        float color[4] = {mMaterial.color.x(), mMaterial.color.y(), mMaterial.color.z(), mMaterial.color.w()};

        if (ImGui::ColorEdit4("Color##Model", (float *) &color))
            mMaterial.color = QVector4D(color[0], color[1], color[2], color[3]);
    }
}
