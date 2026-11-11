#pragma once
#include "raylib.h"
#include "Track.hpp"
#include <array>

struct CarSensor
{
    float angleOffset;
    float maxRange;
    float distance; // Normalized 0.0 (impact) to 1.0 (clear)
    Vector2 hitPoint;
};

class Car
{
public:
    Vector2 position;
    float angleDeg;
    float speed;
    float size; // Square dimension (e.g. 20px)

    bool isCrashed;
    float fitness;

    std::array<CarSensor, 5> sensors;

    Car(Vector2 startPos = {0, 0}, float startAngle = 0.0f);

    void update(const Track &track, float deltaTime);
    void draw() const;
    void reset(Vector2 startPos, float startAngle);

    // Logging & Telemetry

    void drawTelemetry(int posX, int posY) const;
    void logSensorsToTerminal() const;

private:
    void updateSensors(const Track &track);
    std::array<Vector2, 4> getRotatedCorners() const;
};