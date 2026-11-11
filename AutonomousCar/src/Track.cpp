#include "Track.hpp"
#include "raymath.h"
#include <cmath>
#include <limits>

Track::Track() : startPosition{0, 0}, startAngleDeg(0.0f) {}

// Math helper for line-to-line 2D intersection
static bool CheckLineIntersection(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Vector2 &outPoint)
{
    float denominator = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
    if (std::abs(denominator) < 1e-5f)
        return false;

    float ua = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / denominator;
    float ub = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / denominator;

    if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
    {
        outPoint.x = p1.x + ua * (p2.x - p1.x);
        outPoint.y = p1.y + ua * (p2.y - p1.y);
        return true;
    }
    return false;
}

void Track::loadDefaultTrack(float screenWidth, float screenHeight)
{
    walls.clear();

    Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};
    float outerRadiusX = 350.0f;
    float outerRadiusY = 220.0f;
    float trackWidth = 100.0f;

    float innerRadiusX = outerRadiusX - trackWidth;
    float innerRadiusY = outerRadiusY - trackWidth;

    const int segments = 32;

    for (int i = 0; i < segments; ++i)
    {
        float angle1 = (static_cast<float>(i) / segments) * 2.0f * PI;
        float angle2 = (static_cast<float>(i + 1) / segments) * 2.0f * PI;

        // Outer loop
        Vector2 outerStart = {center.x + std::cos(angle1) * outerRadiusX, center.y + std::sin(angle1) * outerRadiusY};
        Vector2 outerEnd = {center.x + std::cos(angle2) * outerRadiusX, center.y + std::sin(angle2) * outerRadiusY};
        walls.push_back({outerStart, outerEnd});

        // Inner loop
        Vector2 innerStart = {center.x + std::cos(angle1) * innerRadiusX, center.y + std::sin(angle1) * innerRadiusY};
        Vector2 innerEnd = {center.x + std::cos(angle2) * innerRadiusX, center.y + std::sin(angle2) * innerRadiusY};
        walls.push_back({innerStart, innerEnd});
    }

    startPosition = {center.x, center.y - (outerRadiusY - trackWidth / 2.0f)};
    startAngleDeg = 0.0f; // Facing right
}

void Track::draw() const
{
    for (const auto &wall : walls)
    {
        DrawLineEx(wall.start, wall.end, 3.0f, DARKGRAY);
    }

    // Start/Finish Line
    Vector2 center = {startPosition.x, startPosition.y};
    DrawLineEx({center.x, center.y - 50.0f}, {center.x, center.y + 50.0f}, 4.0f, RED);
}

bool Track::getRayIntersection(Vector2 rayStart, Vector2 rayEnd, Vector2 &outIntersection) const
{
    bool hit = false;
    float closestDist = std::numeric_limits<float>::max();

    for (const auto &wall : walls)
    {
        Vector2 intersectPoint;
        if (CheckLineIntersection(rayStart, rayEnd, wall.start, wall.end, intersectPoint))
        {
            float dist = Vector2Distance(rayStart, intersectPoint);
            if (dist < closestDist)
            {
                closestDist = dist;
                outIntersection = intersectPoint;
                hit = true;
            }
        }
    }
    return hit;
}