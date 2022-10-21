#include "Gui.h"
#include "Helper.h"

Canavar::Engine::Gui::Gui() {}

Canavar::Engine::Node *Canavar::Engine::Gui::mSelectedNode = nullptr;

void Canavar::Engine::Gui::draw()
{
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Nodes");

    auto &nodes = NodeManager::instance()->nodes();

    QString preview = mSelectedNode ? mSelectedNode->name() : "-";
    if (ImGui::BeginCombo("Select a node", preview.toStdString().c_str()))
    {
        for (int i = 0; i < nodes.size(); ++i)
            if (ImGui::Selectable(nodes[i]->name().toStdString().c_str()))
                mSelectedNode = nodes[i];

        ImGui::EndCombo();
    }

    if (mSelectedNode)
    {
        ImGui::Text("Type: %d", mSelectedNode->type());
        ImGui::Text("Parent: 0x%p", mSelectedNode->parent());

        if (mSelectedNode->parent())
            ImGui::Text("Parent Name: %s", mSelectedNode->parent()->name().toStdString().c_str());

        switch (mSelectedNode->type())
        {
        case Canavar::Engine::Node::NodeType::DummyNode:
            draw(mSelectedNode);
            break;
        case Canavar::Engine::Node::NodeType::FreeCamera:
        case Canavar::Engine::Node::NodeType::DummyCamera:
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
        default:
            break;
        }
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void Canavar::Engine::Gui::draw(Node *node)
{
    // Position
    if (!ImGui::CollapsingHeader("Position##Node"))
    {
        float x = node->position().x();
        float y = node->position().y();
        float z = node->position().z();

        if (ImGui::DragFloat("x##NodePosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodePosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodePosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setPosition(QVector3D(x, y, z));
    }

    // Rotation
    if (ImGui::CollapsingHeader("Rotation##Node"))
    {
        float yaw, pitch, roll;

        Canavar::Engine::Helper::getEulerDegrees(node->rotation(), yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
            node->setRotation(Canavar::Engine::Helper::constructFromEulerDegrees(yaw, pitch, roll));
    }

    // World Position
    if (ImGui::CollapsingHeader("World Position##Node"))
    {
        QVector3D position = node->worldPosition();
        float x = position.x();
        float y = position.y();
        float z = position.z();

        if (ImGui::DragFloat("x##NodeWorldPosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("y##NodeWorldPosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
        if (ImGui::DragFloat("z##NodeWorldPosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
            node->setWorldPosition(QVector3D(x, y, z));
    }

    // World rotation
    if (ImGui::CollapsingHeader("World Rotation##Node"))
    {
        QQuaternion rotation = node->worldRotation();
        float yaw, pitch, roll;

        Helper::getEulerDegrees(rotation, yaw, pitch, roll);

        if (ImGui::SliderFloat("Yaw##NodeRotation", &yaw, 0.0f, 359.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Pitch##NodeRotation", &pitch, -89.999f, 89.999f, "%.3f"))
            node->setWorldRotation(Helper::constructFromEulerDegrees(yaw, pitch, roll));
        if (ImGui::SliderFloat("Roll##NodeRotation", &roll, -179.999f, 179.999f, "%.3f"))
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

void Canavar::Engine::Gui::draw(Model *node)
{
    if (!ImGui::CollapsingHeader("Shading Parameters##Model"))
    {
        float ambient = node->ambient();
        float diffuse = node->diffuse();
        float specular = node->specular();
        float shininess = node->shininess();
        auto color = node->color();

        if (ImGui::SliderFloat("Ambient##Model", &ambient, 0.0f, 1.0f, "%.3f"))
            node->setAmbient(ambient);
        if (ImGui::SliderFloat("Diffuse##Model", &diffuse, 0.0f, 1.0f, "%.3f"))
            node->setDiffuse(diffuse);
        if (ImGui::SliderFloat("Specular##Model", &specular, 0.0f, 1.0f, "%.3f"))
            node->setSpecular(specular);
        if (ImGui::SliderFloat("Shininess##Model", &shininess, 1.0f, 128.0f, "%.3f"))
            node->setShininess(shininess);
        if (ImGui::ColorEdit4("Color##Model", (float *) &color))
            node->setColor(color);
    }
}

void Canavar::Engine::Gui::draw(Sky *node)
{
    if (!ImGui::CollapsingHeader("Sky"))
    {
        ImGui::SliderFloat("Albedo##Sky", &node->mAlbedo, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Turbidity##Sky", &node->mTurbidity, 0.0f, 10.0f, "%.3f");
        ImGui::SliderFloat("Normalized Sun Y##Sun", &node->mNormalizedSunY, 0.0f, 10.0f, "%.3f");
    }
}

void Canavar::Engine::Gui::draw(Sun *sun)
{
    if (!ImGui::CollapsingHeader("Sun"))
    {
        ImGui::Text("Direction:");
        float x = sun->direction().x();
        float y = sun->direction().y();
        float z = sun->direction().z();
        float r = sun->direction().length();
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
        float ambient = sun->ambient();
        float diffuse = sun->diffuse();
        float specular = sun->specular();
        auto color = sun->color();

        if (ImGui::SliderFloat("Ambient##Sun", &ambient, 0.0f, 1.0f, "%.3f"))
            sun->setAmbient(ambient);
        if (ImGui::SliderFloat("Diffuse##Sun", &diffuse, 0.0f, 1.0f, "%.3f"))
            sun->setDiffuse(diffuse);
        if (ImGui::SliderFloat("Specular##Sun", &specular, 0.0f, 1.0f, "%.3f"))
            sun->setSpecular(specular);
        if (ImGui::ColorEdit4("Color##Sun", (float *) &color))
            sun->setColor(color);
    }
}

void Canavar::Engine::Gui::draw(PerspectiveCamera *camera)
{
    if (!ImGui::CollapsingHeader("Parameters##PerspectiveCamera"))
    {
        auto fov = camera->verticalFov();
        auto zNear = camera->zNear();
        auto zFar = camera->zFar();

        if (ImGui::SliderFloat("FOV##PerspectiveCamera", &fov, 1.0f, 120.0))
            camera->setVerticalFov(fov);
        if (ImGui::SliderFloat("Z-Near##PerspectiveCamera", &zNear, 0.1f, 100.0f))
            camera->setZNear(zNear);
        if (ImGui::SliderFloat("Z-Far##PerspectiveCamera", &zFar, 1000.0f, 1000000.0f))
            camera->setZFar(zFar);
    }
}

void Canavar::Engine::Gui::draw(Terrain *node)
{
    if (!ImGui::CollapsingHeader("Terrain"))
    {
        ImGui::SliderFloat("Amplitude##Terrain", &node->mAmplitude, 0.0f, 50.0f, "%.3f");
        ImGui::SliderInt("Octaves##Terrain", &node->mOctaves, 1, 20);
        ImGui::SliderFloat("Power##Terrain", &node->mPower, 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Tessellation Multiplier##Terrain", &node->mTessellationMultiplier, 0.1f, 10.0f, "%.3f");
        ImGui::SliderFloat("Grass Coverage##Terrain", &node->mGrassCoverage, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Ambient##Terrain", &node->mAmbient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Terrain", &node->mDiffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Terrain", &node->mSpecular, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Shininess##Terrain", &node->mShininess, 0.1f, 128.0f, "%.3f");

        if (ImGui::Button("Generate Seed##Terrain"))
            node->mSeed = Canavar::Engine::Helper::generateVec3(1, 1, 1);

        if (ImGui::Button("Reset##Terrain"))
            node->reset();
    }
}
