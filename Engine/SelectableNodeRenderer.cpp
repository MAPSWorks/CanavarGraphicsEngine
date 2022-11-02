#include "SelectableNodeRenderer.h"

#include "CameraManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "ShaderManager.h"

#include <QTimer>

Canavar::Engine::SelectableNodeRenderer::SelectableNodeRenderer()
    : Manager()
    , mWidth(1600)
    , mHeight(900)
    , mResizeLater(true)
{}

bool Canavar::Engine::SelectableNodeRenderer::init()
{
    mShaderManager = ShaderManager::instance();
    mNodeManager = NodeManager::instance();
    mModelDataManager = ModelDataManager::instance();
    mCameraManager = CameraManager::instance();

    initializeOpenGLFunctions();
    mNodeInfoFBO.init();
    mNodeInfoFBO.create(mWidth, mHeight);

    glGenVertexArrays(1, &mCube.mVAO);
    glBindVertexArray(mCube.mVAO);
    glGenBuffers(1, &mCube.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCube.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    return true;
}

void Canavar::Engine::SelectableNodeRenderer::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    if (mResizeLater)
    {
        QTimer::singleShot(100, [=]() {
            mNodeInfoFBO.destroy();
            mNodeInfoFBO.create(mWidth, mHeight);
            mResizeLater = true;
        });

        mResizeLater = false;
    }
}

Canavar::Engine::SelectableNodeRenderer::NodeInfo Canavar::Engine::SelectableNodeRenderer::getNodeInfoByScreenPosition(int x, int y)
{
    mNodeInfoFBO.bind();
    NodeInfo info;
    glReadPixels(x, mHeight - y, 1, 1, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &info);
    mNodeInfoFBO.release();

    return info;
}

void Canavar::Engine::SelectableNodeRenderer::render(float)
{
    mNodeInfoFBO.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    for (const auto &node : mNodeManager->nodes())
    {
        if (!node->getVisible() || !node->getSelectable())
            continue;

        if (auto model = dynamic_cast<Model *>(node))
        {
            if (auto data = mModelDataManager->getModelData(model->getModelName()))
                data->render(RenderMode::NodeInfo, model);
        } else
        {
            mShaderManager->bind(ShaderType::NodeInfoShader);
            mShaderManager->setUniformValue("MVP", mCameraManager->activeCamera()->getViewProjectionMatrix() * node->worldTransformation() * node->getAABB().getTransformation());
            mShaderManager->setUniformValue("nodeID", node->getID());
            mShaderManager->setUniformValue("meshID", 0);
            glBindVertexArray(mCube.mVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            mShaderManager->release();
        }
    }

    mNodeInfoFBO.release();
}

Canavar::Engine::SelectableNodeRenderer *Canavar::Engine::SelectableNodeRenderer::instance()
{
    static SelectableNodeRenderer instance;
    return &instance;
}
