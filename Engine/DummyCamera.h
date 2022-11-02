#ifndef DUMMYCAMERA_H
#define DUMMYCAMERA_H

#include "PerspectiveCamera.h"

namespace Canavar {
namespace Engine {

class DummyCamera : public PerspectiveCamera
{
protected:
    friend class Canavar::Engine::NodeManager;
    DummyCamera();

public:
    void mouseDoubleClicked(QMouseEvent *) override;
    void mousePressed(QMouseEvent *) override;
    void mouseReleased(QMouseEvent *) override;
    void mouseMoved(QMouseEvent *) override;
    void wheelMoved(QWheelEvent *) override;
    void keyPressed(QKeyEvent *) override;
    void keyReleased(QKeyEvent *) override;
    void update(float) override;
    void reset() override;
};
} // namespace Engine
} // namespace Canavar
#endif // DUMMYCAMERA_H
