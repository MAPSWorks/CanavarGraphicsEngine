#ifndef SKY_H
#define SKY_H

#include "CameraManager.h"
#include "DirectionalLight.h"
#include "Framebuffer.h"
#include "Quad.h"
#include "ShaderManager.h"
#include "Texture.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QVector3D>
#include <QVector4D>

#define INT_CEIL(n, d) (int) ceil((float) n / d)

class Sky : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit Sky(QObject *parent = nullptr);

    struct InputTextureSet {
        Texture *weather;
        Texture *perlin;
        Texture *worley;
    };

    struct OutputTextureSet {
        Texture *fragColor;
        Texture *bloom;
        Texture *alphaness;
        Texture *cloudDistance;
    };

    void resize(int width, int height);
    void render(float ifps);
    void drawGui();
    void reset();

    const OutputTextureSet &outputTextures() const;

    Framebuffer *skyBoxFramebuffer() const;

private:
    void generateMaps();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    DirectionalLight *mSun;
    Camera *mCamera;
    Quad *mQuad;

    QVector3D mSkyColorTop;
    QVector3D mSkyColorBottom;

    QVector3D mCloudColor;
    QVector3D mCloudFlareColor;

    float mCloudSpeed;
    float mCoverage;
    float mCrispiness;
    float mCurliness;
    float mDensity;
    float mAbsorption;

    float mEarthRadius;
    float mSphereInnerRadius;
    float mSphereOuterRadius;

    float mPerlinFrequency;
    bool mEnablePower;
    QVector3D mSeed;

    InputTextureSet mInputTextures;
    OutputTextureSet mOutputTextures;

    Framebuffer *mSkyBoxFramebuffer;
    FramebufferFormat mSkyBoxFramebufferFormat;
    float mTimeElapsed;

    int mWidth;
    int mHeight;
};

#endif // SKY_H
