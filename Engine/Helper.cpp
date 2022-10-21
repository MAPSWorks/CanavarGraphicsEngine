#include "Helper.h"
#include "ModelData.h"

#include <QFile>
#include <QtMath>

Canavar::Engine::Helper::Helper() {}

float Canavar::Engine::Helper::calculateHorizontalFovForGivenVerticalFov(float verticalFov, float width, float height)
{
    float hvfr = 0.5f * qDegreesToRadians(verticalFov);
    float hfr = 2 * atan(tan(hvfr) / (width / height));

    return qRadiansToDegrees(hfr);
}

float Canavar::Engine::Helper::calculateVerticalFovForGivenHorizontalFov(float horizontalFov, float width, float height)
{
    float hhfr = 0.5f * qDegreesToRadians(horizontalFov);
    float vfr = 2 * atan(tan(hhfr) * (width / height));

    return qRadiansToDegrees(vfr);
}

QByteArray Canavar::Engine::Helper::getBytes(QString path)
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

QQuaternion Canavar::Engine::Helper::rotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Canavar::Engine::Helper::rotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Canavar::Engine::Helper::rotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

float Canavar::Engine::Helper::angleBetween(const QVector3D &v1, const QVector3D &v2, const QVector3D &left)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;

    } else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;

    } else
    {
        float angle = qRadiansToDegrees(acos(dot));
        QVector3D u1xu2 = QVector3D::crossProduct(u1, u2);

        if (QVector3D::dotProduct(u1xu2, left) < 0.0001f)
            return angle;
        else
            return -angle;
    }
}

float Canavar::Engine::Helper::angleBetween(const QVector3D &v1, const QVector3D &v2)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;

    } else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;

    } else
    {
        return qRadiansToDegrees(acos(dot));
    }
}

void Canavar::Engine::Helper::getEulerDegrees(const QQuaternion &rotation, float &yaw, float &pitch, float &roll)
{
    QVector3D zAxis = rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                          QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                          QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                     QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                     QVector3D(0, 0, -1);

    roll = Canavar::Engine::Helper::angleBetween(xAxis, xAxisProj, left);

    if (yaw < 0.0f)
        yaw += 360.0f;
}

QQuaternion Canavar::Engine::Helper::constructFromEulerDegrees(float yaw, float pitch, float roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
           QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
           QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), roll);
}

QQuaternion Canavar::Engine::Helper::invert(const QQuaternion &rotation)
{
    float yaw, pitch, roll;
    getEulerDegrees(rotation, yaw, pitch, roll);

    QQuaternion r = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -pitch);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), roll);
    return r;
}

Canavar::Engine::ModelData *Canavar::Engine::Helper::loadModel(const QString &name, const QString &path)
{
    Assimp::Importer importer;

    auto *aiScene = importer.ReadFile(path.toStdString(),              //
                                      aiProcess_Triangulate |          //
                                          aiProcess_GenSmoothNormals | //
                                          aiProcess_FlipUVs |          //
                                          aiProcess_CalcTangentSpace | //
                                          aiProcess_GenBoundingBoxes);

    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        qCritical() << Q_FUNC_INFO << importer.GetErrorString();
        return nullptr;
    }

    QString directory = path.left(path.lastIndexOf("/"));

    ModelData *data = new ModelData(name);

    // Node
    ModelDataNode *root = processNode(data, aiScene->mRootNode);
    data->setRootNode(root);

    // Materials
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        Material *material = processMaterial(aiScene->mMaterials[i], directory);
        data->addMaterial(material);
    }

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        aiMesh *aiMesh = aiScene->mMeshes[i];
        Mesh *mesh = processMesh(aiMesh);
        mesh->setId(i);
        mesh->setMaterial(data->getMaterial(aiMesh->mMaterialIndex));
        data->addMesh(mesh);
    }

    return data;
}

Canavar::Engine::Mesh *Canavar::Engine::Helper::processMesh(aiMesh *aiMesh)
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

    Mesh::AABB aabb;
    auto min = aiMesh->mAABB.mMin;
    auto max = aiMesh->mAABB.mMax;
    aabb.min = QVector3D(min.x, min.y, min.z);
    aabb.max = QVector3D(max.x, max.y, max.z);
    mesh->setAABB(aabb);

    return mesh;
}

Canavar::Engine::ModelDataNode *Canavar::Engine::Helper::processNode(ModelData *data, aiNode *aiParent)
{
    ModelDataNode *parent = new ModelDataNode(data);

    for (unsigned int i = 0; i < aiParent->mNumMeshes; ++i)
        parent->addMeshIndex(aiParent->mMeshes[i]);

    parent->setName(aiParent->mName.C_Str());
    parent->setWorldTransformation(toQMatrix(aiParent->mTransformation));

    for (unsigned int i = 0; i < aiParent->mNumChildren; ++i)
        parent->addChild(processNode(data, aiParent->mChildren[i]));

    return parent;
}

Canavar::Engine::Material *Canavar::Engine::Helper::processMaterial(aiMaterial *aiMaterial, const QString &directory)
{
    Material *material = new Material;
    processTexture(material, aiMaterial, aiTextureType_AMBIENT, Material::TextureType::Ambient, directory);
    processTexture(material, aiMaterial, aiTextureType_DIFFUSE, Material::TextureType::Diffuse, directory);
    processTexture(material, aiMaterial, aiTextureType_SPECULAR, Material::TextureType::Specular, directory);
    processTexture(material, aiMaterial, aiTextureType_HEIGHT, Material::TextureType::Normal, directory);

    return material;
}

void Canavar::Engine::Helper::processTexture(Material *material, aiMaterial *aiMaterial, aiTextureType aiType, Material::TextureType type, const QString &directory)
{
    for (int i = 0; i < qMin(1, int(aiMaterial->GetTextureCount(aiType))); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);
        QString filename = QString(str.C_Str());
        auto path = directory + "/" + filename;

        if (auto texture = createTexture(path))
            material->insert(type, texture);
    }
}

QMatrix4x4 Canavar::Engine::Helper::toQMatrix(const aiMatrix4x4 &matrix)
{
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    matrix.Decompose(scaling, rotation, position);

    QVector3D qScaling = QVector3D(scaling.x, scaling.y, scaling.z);
    QQuaternion qRotation = QQuaternion(rotation.w, rotation.x, rotation.y, rotation.z);
    QVector4D qPosition = QVector4D(position.x, position.y, position.z, 1.0);

    QMatrix4x4 transformation;

    transformation.scale(qScaling);
    transformation.rotate(qRotation);
    transformation.setColumn(3, qPosition);

    return transformation;
}

float Canavar::Engine::Helper::generateFloat(float bound)
{
    return mGenerator.bounded(bound);
}

QVector3D Canavar::Engine::Helper::generateVec3(float x, float y, float z)
{
    return QVector3D(generateFloat(x), generateFloat(y), generateFloat(z));
}

QOpenGLTexture *Canavar::Engine::Helper::createTexture(const QString &path)
{
    QImage image(path);
    if (image.isNull())
    {
        qWarning() << "An image at " + path + " is null.";
        return nullptr;
    }

    QOpenGLTexture *texture = new QOpenGLTexture(image, QOpenGLTexture::GenerateMipMaps);
    texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    texture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    return texture;
}

QRandomGenerator Canavar::Engine::Helper::mGenerator = QRandomGenerator::securelySeeded();
