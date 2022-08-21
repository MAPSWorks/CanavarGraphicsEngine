#include "Window.h"
#include "qmath.h"

#include <imgui.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mSelectedNode(nullptr)
{
    mRendererManager = RendererManager::instance();
    mLightManager = LightManager::instance();
    mNodeManager = NodeManager::instance();
    mTerrain = Terrain::instance();
    mRandomGenerator = QRandomGenerator::securelySeeded();

    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    QtImGui::initialize(this);

    emit init();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;
}

void Window::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());

    emit resized(w, h);
}

void Window::paintGL()
{
    mSun = mLightManager->directionalLight();
    mNodes = mNodeManager->nodes();

    mAileron = mAircraftController->aileron();
    mElevator = mAircraftController->elevator();
    mRudder = mAircraftController->rudder();
    mThrottle = mAircraftController->throttle();

    mRenderObjects = mRendererManager->renderObjects();
    mRenderWireframe = mRendererManager->renderWireframe();
    mRenderNormals = mRendererManager->renderNormals();
    mUseBlinnShading = mRendererManager->useBlinnShading();

    mFog = mRendererManager->fog();
    mTerrainProperties = mTerrain->properties();
    mTerrainMaterial = mTerrain->material();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit render(ifps);

    mImguiWantCapture = ImGui::GetIO().WantCaptureMouse;

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Graphics");

    // Render Settings
    if (!ImGui::CollapsingHeader("Render Settings"))
    {
        if (ImGui::Checkbox("Render Objects", &mRenderObjects))
            mRendererManager->setRenderObjects(mRenderObjects);

        if (ImGui::Checkbox("Wireframe", &mRenderWireframe))
            mRendererManager->setRenderWireframe(mRenderWireframe);

        if (ImGui::Checkbox("Render Normals", &mRenderNormals))
            mRendererManager->setRenderNormals(mRenderNormals);

        if (ImGui::Checkbox("Use Blinn Shading", &mUseBlinnShading))
            mRendererManager->setUseBlinnShading(mUseBlinnShading);
    }

    // Light
    if (!ImGui::CollapsingHeader("Sun"))
    {
        {
            ImGui::Text("Direction:");
            QVector3D direction = mSun->direction();
            float x = direction.x();
            float y = direction.y();
            float z = direction.z();
            float r = direction.length();
            float theta = qRadiansToDegrees(atan2(z, x));
            float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));
            if (qFuzzyCompare(abs(phi), 90.0f))
                theta = 0.0f;
            ImGui::SliderFloat("Theta", &theta, -180.0f, 180.0f, "%.1f");
            ImGui::SliderFloat("Phi", &phi, -90.0f, 90.0f, "%.1f");

            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            mSun->setDirection(QVector3D(x, y, z));
        }

        {
            ImGui::Text("Shading Parameters:");
            float ambient = mSun->ambient();
            float diffuse = mSun->diffuse();
            float specular = mSun->specular();

            if (ImGui::SliderFloat("Ambient##Sun", &ambient, 0.0f, 1.0f, "%.3f"))
                mSun->setAmbient(ambient);

            if (ImGui::SliderFloat("Diffuse##Sun", &diffuse, 0.0f, 1.0f, "%.3f"))
                mSun->setDiffuse(diffuse);

            if (ImGui::SliderFloat("Specular##Sun", &specular, 0.0f, 1.0f, "%.3f"))
                mSun->setSpecular(specular);

            float color[4] = {mSun->color().x(), mSun->color().y(), mSun->color().z(), mSun->color().w()};

            if (ImGui::ColorEdit4("Color##Sun", (float *) &color))
                mSun->setColor(QVector4D(color[0], color[1], color[2], color[3]));
        }
    }

    ImGui::Spacing();

    // Fog
    if (!ImGui::CollapsingHeader("Fog"))
    {
        if (ImGui::SliderFloat("Density##Fog", &mFog.density, 0.0f, 4.0f, "%.3f"))
            mRendererManager->setFog(mFog);

        if (ImGui::SliderFloat("Gradient##Fog", &mFog.gradient, 0.0f, 4.0f, "%.3f"))
            mRendererManager->setFog(mFog);

        float color[3] = {mFog.color.x(), mFog.color.y(), mFog.color.z()};

        if (ImGui::ColorEdit3("Color##Fog", (float *) &color))
        {
            mFog.color = QVector3D(color[0], color[1], color[2]);
            mRendererManager->setFog(mFog);
        }

        if (ImGui::Checkbox("Enabled##Fog", &mFog.enabled))
            mRendererManager->setFog(mFog);

        if (ImGui::Button("Reset##Fog"))
            mRendererManager->resetFog();
    }

    ImGui::Spacing();

    // Fog
    if (!ImGui::CollapsingHeader("Terrain"))
    {
        if (ImGui::SliderFloat("Amplitude##Terrain", &mTerrainProperties.amplitude, 0.0f, 50.0f, "%.3f"))
            mTerrain->setProperties(mTerrainProperties);

        if (ImGui::SliderInt("Octaves##Terrain", &mTerrainProperties.octaves, 1, 20))
            mTerrain->setProperties(mTerrainProperties);

        if (ImGui::SliderFloat("Power##Terrain", &mTerrainProperties.power, 0.1f, 10.0f, "%.3f"))
            mTerrain->setProperties(mTerrainProperties);

        if (ImGui::SliderFloat("Tessellation Multiplier##Terrain", &mTerrainProperties.tessellationMultiplier, 0.1f, 10.0f, "%.3f"))
            mTerrain->setProperties(mTerrainProperties);

        if (ImGui::SliderFloat("Grass Coverage##Terrain", &mTerrainProperties.grassCoverage, 0.0f, 1.0f, "%.3f"))
            mTerrain->setProperties(mTerrainProperties);

        if (ImGui::SliderFloat("Ambient##Terrain", &mTerrainMaterial.ambient, 0.0f, 1.0f, "%.3f"))
            mTerrain->setMaterial(mTerrainMaterial);

        if (ImGui::SliderFloat("Diffuse##Terrain", &mTerrainMaterial.diffuse, 0.0f, 1.0f, "%.3f"))
            mTerrain->setMaterial(mTerrainMaterial);

        if (ImGui::SliderFloat("Specular##Terrain", &mTerrainMaterial.specular, 0.0f, 1.0f, "%.3f"))
            mTerrain->setMaterial(mTerrainMaterial);

        if (ImGui::SliderFloat("Shininess##Terrain", &mTerrainMaterial.shininess, 0.1f, 128.0f, "%.3f"))
            mTerrain->setMaterial(mTerrainMaterial);

        if (ImGui::Button("Generate Seed##Terrain"))
        {
            mTerrainProperties.seed = getRandomSeed();
            mTerrain->setProperties(mTerrainProperties);
        }

        if (ImGui::Button("Reset##Terrain"))
            mTerrain->reset();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Nodes
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Nodes");

        if (!ImGui::CollapsingHeader("Select a Node"))
        {
            QString preview = mSelectedNode ? mSelectedNode->name() : "-";
            if (ImGui::BeginCombo("Nodes", preview.toStdString().c_str()))
            {
                for (int i = 0; i < mNodes.size(); ++i)
                    populateComboBox(mNodes[i]);

                ImGui::EndCombo();
            }

            // Metadata
            if (mSelectedNode)
            {
                ImGui::Text("Type: %s", mSelectedNode->nodeTypeString().toStdString().c_str());
                ImGui::Text("Parent: 0x%p", static_cast<void *>(mSelectedNode->parent()));
            }

            if (mSelectedNode)
            {
                {
                    ImGui::Text("Position:");
                    QVector3D position = mSelectedNode->position();
                    float x = position.x();
                    float y = position.y();
                    float z = position.z();

                    if (ImGui::DragFloat("x##NodePosition", &x, 0.01f, -1000.0f, 1000.0f, "%.3f"))
                        mSelectedNode->setPosition(QVector3D(x, y, z));
                    if (ImGui::DragFloat("y##NodePosition", &y, 0.01f, -1000.0f, 1000.0f, "%.3f"))
                        mSelectedNode->setPosition(QVector3D(x, y, z));
                    if (ImGui::DragFloat("z##NodePosition", &z, 0.01f, -1000.0f, 1000.0f, "%.3f"))
                        mSelectedNode->setPosition(QVector3D(x, y, z));
                }

                {
                    ImGui::Text("Rotation:");
                    QQuaternion rotation = mSelectedNode->rotation();
                    float w = rotation.scalar();
                    float x = rotation.x();
                    float y = rotation.y();
                    float z = rotation.z();

                    if (ImGui::DragFloat("w##NodeRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("x##NodeRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("y##NodeRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("z##NodeRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setRotation(QQuaternion(w, x, y, z).normalized());
                }

                {
                    ImGui::Text("World Rotation:");
                    QQuaternion rotation = mSelectedNode->worldRotation();
                    float w = rotation.scalar();
                    float x = rotation.x();
                    float y = rotation.y();
                    float z = rotation.z();

                    if (ImGui::DragFloat("w##NodeWorldRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setWorldRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("x##NodeWorldRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setWorldRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("y##NodeWorldRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setWorldRotation(QQuaternion(w, x, y, z).normalized());
                    if (ImGui::DragFloat("z##NodeWorldRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                        mSelectedNode->setWorldRotation(QQuaternion(w, x, y, z).normalized());
                }

                {
                    ImGui::Text("Scale:");
                    QVector3D scale = mSelectedNode->scale();
                    float x = scale.x();
                    float y = scale.y();
                    float z = scale.z();

                    if (ImGui::DragFloat("x##NodeScale", &x, 0.01f, 0.0001f, 100.0f, "%.3f"))
                        mSelectedNode->setScale(QVector3D(x, y, z));
                    if (ImGui::DragFloat("y##NodeScale", &y, 0.01f, 0.0001f, 100.0f, "%.3f"))
                        mSelectedNode->setScale(QVector3D(x, y, z));
                    if (ImGui::DragFloat("z##NodeScale", &z, 0.01f, 0.0001f, 100.0f, "%.3f"))
                        mSelectedNode->setScale(QVector3D(x, y, z));
                }
            }

            // Shading Parameters
            Model *model = dynamic_cast<Model *>(mSelectedNode);
            if (model)
            {
                ImGui::Text("Shading Parameters:");
                Model::Material material = model->material();

                if (ImGui::SliderFloat("Ambient##Model", &material.ambient, 0.0f, 1.0f, "%.3f"))
                    model->setMaterial(material);

                if (ImGui::SliderFloat("Diffuse##Model", &material.diffuse, 0.0f, 1.0f, "%.3f"))
                    model->setMaterial(material);

                if (ImGui::SliderFloat("Specular##Model", &material.specular, 0.0f, 1.0f, "%.3f"))
                    model->setMaterial(material);

                if (ImGui::SliderFloat("Shininess##Model", &material.shininess, 1.0f, 128.0f, "%.3f"))
                    model->setMaterial(material);

                float color[4] = {material.color.x(), material.color.y(), material.color.z(), material.color.w()};

                if (ImGui::ColorEdit4("Color##Model", (float *) &color))
                {
                    QVector4D newColor = QVector4D(color[0], color[1], color[2], color[3]);
                    material.color = newColor;
                    model->setMaterial(material);
                }
            }

            PointLight *pointLight = dynamic_cast<PointLight *>(mSelectedNode);
            if (pointLight)
            {
                float constant = pointLight->constant();
                float linear = pointLight->linear();
                float quadratic = pointLight->quadratic();
                float ambient = pointLight->ambient();
                float diffuse = pointLight->diffuse();
                float specular = pointLight->specular();
                QVector4D color = pointLight->color();

                ImGui::Text("Light Parameters:");

                if (ImGui::SliderFloat("Constant##PointLight", &constant, 0.0f, 1.0f, "%.3f"))
                    pointLight->setConstant(constant);

                if (ImGui::SliderFloat("Linear##PointLight", &linear, 0.0f, 1.0f, "%.3f"))
                    pointLight->setLinear(linear);

                if (ImGui::SliderFloat("Quadratic##PointLight", &quadratic, 0.0f, 1.0f, "%.3f"))
                    pointLight->setQuadratic(quadratic);

                if (ImGui::SliderFloat("Ambient##PointLight", &ambient, 0.0f, 1.0f, "%.3f"))
                    pointLight->setAmbient(ambient);

                if (ImGui::SliderFloat("Diffuse##PointLight", &diffuse, 0.0f, 1.0f, "%.3f"))
                    pointLight->setDiffuse(diffuse);

                if (ImGui::SliderFloat("Specular##PointLight", &specular, 0.0f, 1.0f, "%.3f"))
                    pointLight->setSpecular(specular);

                float colorf[4] = {color.x(), color.y(), color.z(), color.w()};

                if (ImGui::ColorEdit4("Color##PointLight", (float *) &colorf))
                {
                    QVector4D newColor = QVector4D(colorf[0], colorf[1], colorf[2], colorf[3]);
                    pointLight->setColor(newColor);
                }
            }
        }
    }

    // Nozzle Effect
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("NozzleEffect");

        float radius = mNozzleEffect->radius();
        float maxLife = mNozzleEffect->maxLife();
        float minVelocity = mNozzleEffect->minVelocity();
        float maxVelocity = mNozzleEffect->maxVelocity();

        if (ImGui::SliderFloat("Radius##NozzleEffect", &radius, 0.001f, 4.0f, "%.3f"))
            mNozzleEffect->setRadius(radius);
        if (ImGui::SliderFloat("Max Life##NozzleEffect", &maxLife, 0.001f, 2.0f, "%.3f"))
            mNozzleEffect->setMaxLife(maxLife);
        if (ImGui::SliderFloat("Min Velocity##NozzleEffect", &minVelocity, 0.0f, 20.0f, "%.3f"))
            mNozzleEffect->setMinVelocity(minVelocity);
        if (ImGui::SliderFloat("Max Velocity##NozzleEffect", &maxVelocity, 20.0f, 100.0f, "%.3f"))
            mNozzleEffect->setMaxVelocity(maxVelocity);

        {
            ImGui::Text("Position:");
            QVector3D position = mNozzleEffect->particlesPosition();
            float x = position.x();
            float y = position.y();
            float z = position.z();

            if (ImGui::DragFloat("x##NozzleEffectPosition", &x, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setParticlesPosition(QVector3D(x, y, z));
            if (ImGui::DragFloat("y##NozzleEffectPosition", &y, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setParticlesPosition(QVector3D(x, y, z));
            if (ImGui::DragFloat("z##NozzleEffectPosition", &z, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setParticlesPosition(QVector3D(x, y, z));
        }

        {
            ImGui::Text("Rotation:");
            QQuaternion rotation = mNozzleEffect->particlesRotation();
            float w = rotation.scalar();
            float x = rotation.x();
            float y = rotation.y();
            float z = rotation.z();

            if (ImGui::DragFloat("w##NozzleEffectRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setParticlesRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("x##NozzleEffectRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setParticlesRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("y##NozzleEffectRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setParticlesRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("z##NozzleEffectRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setParticlesRotation(QQuaternion(w, x, y, z).normalized());
        }

        {
            ImGui::Text("Blur Position:");
            QVector3D position = mNozzleEffect->position();
            float x = position.x();
            float y = position.y();
            float z = position.z();

            if (ImGui::DragFloat("x##NozzleEffectBlurPosition", &x, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setPosition(QVector3D(x, y, z));
            if (ImGui::DragFloat("y##NozzleEffectBlurPosition", &y, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setPosition(QVector3D(x, y, z));
            if (ImGui::DragFloat("z##NozzleEffectBlurPosition", &z, 0.01f, -50.0f, 50.0f, "%.3f"))
                mNozzleEffect->setPosition(QVector3D(x, y, z));
        }

        {
            ImGui::Text("Blur Rotation:");
            QQuaternion rotation = mNozzleEffect->rotation();
            float w = rotation.scalar();
            float x = rotation.x();
            float y = rotation.y();
            float z = rotation.z();

            if (ImGui::DragFloat("w##NozzleEffectBlurRotation", &w, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("x##NozzleEffectBlurRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("y##NozzleEffectBlurRotation", &y, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setRotation(QQuaternion(w, x, y, z).normalized());
            if (ImGui::DragFloat("z##NozzleEffectBlurRotation", &x, 0.001f, -1.0f, 1.0f, "%.3f"))
                mNozzleEffect->setRotation(QQuaternion(w, x, y, z).normalized());
        }

        {
            ImGui::Text("Blur Scale:");
            QVector3D scale = mNozzleEffect->scale();
            float x = scale.x();
            float y = scale.y();
            float z = scale.z();

            if (ImGui::DragFloat("x##NozzleEffectBlurScale", &x, 0.00001f, 0.00001f, 1.0f, "%.6f"))
                mNozzleEffect->setScale(QVector3D(x, y, z));
            if (ImGui::DragFloat("y##NozzleEffectBlurScale", &y, 0.00001f, 0.00001f, 1.0f, "%.6f"))
                mNozzleEffect->setScale(QVector3D(x, y, z));
            if (ImGui::DragFloat("z##NozzleEffectBlurScale", &z, 0.00001f, 0.00001f, 1.0f, "%.6f"))
                mNozzleEffect->setScale(QVector3D(x, y, z));
        }
    }

    // Simulator
    {
        ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
        ImGui::Begin("Simulator");

        ImGui::BeginDisabled();

        ImGui::SliderFloat("Elevator", &mElevator, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Aileron", &mAileron, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Rudder", &mRudder, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Throttle", &mThrottle, 0.0f, 1.0f, "%.3f");

        ImGui::EndDisabled();

        if (ImGui::Button("Init Running"))
            emit command(Aircraft::Command::InitRunning);

        if (ImGui::Button("Hold"))
            emit command(Aircraft::Command::Hold);

        if (ImGui::Button("Resume"))
            emit command(Aircraft::Command::Resume);

        ImGui::Text("Airspeed:    %.2f knots", mPfd.airspeed);
        ImGui::Text("Latitude:    %.6f °", mPfd.latitude);
        ImGui::Text("Longitude:   %.6f °", mPfd.longitude);
        ImGui::Text("Altitude:    %.2f meters", mPfd.altitude);
        ImGui::Text("Roll:        %.1f °", mPfd.roll);
        ImGui::Text("Pitch:       %.1f °", mPfd.pitch);
        ImGui::Text("Heading:     %.1f °", mPfd.heading);
    }

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    emit keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);
}

void Window::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDoubleClicked(event);
}

void Window::populateComboBox(Node *node)
{
    switch (node->nodeType())
    {
    case Node::NodeType::FreeCamera:
    case Node::NodeType::DirectionalLight:
        return;
    default:
        break;
    }

    if (ImGui::Selectable(node->name().toStdString().c_str()))
        mSelectedNode = node;
}

QVector3D Window::getRandomSeed()
{
    float x = mRandomGenerator.generateDouble();
    float y = mRandomGenerator.generateDouble();
    float z = mRandomGenerator.generateDouble();

    return QVector3D(x, y, z);
}

NozzleEffect *Window::nozzleEffect() const
{
    return mNozzleEffect;
}

void Window::setNozzleEffect(NozzleEffect *newNozzleEffect)
{
    mNozzleEffect = newNozzleEffect;
}

void Window::setAircraftController(AircraftController *newAircraftController)
{
    mAircraftController = newAircraftController;
}

void Window::onPfdChanged(Aircraft::PrimaryFlightData pfd)
{
    mPfd = pfd;
}

bool Window::imguiWantCapture() const
{
    return mImguiWantCapture;
}
