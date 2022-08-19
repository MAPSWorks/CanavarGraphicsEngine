#include "Helper.h"
#include "Mesh.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuaternion>
#include <QTextStream>
#include <QtMath>

#include <Dense>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    } else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QQuaternion Helper::rotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

QVector3D Helper::projectOntoPlane(const QVector3D &normal, const QVector3D &point, const QVector3D &subject)
{
    Eigen::Vector3f normalEigen = Eigen::Vector3f(normal.x(), normal.y(), normal.z());
    Eigen::Vector3f pointEigen = Eigen::Vector3f(point.x(), point.y(), point.z());
    Eigen::Vector3f subjectEigen = Eigen::Vector3f(subject.x(), subject.y(), subject.z());
    Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(normalEigen, -normalEigen.dot(pointEigen));
    Eigen::Vector3f projection = plane.projection(subjectEigen);

    return QVector3D(projection.x(), projection.y(), projection.z());
}

ModelData *Helper::loadModel(const QString &name, const QString &path)
{
    Assimp::Importer importer;
    const aiScene *aiScene = importer.ReadFile(path.toStdString(),              //
                                               aiProcess_Triangulate |          //
                                                   aiProcess_GenSmoothNormals | //
                                                   aiProcess_FlipUVs |          //
                                                   aiProcess_CalcTangentSpace);

    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        qCritical() << Q_FUNC_INFO << importer.GetErrorString();
        return nullptr;
    }

    ModelData *data = new ModelData(name);

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        aiMesh *aiMesh = aiScene->mMeshes[i];
        Mesh *mesh = processMesh(aiMesh);
        mesh->setData(data);
        data->addMesh(mesh);
    }

    // Node
    ModelDataNode *rootNode = processNode(data, aiScene->mRootNode);
    data->setRootNode(rootNode);

    QString directory = path.left(path.lastIndexOf("/"));

    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        TextureMaterial *material = processMaterial(aiScene->mMaterials[i], directory);
        data->addMaterial(material);
    }

    return data;
}

QVector<PointLight *> Helper::getClosePointLights(const QList<PointLight *> &pointLights, Node *node)
{
    QMap<float, PointLight *> distanceToLight;

    for (const auto &light : pointLights)
    {
        distanceToLight.insert((light->position() - node->position()).length(), light);
    }

    QList<PointLight *> lights = distanceToLight.values();

    QVector<PointLight *> lightsVector;

    for (int i = 0; i < qMin(8, lights.size()); ++i)
    {
        lightsVector << lights[i];
    }

    return lightsVector;
}

QVector<SpotLight *> Helper::getCloseSpotLights(const QList<SpotLight *> &spotLights, Node *node)
{
    QMap<float, SpotLight *> distanceToLight;

    for (const auto &light : spotLights)
    {
        distanceToLight.insert((light->position() - node->position()).length(), light);
    }

    QList<SpotLight *> lights = distanceToLight.values();

    QVector<SpotLight *> lightsVector;

    for (int i = 0; i < qMin(8, lights.size()); ++i)
    {
        lightsVector << lights[i];
    }

    return lightsVector;
}

Mesh *Helper::processMesh(aiMesh *aiMesh)
{
    Mesh *mesh = new Mesh;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        vertex.position = QVector3D(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
            vertex.normal = QVector3D(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);

        if (aiMesh->mTextureCoords[0])
            vertex.texture = QVector2D(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
        else
            vertex.texture = QVector2D(0, 0);

        if (aiMesh->HasTangentsAndBitangents())
        {
            vertex.tangent = QVector3D(aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z);
            vertex.bitangent = QVector3D(aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z);
        }

        mesh->addVertex(vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace aiFace = aiMesh->mFaces[i];

        for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
            mesh->addIndex(aiFace.mIndices[j]);
    }

    mesh->setName(aiMesh->mName.C_Str());
    mesh->setMaterialIndex(aiMesh->mMaterialIndex);

    return mesh;
}

ModelDataNode *Helper::processNode(ModelData *data, aiNode *aiParentNode)
{
    ModelDataNode *parentNode = new ModelDataNode(data);

    for (unsigned int i = 0; i < aiParentNode->mNumMeshes; ++i)
        parentNode->addMeshIndex(aiParentNode->mMeshes[i]);

    parentNode->setName(aiParentNode->mName.C_Str());
    parentNode->setInitialTransformation(toQMatrix(aiParentNode->mTransformation));

    for (unsigned int i = 0; i < aiParentNode->mNumChildren; ++i)
        parentNode->addChild(processNode(data, aiParentNode->mChildren[i]));

    return parentNode;
}

TextureMaterial *Helper::processMaterial(aiMaterial *aiMaterial, const QString &directory)
{
    TextureMaterial *material = new TextureMaterial;
    processTexture(material, aiMaterial, aiTextureType_AMBIENT, Texture::Type::Ambient, directory);
    processTexture(material, aiMaterial, aiTextureType_DIFFUSE, Texture::Type::Diffuse, directory);
    processTexture(material, aiMaterial, aiTextureType_SPECULAR, Texture::Type::Specular, directory);
    processTexture(material, aiMaterial, aiTextureType_HEIGHT, Texture::Type::Height, directory);
    processTexture(material, aiMaterial, aiTextureType_BASE_COLOR, Texture::Type::BaseColor, directory);

    return material;
}

void Helper::processTexture(TextureMaterial *material, aiMaterial *aiMaterial, aiTextureType aiType, Texture::Type type, const QString &directory)
{
    for (unsigned int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);
        QString filename = QString(str.C_Str());
        Texture *texture = new Texture(type, directory + "/" + filename);
        texture->create();
        material->addTexture(texture);

        //qInfo() << "Texture" << texture->path() << "is loaded and created. ID is" << texture->id();
    }
}

// TODO
QMatrix4x4 Helper::toQMatrix(const aiMatrix4x4 &matrix)
{
    return QMatrix4x4();
}
