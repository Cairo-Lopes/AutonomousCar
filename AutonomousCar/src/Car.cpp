#include "Car.hpp"
#include "raymath.h"
#include <cmath>
#include <iomanip>
#include <iostream>

Car::Car(Vector2 startPos, float startAngle)
    : position(startPos), angleDeg(startAngle), speed(0.0f), size(20.0f), isCrashed(false), fitness(0.0f)
{

    // Set 5 fan-shaped sensor offsets
    sensors[0] = {-90.0f, 200.0f, 1.0f, {0, 0}};
    sensors[1] = {-45.0f, 200.0f, 1.0f, {0, 0}};
    sensors[2] = {0.0f, 200.0f, 1.0f, {0, 0}};
    sensors[3] = {45.0f, 200.0f, 1.0f, {0, 0}};
    sensors[4] = {90.0f, 200.0f, 1.0f, {0, 0}};
}

std::array<Vector2, 4> Car::getRotatedCorners() const
{
    float rad = angleDeg * DEG2RAD;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);
    float halfSize = size / 2.0f;

    std::array<Vector2, 4> localCorners = {{{-halfSize, -halfSize},
                                            {halfSize, -halfSize},
                                            {halfSize, halfSize},
                                            {-halfSize, halfSize}}};

    std::array<Vector2, 4> worldCorners;
    for (int i = 0; i < 4; ++i)
    {
        worldCorners[i].x = position.x + (localCorners[i].x * cosA - localCorners[i].y * sinA);
        worldCorners[i].y = position.y + (localCorners[i].x * sinA + localCorners[i].y * cosA);
    }
    return worldCorners;
}

void Car::updateSensors(const Track &track)
{
    for (auto &sensor : sensors)
    {
        float totalAngleRad = (angleDeg + sensor.angleOffset) * DEG2RAD;
        Vector2 rayMaxEnd = {
            position.x + std::cos(totalAngleRad) * sensor.maxRange,
            position.y + std::sin(totalAngleRad) * sensor.maxRange};

        Vector2 hitPoint;
        if (track.getRayIntersection(position, rayMaxEnd, hitPoint))
        {
            float dist = Vector2Distance(position, hitPoint);
            sensor.distance = dist / sensor.maxRange;
            sensor.hitPoint = hitPoint;
        }
        else
        {
            sensor.distance = 1.0f;
            sensor.hitPoint = rayMaxEnd;
        }
    }
}

void Car::update(const Track &track, float deltaTime)
{
    if (isCrashed)
        return;

    // 1. Position update
    float rad = angleDeg * DEG2RAD;
    position.x += std::cos(rad) * speed * deltaTime * 60.0f;
    position.y += std::sin(rad) * speed * deltaTime * 60.0f;

    // 2. Box Collider Check against track walls
    auto corners = getRotatedCorners();
    std::array<LineSegment, 4> carEdges = {{{corners[0], corners[1]},
                                            {corners[1], corners[2]},
                                            {corners[2], corners[3]},
                                            {corners[3], corners[0]}}};

    for (const auto &edge : carEdges)
    {
        Vector2 hit;
        if (track.getRayIntersection(edge.start, edge.end, hit))
        {
            isCrashed = true;
            speed = 0.0f;
            return;
        }
    }

    // 3. Update sensors
    updateSensors(track);
    fitness += speed * deltaTime;
}

void Car::draw() const
{
    if (isCrashed)
    {
        // Draw crashed indicator
        DrawText("CRASHED!", position.x - 30, position.y - 30, 16, RED);
        return;
    }

    // 1. Draw outer border (Rotated square in MAROON)
    float borderWidth = 2.0f;
    Rectangle outlineBox = {position.x, position.y, size + borderWidth * 2, size + borderWidth * 2};
    Vector2 outlineOrigin = {(size + borderWidth * 2) / 2.0f, (size + borderWidth * 2) / 2.0f};
    DrawRectanglePro(outlineBox, outlineOrigin, angleDeg, MAROON);

    // 2. Draw inner car body (Rotated square in RED)
    Rectangle carBox = {position.x, position.y, size, size};
    Vector2 origin = {size / 2.0f, size / 2.0f};
    DrawRectanglePro(carBox, origin, angleDeg, RED);

    // 3. Draw Sensor Rays
    for (const auto &sensor : sensors)
    {
        Color rayColor = GREEN;
        if (sensor.distance < 0.1f)
            rayColor = RED;
        else if (sensor.distance < 0.5f)
            rayColor = GOLD;

        DrawLineEx(position, sensor.hitPoint, 2.0f, Fade(rayColor, 0.6f));
        DrawCircleV(sensor.hitPoint, 3.0f, rayColor);
    }
}

void Car::reset(Vector2 startPos, float startAngle)
{
    position = startPos;
    angleDeg = startAngle;
    speed = 0.0f;
    isCrashed = false;
    fitness = 0.0f;
}

// 1. Draw sensor HUD on Screen
void Car::drawTelemetry(int posX, int posY) const
{
    int lineOffset = 0;

    // Header
    DrawText("--- SENSOR READING ---", posX, posY, 14, DARKBLUE);
    lineOffset += 20;

    for (size_t i = 0; i < sensors.size(); ++i)
    {
        float angle = sensors[i].angleOffset;
        float normDisct = sensors[i].distance;
        float realDisct = normDisct * sensors[i].maxRange;

        // Color indicator based on Distance
        Color textColor = DARKGRAY;
        if (normDisct < 0.1f)
            textColor = RED;
        else if (normDisct < 0.5f)
            textColor = GOLD;

        // Draw formatted text: eg., "S1 (-90.0 deg): 0.05 (10.0 px)"
        const char *logText = TextFormat("S%zu (%+.1f deg): %.2f (%.0f px)",
                                         i + 1, angle, normDisct, realDisct);
        DrawText(logText, posX, posY + lineOffset, 14, textColor);
        lineOffset += 18;
    }

    if (isCrashed)
    {
        DrawText("STATUS: CRASHED!", posX, posY + lineOffset, 16, RED);
    }
    else
    {
        DrawText("STATUS: RUNNING", posX, posY + lineOffset, 16, LIME);
    }
}

// 2. Print Sensor Readings to Terminal
void Car::logSensorsToTerminal() const
{
    std::cout << "\r[Telemetry] ";
    for (size_t i = 0; i < sensors.size(); ++i)
    {
        std::cout << "S" << (i + 1) << ": "
                  << std::fixed << std::setprecision(2) << sensors[i].distance
                  << " | ";
    }
    std::cout << "Angle: " << std::fixed << std::setprecision(1) << angleDeg
              << " | Speed: " << speed
              << std::flush; // '\r' and std::flush overwrite the line live in terminal
}