#ifndef GUI_H
#define GUI_H

#include "FirecrackerEffect.h"
#include "Mesh.h"
#include "Model.h"
#include "Node.h"
#include "NozzleEffect.h"
#include "PerspectiveCamera.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"

#include <imgui.h>
#include <QtImGui.h>

namespace Canavar {
namespace Engine {

class Gui : public QObject
{
public:
    explicit Gui(QObject *parent = nullptr);

    void draw();

    void draw(Canavar::Engine::Node *node);
    void draw(Canavar::Engine::Model *node);
    void draw(Canavar::Engine::Sky *node);
    void draw(Canavar::Engine::Sun *node);
    void draw(Canavar::Engine::PerspectiveCamera *node);
    void draw(Canavar::Engine::Terrain *node);
    void draw(Canavar::Engine::Light *node);
    void draw(Canavar::Engine::PointLight *node);
    void draw(Canavar::Engine::Haze *node);
    void draw(Canavar::Engine::NozzleEffect *node);
    void draw(Canavar::Engine::FirecrackerEffect *node);

    Canavar::Engine::Node *getSelectedNode() const;
    void setSelectedNode(Canavar::Engine::Node *newSelectedNode);

    Canavar::Engine::Mesh *getSelectedMesh() const;
    void setSelectedMesh(Canavar::Engine::Mesh *newSelectedMesh);

    void mousePressed(QMouseEvent *event);

private:
    Canavar::Engine::Node *mSelectedNode;
    Canavar::Engine::Mesh *mSelectedMesh;
    bool mDrawAllBBs;
    bool mNodeSelectionEnabled;
};

} // namespace Engine
} // namespace Canavar

#endif // GUI_H
