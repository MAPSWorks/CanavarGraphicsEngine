#include "Gui.h"
#include "Config.h"
#include "Haze.h"
#include "Helper.h"
#include "ModelDataManager.h"
#include "RendererManager.h"
#include "SelectableNodeRenderer.h"

#include <QFileDialog>
#include <QJsonDocument>

Canavar::Engine::Gui::Gui(QObject *parent)
    : QObject(parent)
    , mSelectedNode(nullptr)
    , mSelectedModel(nullptr)
    , mSelectedMesh(nullptr)
    , mSelectedVertexIndex(-1)
    , mDrawAllBBs(false)
    , mNodeSelectionEnabled(false)
    , mMeshSelectionEnabled(false)
    , mVertexSelectionEnabled(false)
    , mWidth(1600)
    , mHeight(900)
    , mMoveEnabled(false)
    , mLockNode(false)
    , mLockAxisX(false)
    , mLockAxisY(false)
    , mLockAxisZ(false)

{
    mCreatableNodeNames << "Dummy Node"
                        << "Model"
                        << "Free Camera"
                        << "Dummy Camera"
                        << "Point Light"
                        << "Nozzle Effect"
                        << "Firecracker Effect"
                        << "Persecutor Camera";

    mNodeManager = NodeManager::instance();

    connect(mNodeManager, &NodeManager::nodeCreated, this, [=](Canavar::Engine::Node *node) {
        if (mDrawAllBBs)
            RendererManager::instance()->addSelectableNode(node, QVector4D(1, 1, 1, 1));
    });

    connect(
        this,
        &Gui::showFileDialog,
        this,
        [=]() { //
            QString path = QFileDialog::getSaveFileName(nullptr, "Save as JSON", "", "*.json");
            if (!path.isEmpty())
            {
                QJsonDocument document;
                QJsonObject object;
                mNodeManager->toJson(object);
                document.setObject(object);
                QByteArray content = document.toJson(QJsonDocument::Indented);
                Helper::writeToFile(path, content);
            }
        },
        Qt::QueuedConnection);
}

