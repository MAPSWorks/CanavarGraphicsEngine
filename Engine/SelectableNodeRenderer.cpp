#include "SelectableNodeRenderer.h"

#include "CameraManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "ShaderManager.h"

Canavar::Engine::SelectableNodeRenderer::SelectableNodeRenderer(QObject *parent)
    : Manager(parent)
    , mWidth(1600)
    , mHeight(900)
{}

bool Canavar::Engine::SelectableNodeRenderer::init()
{
    mShaderManager = ShaderManager::instance();
    mNodeManager = NodeManager::instance();
    mModelDataManager = ModelDataManager::instance();
    mCameraManager = CameraManager::instance();

    initializeOpenGLFunctions();
    mNodeInfoFBO.create(mWidth, mHeight);

    return true;
}

void Canavar::Engine::SelectableNodeRenderer::resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    mNodeInfoFBO.destroy();
    mNodeInfoFBO.create(width, height);
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
            glBindVertexArray(mModelDataManager->mCube.mVAO);
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

void Canavar::Engine::SelectableNodeRenderer::add(Node *node)
{
    if (node)
    {
        mRenderList << node;
        connect(node, &QObject::destroyed, this, &SelectableNodeRenderer::onObjectDestroyed);
    }
}

void Canavar::Engine::SelectableNodeRenderer::remove(Node *node)
{
    if (node)
    {
        disconnect(node, &QObject::destroyed, this, &SelectableNodeRenderer::onObjectDestroyed);
        mRenderList.removeAll(node);
    }
}

void Canavar::Engine::SelectableNodeRenderer::onObjectDestroyed(QObject *object)
{
    mRenderList.removeAll(object);
}

const QList<Canavar::Engine::Node *> &Canavar::Engine::SelectableNodeRenderer::renderList() const
{
    return mRenderList;
}
