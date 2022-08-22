#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Haze.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "NozzleEffect.h"
#include "Quad.h"
#include "ShaderManager.h"
#include "SkyBox.h"
#include "Terrain.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>

class RendererManager : public QObject, protected QOpenGLExtraFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    struct Framebuffer {
        unsigned int framebuffer;
        unsigned int texture;
        unsigned int renderObject;
        unsigned int depthTexture;
    };

    struct MotionBlur {
        QMatrix4x4 previousViewProjectionMatrix;
        float strength;
        int samples;
        bool enabled;
    };

    enum class BlurDirection { Horizontal, Vertical };

    static RendererManager *instance();

    bool init();
    void render(float ifps);
    void resize(int w, int h);

    ModelData *getModelData(const QString &modelName);
    void setNozzleModel(ModelData *newNozzleModel);
    void setNozzleEffect(NozzleEffect *newNozzleEffect);
    void drawGui();

private:
    void renderModels(float ifps);
    void renderSkyBox(float ifps);
    void renderTerrain(float ifps);
    void renderParticles(float ifps);
    void renderModel(Model *model);
    void fillFramebuffer(Framebuffer read, Framebuffer draw);
    bool createFramebuffers();
    void deleteFramebuffers();
    void loadModels();
    void applyMotionBlur();
    void fillStencilBuffer(Framebuffer framebuffer, float ifps);
    void applyBlur(Framebuffer stencilSource, Framebuffer textureSource);

private:
    QMap<QString, ModelData *> mModelsData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    DirectionalLight *mSun;

    bool mRenderObjects;
    bool mRenderWireframe;
    bool mRenderNormals;

    SkyBox *mSkyBox;
    Terrain *mTerrain;
    Haze *mHaze;

    bool mUseBlinnShading;

    Quad *mQuad;

    Framebuffer mFramebuffers[4];

    int mWidth;
    int mHeight;

    NozzleEffect *mNozzleEffect;

    bool mFlag;

    MotionBlur mMotionBlur;
};

#endif // RENDERERMANAGER_H
