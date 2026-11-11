#pragma once
#include "raylib.h"
#include <vector>

struct LineSegment {
    Vector2 start;
    Vector2 end;
};

class Track {
public:
    std::vector<LineSegment> walls;
    Vector2 startPosition;
    float startAngleDeg;

    Track();

    void loadDefaultTrack(float screenWidth, float screenHeight);
    void draw() const;

    // Helper method to check if a ray/line intersects with any track wall
    bool getRayIntersection(Vector2 rayStart, Vector2 rayEnd, Vector2& outIntersection) const;
};