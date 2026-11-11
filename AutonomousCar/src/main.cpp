#include "raylib.h"
#include "Track.hpp"
#include "Car.hpp"
#include <algorithm>

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Autonomous Car Simulation - C++");
    SetTargetFPS(60);

    Track track;
    track.loadDefaultTrack(screenWidth, screenHeight);

    Car car(track.startPosition, track.startAngleDeg);
    car.speed = 2.0f; // Default cruising speed
    float logTimer = 0.0f; // Timer to throttle terminal output

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Manual controls for testing
        if (!car.isCrashed)
        {
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
                car.angleDeg -= 3.0f;
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
                car.angleDeg += 3.0f;
            // std::clamp do C++20
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
                car.speed = std::clamp(car.speed + 0.1f, 0.0f, 5.0f);
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
                car.speed = std::clamp(car.speed - 0.1f, 0.0f, 5.0f);
        }

        // Press 'R' to reset car after crash
        if (IsKeyPressed(KEY_R))
        {
            car.reset(track.startPosition, track.startAngleDeg);
            car.speed = 2.0f;
        }

        // Physics & Sensor update
        car.update(track, deltaTime);

        // --- TERMINAL LOGGING (Throttled to 10 times per second to prevent screen flicker) ---
        logTimer += deltaTime;
        if (logTimer >= 0.1f)
        {
            car.logSensorsToTerminal();
            logTimer = 0.0f;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        track.draw();
        car.draw();

        // Telemetry HUD
        DrawFPS(10, 10);
        DrawText(TextFormat("Speed: %.1f", car.speed), 10, 35, 16, DARKGRAY);
        DrawText(TextFormat("Angle: %.1f deg", car.angleDeg), 10, 55, 16, DARKGRAY);
        DrawText("Controls: A/D (Steer), W/S (Speed), R (Reset)", 10, 570, 16, DARKBLUE);

        // --- ON-SCREEN SENSOR HUD ---
        // Draws at position X=600, Y=10 (Top-right corner)
        car.drawTelemetry(580, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}