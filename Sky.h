#ifndef SKY_H
#define SKY_H

#include "CameraManager.h"
#include "DirectionalLight.h"
#include "Framebuffer.h"
#include "Haze.h"
#include "PerspectiveCamera.h"
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
private:
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

public:
    void render(const RenderSettings &settings);
    void renderWeather(const RenderSettings &settings);
    void resize(int width, int height);
    void drawGUI();
    void reset();

    static Sky *instance();

private:
    void generateMaps();

private:
    ShaderManager *mShaderManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;

    DirectionalLight *mSun;
    PerspectiveCamera *mCamera;
    Quad *mQuad;
    Haze *mHaze;

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

    Framebuffer *mSkyFramebuffer;
    FramebufferFormat mSkyFramebufferFormat;
    float mTimeElapsed;

    int mWidth;
    int mHeight;
};

#endif // SKY_H
