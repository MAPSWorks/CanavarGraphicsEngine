#include "Sky.h"
#include "HosekData.h"
#include "HosekFunctions.h"
#include "Sun.h"

// Sky model belongs to Hosek-Wilkie - An Analytic Model for Full Spectral Sky-Dome Radiance
// Code is taken from https://github.com/diharaw/sky-models

Canavar::Engine::Sky::Sky()
    : Node()
    , mEnabled(true)
    , mAlbedo(0.1f)
    , mTurbidity(4.0f)
    , mNormalizedSunY(1.15f)
{
    mShaderManager = Canavar::Engine::ShaderManager::instance();
    mCameraManager = Canavar::Engine::CameraManager::instance();
    mLightManager = Canavar::Engine::LightManager::instance();

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::QUAD), Canavar::Engine::QUAD, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector2D), (void *) 0);

    mType = Node::NodeType::Sky;
    mName = "Sky";
    mSelectable = false;
}

Canavar::Engine::Sky *Canavar::Engine::Sky::instance()
{
    static Sky instance;
    return &instance;
}

void Canavar::Engine::Sky::toJson(QJsonObject &object)
{
    Node::toJson(object);

    object.insert("enabled", mEnabled);
    object.insert("albedo", mAlbedo);
    object.insert("turbidity", mTurbidity);
    object.insert("normalized_sun_y", mNormalizedSunY);
}

void Canavar::Engine::Sky::fromJson(const QJsonObject &object)
{
    Node::fromJson(object);

    mEnabled = object["enabled"].toBool();
    mAlbedo = object["albedo"].toDouble();
    mTurbidity = object["turbidity"].toDouble();
    mNormalizedSunY = object["normalized_sun_y"].toDouble();
}

void Canavar::Engine::Sky::render()
{
    const auto sunDir = -Sun::instance()->getDirection().normalized();
    const auto sunTheta = acos(qBound(0.f, sunDir.y(), 1.f));

    for (int i = 0; i < 3; ++i)
    {
        A[i] = evaluate(datasetsRGB[i] + 0, 9, mTurbidity, mAlbedo, sunTheta);
        B[i] = evaluate(datasetsRGB[i] + 1, 9, mTurbidity, mAlbedo, sunTheta);
        C[i] = evaluate(datasetsRGB[i] + 2, 9, mTurbidity, mAlbedo, sunTheta);
        D[i] = evaluate(datasetsRGB[i] + 3, 9, mTurbidity, mAlbedo, sunTheta);
        E[i] = evaluate(datasetsRGB[i] + 4, 9, mTurbidity, mAlbedo, sunTheta);
        F[i] = evaluate(datasetsRGB[i] + 5, 9, mTurbidity, mAlbedo, sunTheta);
        G[i] = evaluate(datasetsRGB[i] + 6, 9, mTurbidity, mAlbedo, sunTheta);

        // Swapped in the dataset
        H[i] = evaluate(datasetsRGB[i] + 8, 9, mTurbidity, mAlbedo, sunTheta);
        I[i] = evaluate(datasetsRGB[i] + 7, 9, mTurbidity, mAlbedo, sunTheta);

        Z[i] = evaluate(datasetsRGBRad[i], 1, mTurbidity, mAlbedo, sunTheta);
    }

    if (mNormalizedSunY)
    {
        QVector3D S = hosekWilkie(cos(sunTheta), 0, 1.f, A, B, C, D, E, F, G, H, I) * Z;
        Z /= QVector3D::dotProduct(S, QVector3D(0.2126, 0.7152, 0.0722));
        Z *= mNormalizedSunY;
    }

    glDisable(GL_DEPTH_TEST);

    mShaderManager->bind(ShaderType::SkyShader);
    mShaderManager->setUniformValue("IVP", mCameraManager->activeCamera()->getRotationMatrix().inverted() * mCameraManager->activeCamera()->getProjectionMatrix().inverted());
    mShaderManager->setUniformValue("sunDir", sunDir);
    mShaderManager->setUniformValue("A", A);
    mShaderManager->setUniformValue("B", B);
    mShaderManager->setUniformValue("C", C);
    mShaderManager->setUniformValue("D", D);
    mShaderManager->setUniformValue("E", E);
    mShaderManager->setUniformValue("F", F);
    mShaderManager->setUniformValue("G", G);
    mShaderManager->setUniformValue("H", H);
    mShaderManager->setUniformValue("I", I);
    mShaderManager->setUniformValue("Z", Z);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mShaderManager->release();

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}