void Canavar::Engine::Gui::draw()
{
    // Render Settings
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Render Settings");

        ImGui::SliderFloat("Exposure##RenderSettings", &RendererManager::instance()->getExposure_nonConst(), 0.01f, 2.0f, "%.3f");
        ImGui::SliderFloat("Gamma##RenderSettings", &RendererManager::instance()->getGamma_nonConst(), 0.01f, 4.0f, "%.3f");
        ImGui::SliderInt("Bloom Blur Pass##RenderSettings", &RendererManager::instance()->getBlurPass_nonConst(), 0, 100);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // Create Node
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Create Node");

        // Select a node type
        if (ImGui::BeginCombo("Choose a node type", mSelectedNodeName.toStdString().c_str()))
        {
            for (const auto &name : mCreatableNodeNames)
                if (ImGui::Selectable(name.toStdString().c_str()))
                    mSelectedNodeName = name;

            ImGui::EndCombo();
        }

        if (mSelectedNodeName == "Model")
        {
            const auto &modelNames = ModelDataManager::instance()->getModelNames();

            if (ImGui::BeginCombo("Choose a 3D Model", mSelectedModelName.toStdString().c_str()))
            {
                for (const auto &name : modelNames)
                    if (ImGui::Selectable(name.toStdString().c_str()))
                        mSelectedModelName = name;

                ImGui::EndCombo();
            }
        }

        if (ImGui::Button("Create Node"))
        {
            Node *node = nullptr;

            if (mSelectedNodeName == "Dummy Node")
                node = NodeManager::instance()->createNode(Node::NodeType::DummyNode);
            else if (mSelectedNodeName == "Free Camera")
                node = NodeManager::instance()->createNode(Node::NodeType::FreeCamera);
            else if (mSelectedNodeName == "Dummy Camera")
                node = NodeManager::instance()->createNode(Node::NodeType::DummyCamera);
            else if (mSelectedNodeName == "Point Light")
                node = NodeManager::instance()->createNode(Node::NodeType::PointLight);
            else if (mSelectedNodeName == "Nozzle Effect")
                node = NodeManager::instance()->createNode(Node::NodeType::NozzleEffect);
            else if (mSelectedNodeName == "Firecracker Effect")
                node = NodeManager::instance()->createNode(Node::NodeType::FirecrackerEffect);
            else if (mSelectedNodeName == "Persecutor Camera")
                node = NodeManager::instance()->createNode(Node::NodeType::PersecutorCamera);
            else if (mSelectedNodeName == "Model" && !mSelectedModelName.isEmpty())
                node = NodeManager::instance()->createModel(mSelectedModelName);

            if (node)
            {
                auto position = CameraManager::instance()->activeCamera()->worldPosition();
                auto viewDir = CameraManager::instance()->activeCamera()->getViewDirection();

                node->setWorldPosition(position + 10 * viewDir);
            }
        }

        ImGui::End();
    }

    // Nodes
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Nodes", NULL, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Export World to JSON File"))
                    emit showFileDialog();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        const auto &nodes = NodeManager::instance()->nodes();

        ImGui::Text("Selection");

        // Draw All Bounding Boxes
        if (ImGui::Checkbox("Draw All Bounding Boxes", &mDrawAllBBs))
        {
            if (mDrawAllBBs)
                for (const auto &node : nodes)
                    RendererManager::instance()->addSelectableNode(node, QVector4D(1, 1, 1, 1));
            else
                for (const auto &node : nodes)
                    RendererManager::instance()->removeSelectableNode(node);

            if (mNodeSelectionEnabled)
                if (mSelectedNode)
                    RendererManager::instance()->addSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));
        }

        // Node Selection
        {
            ImGui::BeginDisabled(!Config::instance()->getNodeSelectionEnabled());

            if (ImGui::Checkbox("Node Selection", &mNodeSelectionEnabled))
            {
                if (mNodeSelectionEnabled)
                {
                    RendererManager::instance()->addSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));

                } else
                {
                    if (mDrawAllBBs)
                        RendererManager::instance()->addSelectableNode(mSelectedNode, QVector4D(1, 1, 1, 1));
                    else
                        for (const auto &node : nodes)
                            RendererManager::instance()->removeSelectableNode(node);

                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                    mLockNode = false;
                    setSelectedMesh(nullptr);
                }
            }

            // Lock Node
            ImGui::BeginDisabled(!mSelectedNode);
            ImGui::SameLine();
            ImGui::Checkbox("Lock Node", &mLockNode);
            ImGui::EndDisabled();

            // Mesh Selection
            ImGui::BeginDisabled(!mSelectedModel || !mNodeSelectionEnabled);
            if (ImGui::Checkbox("Mesh Selection", &mMeshSelectionEnabled))
            {
                setSelectedMesh(nullptr);
                mVertexSelectionEnabled = false;
            }
            ImGui::EndDisabled();

            // Vertex Selection
            ImGui::BeginDisabled(!mSelectedMesh || !mMeshSelectionEnabled);
            if (ImGui::Checkbox("Vertex Selection", &mVertexSelectionEnabled))
            {
                RendererManager::instance()->getSelectedMeshParameters_Ref(mSelectedModel).mRenderVertices = true;
                setSelectedVertexIndex(-1);
            }
            ImGui::EndDisabled();

            ImGui::EndDisabled();
        }

        // Move
        ImGui::Text("Move");
        ImGui::Checkbox("Move enabled", &mMoveEnabled);
        ImGui::BeginDisabled(!mMoveEnabled);
        ImGui::Checkbox("Move only on x-Axis", &mLockAxisX);
        ImGui::Checkbox("Move only on y-Axis", &mLockAxisY);
        ImGui::Checkbox("Move only on z-Axis", &mLockAxisZ);
        ImGui::EndDisabled();

        // Select a node
        ImGui::Text("Nodes");

        if (ImGui::BeginCombo("Select a node", mSelectedNode ? mSelectedNode->getName().toStdString().c_str() : "-"))
        {
            for (int i = 0; i < nodes.size(); ++i)
                if (ImGui::Selectable((nodes[i]->getName() + "##" + nodes[i]->getUUID()).toStdString().c_str()))
                {
                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                    setSelectedNode(nodes[i]);
                }

            ImGui::EndCombo();
        }

        // Actions
        if (mSelectedNode)
        {
            if (!ImGui::CollapsingHeader("Actions"))
            {
                ImGui::Text("Type: %d", mSelectedNode->getType());
                ImGui::Text("ID: %d", mSelectedNode->getID());
                ImGui::Text("UUID: %s", mSelectedNode->getUUID().toStdString().c_str());

                // Assign a parent
                {
                    if (ImGui::BeginCombo("Assign a parent", mSelectedNode->parent() ? mSelectedNode->parent()->getName().toStdString().c_str() : "-"))
                    {
                        if (ImGui::Selectable("-"))
                            mSelectedNode->setParent(nullptr);

                        for (int i = 0; i < nodes.size(); ++i)
                        {
                            if (mSelectedNode == nodes[i])
                                continue;

                            if (ImGui::Selectable(nodes[i]->getName().toStdString().c_str()))
                                mSelectedNode->setParent(nodes[i]);
                        }

                        ImGui::EndCombo();
                    }
                }

                if (ImGui::Button("Remove this node"))
                {
                    NodeManager::instance()->removeNode(mSelectedNode);
                    mMeshSelectionEnabled = false;
                    mVertexSelectionEnabled = false;
                }
            }
        }

        if (mSelectedNode)
        {
            switch (mSelectedNode->getType())
            {
            case Canavar::Engine::Node::NodeType::DummyNode:
                draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::PersecutorCamera:
            case Canavar::Engine::Node::NodeType::DummyCamera:
                draw(dynamic_cast<PerspectiveCamera *>(mSelectedNode));
                draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::FreeCamera:
                draw(dynamic_cast<FreeCamera *>(mSelectedNode));
                draw(dynamic_cast<PerspectiveCamera *>(mSelectedNode));
                draw(mSelectedNode);
                break;
            case Canavar::Engine::Node::NodeType::Sky:
                draw(dynamic_cast<Sky *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Sun:
                draw(dynamic_cast<Sun *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Model:
                draw(mSelectedNode);
                draw(dynamic_cast<Model *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Terrain:
                draw(dynamic_cast<Terrain *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::Haze:
                draw(dynamic_cast<Haze *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::PointLight:
                draw(dynamic_cast<Light *>(mSelectedNode));
                draw(dynamic_cast<PointLight *>(mSelectedNode));
                draw(dynamic_cast<Node *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::NozzleEffect:
                draw(dynamic_cast<NozzleEffect *>(mSelectedNode));
                draw(dynamic_cast<Node *>(mSelectedNode));
                break;
            case Canavar::Engine::Node::NodeType::FirecrackerEffect:
                draw(dynamic_cast<FirecrackerEffect *>(mSelectedNode));
                draw(dynamic_cast<Node *>(mSelectedNode));
                break;
            default:
                break;
            }
        }

        ImGui::End();
    }
}

void Canavar::Engine::Gui::draw(Node *node)
{
    // Position
    if (!ImGui::CollapsingHeader("Position##Node"))
    {
        float x = node->position().x();
        float y = node->position().y();
        float z = node->position().z();

        if (ImGui::DragFloat("x##NodePosition", &x, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodePosition", &y, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodePosition", &z, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
    }

    // Rotation
    if (ImGui::CollapsingHeader("Rotation##Node"))
    {
        float yaw, pitch, roll;

        Canavar::Engine::Helper::getEulerDegrees(node->rotation(), yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 360.0f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -180.0f, 180.0f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##Node"))
    {
        QVector3D position = node->worldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::DragFloat("x##NodeWorldPosition", &x, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeWorldPosition", &y, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeWorldPosition", &z, 1.0f, -10e10f, 10e10f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (ImGui::CollapsingHeader("World Rotation##Node"))
    {
        QQuaternion rotation = node->worldRotation();
        float yaw, pitch, roll;

        Helper::getEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 360.0f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -180.0f, 180.0f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // Scale
    if (ImGui::CollapsingHeader("Scale##Node"))
    {
        QVector3D scale = node->scale();
        float x = scale.x();
        float y = scale.y();
        float z = scale.z();
        float all = qMin(x, qMin(y, z));

        if (ImGui::DragFloat("All##NodeScale", &all, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(all, all, all));
        if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
            node->setScale(QVector3D(x, y, z));
    }
}

void Canavar::Engine::Gui::draw(Model *model)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        ImGui::SliderFloat("Ambient##Model", &model->getAmbient_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Model", &model->getDiffuse_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Model", &model->getSpecular_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Model", &model->getShininess_nonConst(), 1.0f, 128.0f, "%.3f");
        ImGui::ColorEdit4("Color##Model", (float *) &model->getColor_nonConst());
        ImGui::SliderFloat("Overlay Color Factor##Model", &model->getOverlayColorFactor_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Overlay Color##Model", (float *) &model->getOverlayColor_nonConst());
    }

    if (!ImGui::CollapsingHeader("Meshes##Model"))
    {
        if (auto data = ModelDataManager::instance()->getModelData(model->getModelName()))
        {
            const auto &meshes = data->meshes();

            if (ImGui::BeginCombo("Select a mesh", mSelectedMesh ? mSelectedMesh->getName().toStdString().c_str() : "-"))
            {
                for (int i = 0; i < meshes.size(); ++i)
                    if (ImGui::Selectable(meshes[i]->getName().toStdString().c_str()))
                        setSelectedMesh(meshes[i]);

                ImGui::EndCombo();
            }

            if (mSelectedMesh)
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh Info");
                ImGui::Text("ID: %d", mSelectedMesh->getID());
                ImGui::Text("Number of Vertices: %d", mSelectedMesh->getNumberOfVertices());

                auto transformation = model->getMeshTransformation(mSelectedMesh->getName());
                auto position = transformation.column(3);
                auto rotation = QQuaternion::fromRotationMatrix(transformation.normalMatrix());

                // Position
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Position");
                ImGui::DragFloat("x##MeshPosition", &position[0], 0.01f, -1000.0f, 1000.0f, "%.3f");
                ImGui::DragFloat("y##MeshPosition", &position[1], 0.01f, -1000.0f, 1000.0f, "%.3f");
                ImGui::DragFloat("z##MeshPosition", &position[2], 0.01f, -1000.0f, 1000.0f, "%.3f");

                // Rotation
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Rotation");
                float yaw, pitch, roll;

                Canavar::Engine::Helper::getEulerDegrees(rotation, yaw, pitch, roll);

                if (ImGui::SliderFloat("Yaw##MeshRotation", &yaw, 0.0f, 360.0f, "%.3f"))
                    rotation = Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll);
                if (ImGui::SliderFloat("Pitch##MeshRotation", &pitch, -89.999f, 89.999f, "%.3f"))
                    rotation = Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll);
                if (ImGui::SliderFloat("Roll##MeshRotation", &roll, -180.0f, 180.0f, "%.3f"))
                    rotation = Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll);

                transformation.setToIdentity();
                transformation.setColumn(3, position);
                transformation.rotate(rotation);
                model->setMeshTransformation(mSelectedMesh->getName(), transformation);

                if (mSelectedVertexIndex != -1)
                {
                    Mesh::Vertex vertex = mSelectedMesh->getVertex(mSelectedVertexIndex);
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Vertex Info");
                    ImGui::Text("Selected vertex index: %d", mSelectedVertexIndex);
                    ImGui::Text("Selected vertex position: (%.4f, %.4f, %.4f)", vertex.position[0], vertex.position[1], vertex.position[2]);
                }
            }
        }
    }
}

void Canavar::Engine::Gui::draw(Sky *node)
{
    if (!ImGui::CollapsingHeader("Sky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &node->getAlbedo_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &node->getTurbidity_nonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &node->getNormalizedSunY_nonConst(), 0.0f, 10.0f, "%.3f");
    }
}

void Canavar::Engine::Gui::draw(Sun *sun)
{
    if (!ImGui::CollapsingHeader("Sun"))
    {
        ImGui::Text("Direction:");
        float x = sun->getDirection().x();
        float y = sun->getDirection().y();
        float z = sun->getDirection().z();
        float r = sun->getDirection().length();
        float theta = qRadiansToDegrees(atan2(z, x));
        float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

        if (qFuzzyCompare(abs(phi), 90.0f))
            theta = 0.0f;

        bool updated = false;

        if (ImGui::SliderFloat("Theta##DirectionalLight", &theta, -180.0f, 180.0f, "%.1f"))
            updated = true;

        if (ImGui::SliderFloat("Phi##DirectionalLight", &phi, -90.0f, 90.0f, "%.1f"))
            updated = true;

        if (updated)
        {
            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            sun->setDirection(QVector3D(x, y, z));
        }

        ImGui::Text("Shading Parameters:");
        ImGui::SliderFloat("Ambient##Sun", &sun->getAmbient_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Sun", &sun->getDiffuse_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Sun", &sun->getSpecular_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##Sun", (float *) &sun->getColor_nonConst());
    }
}

void Canavar::Engine::Gui::draw(PerspectiveCamera *camera)
{
    if (!ImGui::CollapsingHeader("Parameters##PerspectiveCamera"))
    {
        ImGui::SliderFloat("FOV##PerspectiveCamera", &camera->getVerticalFov_nonConst(), 1.0f, 120.0);
        ImGui::SliderFloat("Z-Near##PerspectiveCamera", &camera->getZNear_nonConst(), 0.1f, 100.0f);
        ImGui::SliderFloat("Z-Far##PerspectiveCamera", &camera->getZFar_nonConst(), 1000.0f, 1000000.0f);
    }
}

void Canavar::Engine::Gui::draw(FreeCamera *node)
{
    if (!ImGui::CollapsingHeader("Speed##Free Camera"))
    {
        ImGui::SliderFloat("Linear Speed##FreeCamera", &node->getSpeed_nonConst().mLinear, 0.1f, 10000.0f);
        ImGui::SliderFloat("Linear Speed Multiplier##FreeCamera", &node->getSpeed_nonConst().mLinearMultiplier, 0.1f, 10.0f);
        ImGui::SliderFloat("Angular Speed##FreeCamera", &node->getSpeed_nonConst().mAngular, 0.1f, 100.0f);
        ImGui::SliderFloat("Angular Speed Multiplier##FreeCamera", &node->getSpeed_nonConst().mAngularMultiplier, 0.1f, 10.0f);
    }
}

void Canavar::Engine::Gui::draw(Terrain *node)
{
    if (!ImGui::CollapsingHeader("Terrain"))
    {
        ImGui::SliderFloat("Amplitude##Terrain", &node->getAmplitude_nonConst(), 0.0f, 50.0f, "%.3f");
        ImGui::SliderInt("Octaves##Terrain", &node->getOctaves_nonConst(), 1, 20);
        ImGui::SliderFloat("Power##Terrain", &node->getPower_nonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Tessellation Multiplier##Terrain", &node->getTessellationMultiplier_nonConst(), 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Grass Coverage##Terrain", &node->getGrassCoverage_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Ambient##Terrain", &node->getAmbient_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Terrain", &node->getDiffuse_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Terrain", &node->getSpecular_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Terrain", &node->getShininess_nonConst(), 0.1f, 128.0f, "%.3f");
        ImGui::Checkbox("Enabled", &node->getEnabled_nonConst());

        if (ImGui::Button("Generate Seed##Terrain"))
            node->setSeed(Canavar::Engine::Helper::generateVec3(1, 1, 1));

        if (ImGui::Button("Reset##Terrain"))
            node->reset();
    }
}

void Canavar::Engine::Gui::draw(Light *node)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Light"))
    {
        ImGui::SliderFloat("Ambient##Light", &node->getAmbient_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Light", &node->getDiffuse_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Light", &node->getSpecular_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##Light", (float *) &node->getColor_nonConst());
    }
}

void Canavar::Engine::Gui::draw(PointLight *node)
{
    if (!ImGui::CollapsingHeader("Attenuation##PointLight"))
    {
        ImGui::SliderFloat("Constant##PointLight", &node->getConstant_nonConst(), 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Linear##PointLight", &node->getLinear_nonConst(), 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Quadratic##PointLight", &node->getQuadratic_nonConst(), 0.00001f, 0.1f, "%.6f");
    }
}

void Canavar::Engine::Gui::draw(Haze *node)
{
    if (!ImGui::CollapsingHeader("Haze##Haze"))
    {
        ImGui::SliderFloat("Density##Haze", &node->getDensity_nonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::SliderFloat("Gradient##Haze", &node->getGradient_nonConst(), 0.0f, 4.0f, "%.3f");
        ImGui::ColorEdit4("Color##Haze", (float *) &node->getColor_nonConst());
    }
}

void Canavar::Engine::Gui::draw(NozzleEffect *node)
{
    if (!ImGui::CollapsingHeader("Nozzle Effect Parameters##NozzleEffect"))
    {
        ImGui::SliderFloat("Max Radius##NozzleEffect", &node->getMaxRadius_nonConst(), 0.001f, 4.0f, "%.4f");
        ImGui::SliderFloat("Max Life##NozzleEffect", &node->getMaxLife_nonConst(), 0.0000f, 0.1f, "%.5f");
        ImGui::SliderFloat("Max Distance##NozzleEffect", &node->getMaxDistance_nonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Min Distance##NozzleEffect", &node->getMinDistance_nonConst(), 1.0f, 30.0f, "%.3f");
        ImGui::SliderFloat("Speed##NozzleEffect", &node->getSpeed_nonConst(), 0.0f, 10.0f, "%.5f");
        ImGui::SliderFloat("Scale##NozzleEffect", &node->getScale_nonConst(), 0.001f, 0.1f, "%.4f");
    }
}

void Canavar::Engine::Gui::draw(FirecrackerEffect *node)
{
    if (!ImGui::CollapsingHeader("Firecracker Effect Parameters##Firecracker"))
    {
        ImGui::SliderFloat("Span Angle##Firecracker", &node->getSpanAngle_nonConst(), 0.0f, 180.0f, "%.1f");
        ImGui::SliderFloat("Gravity##Firecracker", &node->getGravity_nonConst(), 0.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Max Life##Firecracker", &node->getMaxLife_nonConst(), 0.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Min Life##Firecracker", &node->getMinLife_nonConst(), 0.0f, 20.0f, "%.2f");
        ImGui::SliderFloat("Initial Speed##Firecracker", &node->getInitialSpeed_nonConst(), 0.0f, 2000.0f, "%.1f");
        ImGui::SliderFloat("Damping##Firecracker", &node->getDamping_nonConst(), 0.000f, 10.0f, "%.2f");
        ImGui::SliderFloat("Scale##Firecracker", &node->getScale_nonConst(), 0.001f, 10.0f, "%.4f");
        ImGui::Checkbox("Loop##Firecracker", &node->getLoop_nonConst());
    }
}

Canavar::Engine::Node *Canavar::Engine::Gui::getSelectedNode() const
{
    return mSelectedNode;
}

void Canavar::Engine::Gui::setSelectedNode(Canavar::Engine::Node *newSelectedNode)
{
    setSelectedMesh(nullptr);
    mSelectedModel = dynamic_cast<Model *>(newSelectedNode);

    if (mSelectedNode)
    {
        mSelectedNode->disconnect(this);

        if (mDrawAllBBs)
            RendererManager::instance()->addSelectableNode(mSelectedNode, QVector4D(1, 1, 1, 1));
        else
            RendererManager::instance()->removeSelectableNode(mSelectedNode);
    }

    mSelectedNode = newSelectedNode;

    if (mSelectedNode)
    {
        if (mNodeSelectionEnabled)
            RendererManager::instance()->addSelectableNode(mSelectedNode, QVector4D(1, 0, 0, 1));

        connect(mSelectedNode, &QObject::destroyed, this, [=]() { setSelectedNode(nullptr); });
    }
}

Canavar::Engine::Mesh *Canavar::Engine::Gui::getSelectedMesh() const
{
    return mSelectedMesh;
}

void Canavar::Engine::Gui::setSelectedMesh(Canavar::Engine::Mesh *newSelectedMesh)
{
    if (mSelectedMesh)
        RendererManager::instance()->removeSelectedMesh(mSelectedModel);

    mSelectedMesh = newSelectedMesh;

    if (mSelectedMesh)
    {
        SelectedMeshParameters parameters;
        parameters.mMesh = mSelectedMesh;

        RendererManager::instance()->addSelectedMesh(mSelectedModel, parameters);
    }
}

int Canavar::Engine::Gui::getSelectedVertexIndex() const
{
    return mSelectedVertexIndex;
}

void Canavar::Engine::Gui::setSelectedVertexIndex(int newSelectedVertexIndex)
{
    mSelectedVertexIndex = newSelectedVertexIndex;

    if (mSelectedModel)
        RendererManager::instance()->getSelectedMeshParameters_Ref(mSelectedModel).mSelectedVertexID = mSelectedVertexIndex;
}

void Canavar::Engine::Gui::mousePressed(QMouseEvent *event)
{
    auto info = SelectableNodeRenderer::instance()->getNodeInfoByScreenPosition(event->position().x(), event->position().y());
    mMouse.mPressedButton = NodeManager::instance()->getNodeByID(info.nodeID) == mSelectedNode ? event->button() : Qt::NoButton;
    mMouse.mX = event->position().x();
    mMouse.mY = event->position().y();

    if (event->button() != Qt::LeftButton)
        return;

    if (mVertexSelectionEnabled)
    {
        auto info = SelectableNodeRenderer::instance()->getVertexInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            if (mSelectedModel == NodeManager::instance()->getNodeByID(info.nodeID))
                if (auto data = ModelDataManager::instance()->getModelData(mSelectedModel->getModelName()))
                    if (data->getMeshByID(info.meshID) == mSelectedMesh)
                        setSelectedVertexIndex(info.vertexID);

    } else if (mMeshSelectionEnabled)
    {
        auto info = SelectableNodeRenderer::instance()->getNodeInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            if (mSelectedModel == NodeManager::instance()->getNodeByID(info.nodeID))
                if (auto data = ModelDataManager::instance()->getModelData(mSelectedModel->getModelName()))
                    setSelectedMesh(data->getMeshByID(info.meshID));

    } else if (mNodeSelectionEnabled && !mLockNode)
    {
        auto info = SelectableNodeRenderer::instance()->getNodeInfoByScreenPosition(event->position().x(), event->position().y());

        if (info.success)
            setSelectedNode(NodeManager::instance()->getNodeByID(info.nodeID));
        else
            setSelectedNode(nullptr);
    }
}

void Canavar::Engine::Gui::mouseMoved(QMouseEvent *event)
{
    if (mMoveEnabled && mNodeSelectionEnabled && mSelectedNode && mMouse.mPressedButton == Qt::LeftButton)
    {
        auto vp = CameraManager::instance()->activeCamera()->getViewProjectionMatrix();
        auto ssc = vp * QVector4D(mSelectedNode->worldPosition(), 1);

        QVector4D prevMouseWorldPos;
        QVector4D currMouseWorldPos;

        {
            float x = (mMouse.mX - 0.5f * mWidth) / (0.5f * mWidth);
            float y = (0.5f * mHeight - mMouse.mY) / (0.5f * mHeight);
            float z = ssc.z();
            float w = ssc.w();
            prevMouseWorldPos = vp.inverted() * QVector4D(x * w, y * w, z, w);

            mMouse.mX = event->position().x();
            mMouse.mY = event->position().y();
        }

        {
            float x = (event->position().x() - 0.5f * mWidth) / (0.5f * mWidth);
            float y = (0.5f * mHeight - event->position().y()) / (0.5f * mHeight);
            float z = ssc.z();
            float w = ssc.w();
            currMouseWorldPos = vp.inverted() * QVector4D(x * w, y * w, z, w);
        }

        auto delta = (currMouseWorldPos - prevMouseWorldPos).toVector3D();

        QVector3D newWorldPos = mSelectedNode->worldPosition();

        if (mLockAxisX)
            newWorldPos += QVector3D(delta.x(), 0, 0);

        if (mLockAxisY)
            newWorldPos += QVector3D(0, delta.y(), 0);

        if (mLockAxisZ)
            newWorldPos += QVector3D(0, 0, delta.z());

        if (!mLockAxisX && !mLockAxisY && !mLockAxisZ)
            newWorldPos += delta;

        mSelectedNode->setWorldPosition(newWorldPos);
    }
}

void Canavar::Engine::Gui::mouseReleased(QMouseEvent *event)
{
    mMouse.mPressedButton = Qt::NoButton;
}

void Canavar::Engine::Gui::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;
}
