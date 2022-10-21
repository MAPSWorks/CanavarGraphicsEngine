#include "Manager.h"

Canavar::Engine::Manager::Manager(QObject *parent)
    : QObject(parent)
{}

void Canavar::Engine::Manager::mouseDoubleClicked(QMouseEvent *) {}

void Canavar::Engine::Manager::mousePressed(QMouseEvent *) {}

void Canavar::Engine::Manager::mouseReleased(QMouseEvent *) {}

void Canavar::Engine::Manager::mouseMoved(QMouseEvent *) {}

void Canavar::Engine::Manager::wheelMoved(QWheelEvent *) {}

void Canavar::Engine::Manager::keyPressed(QKeyEvent *) {}

void Canavar::Engine::Manager::keyReleased(QKeyEvent *) {}

void Canavar::Engine::Manager::resize(int, int) {}

void Canavar::Engine::Manager::update(float) {}

void Canavar::Engine::Manager::reset() {}
