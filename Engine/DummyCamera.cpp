#include "DummyCamera.h"

Canavar::Engine::DummyCamera::DummyCamera(const QString &uuid)
    : PerspectiveCamera(uuid)
{
    mType = Canavar::Engine::Node::NodeType::DummyCamera;
}

void Canavar::Engine::DummyCamera::mouseDoubleClicked(QMouseEvent *) {}

void Canavar::Engine::DummyCamera::mousePressed(QMouseEvent *) {}

void Canavar::Engine::DummyCamera::mouseReleased(QMouseEvent *) {}

void Canavar::Engine::DummyCamera::mouseMoved(QMouseEvent *) {}

void Canavar::Engine::DummyCamera::wheelMoved(QWheelEvent *) {}

void Canavar::Engine::DummyCamera::keyPressed(QKeyEvent *) {}

void Canavar::Engine::DummyCamera::keyReleased(QKeyEvent *) {}

void Canavar::Engine::DummyCamera::update(float) {}

void Canavar::Engine::DummyCamera::reset() {}
