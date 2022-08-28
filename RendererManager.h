#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "CameraManager.h"
#include "Common.h"
#include "Framebuffer.h"
#include "Haze.h"
#include "LightManager.h"
#include "ModelData.h"
#include "NodeManager.h"
#include "Quad.h"
#include "ShaderManager.h"
#include "Sky.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "Water.h"

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
    static RendererManager *instance();

    bool init();
    void render(float ifps);
    void resize(int w, int h);

    ModelData *getModelData(const QString &modelName);
    void drawGUI();

private:
    void renderNodes(int up);
    void renderTerrain(int up);
    void renderModel(Model *model, int up);
    void fillFramebuffer(Framebuffer *source, Framebuffer *target);
    void fillFramebufferMultisampled(Framebuffer *source, Framebuffer *target);
    void createFramebuffers();
    void deleteFramebuffers();
    void loadModels();
    void applyBlur(Framebuffer *read, Framebuffer *draw, int times);
    void applyBlurMultisampled(Framebuffer *read, Framebuffer *draw, int times);

private:
    QMap<QString, ModelData *> mModelsData;

    NodeManager *mNodeManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    ShaderManager *mShaderManager;

    Camera *mCamera;
    DirectionalLight *mSun;
    SkyBox *mSkyBox;
    Terrain *mTerrain;
    Haze *mHaze;
    Sky *mSky;
    Quad *mQuad;
    Water *mWater;

    bool mRenderObjects;
    bool mRenderWireframe;
    bool mRenderNormals;
    bool mUseBlinnShading;

    Framebuffer *mFramebuffers[2];
    FramebufferFormat mFramebufferFormat;

    int mWidth;
    int mHeight;

    bool mFlag;
    float mIfps;
};

#endif // RENDERERMANAGER_H
