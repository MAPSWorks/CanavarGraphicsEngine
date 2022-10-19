#include "Manager.h"

Canavar::Engine::Manager::Manager(QObject *parent)
    : QObject(parent)
{}

void Canavar::Engine::Manager::mouseDoubleClicked(QMouseEvent *event) {}

void Canavar::Engine::Manager::mousePressed(QMouseEvent *event) {}

void Canavar::Engine::Manager::mouseReleased(QMouseEvent *event) {}

void Canavar::Engine::Manager::mouseMoved(QMouseEvent *event) {}

void Canavar::Engine::Manager::wheelMoved(QWheelEvent *event) {}

void Canavar::Engine::Manager::keyPressed(QKeyEvent *event) {}

void Canavar::Engine::Manager::keyReleased(QKeyEvent *event) {}

void Canavar::Engine::Manager::resize(int width, int height) {}

void Canavar::Engine::Manager::update(float ifps) {}

void Canavar::Engine::Manager::reset() {}
