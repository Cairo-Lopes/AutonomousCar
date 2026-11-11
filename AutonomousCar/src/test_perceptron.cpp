#include <iostream>       // Include the iostream library for input and output operations
#include <vector>         // Include the vector library for using the std::vector container
#include <iomanip>        // Include the iomanip library for manipulating input and output formatting
#include "Perceptron.hpp" // Include the Perceptron header file for using the Perceptron class

// UTILITY FUNCTION TO PRINT TEST RESULTS CLEANLY
void printModelOutput(const std::string &testName, const std::vector<float> &sensors, float output)
{
    std::cout << "\n--- Test: " << testName << " ---\n";
    std::cout << "Sensors [L90, L45, F0, R45, R90]: [ ";
    for (float s : sensors)
    {
        std::cout << std::fixed << std::setprecision(2) << s << " ";
    }
    std::cout << "]\n";
    std::cout << "Model Action Output (Steering): " << std::fixed << std::setprecision(4) << output;

    if (output < -0.2f)
        std::cout << " -> Steering LEFT\n";
    else if (output > 0.2f)
        std::cout << " -> Steering RIGHT\n";
    else
        std::cout << " -> Going STRAIGHT\n";
}

int main()
{
    std::cout << "=======================================\n";
    std::cout << "  AI MODEL PLAYGROUND & TEST BENCH     \n";
    std::cout << "=======================================\n";

    // 1. Instantiate a 5-input Perceptron model
    Perceptron model(5);

    // Set custom test weights manually to simulate a trained state
    //  Negative weigths for left sensors, postitive for rigth sensors
    model.weights = {-0.8f, -0.5f, 0.0f, 0.5f, 0.8f}; // Example weights for left and right sensors
    model.bias = 0.0f;                                // Example bias
    // Choose activation function
    model.setActivationFunction(Perceptron::ActivationFunction::Tanh); // Use Tanh for smoother output between -1 and 1
    // 2. Scenarion A: Wall on the left side (Sensor 0 and 1 hit obstacle -> lower  values)
    std::vector<float> wallOnLeft = {0.1f, 0.3f, 1.0f, 1.0f, 1.0f}; // Simulated sensor readings for wall on the left
    float outputA = model.predict(wallOnLeft);                      // Get the model's prediction for this scenario
    printModelOutput("Wall on the Left", wallOnLeft, outputA);

    // 3. Scenario B: Wall on the right side (Sensor 3 and 4 hit obstacle -> lower values)
    std::vector<float> wallOnRight = {1.0f, 1.0f, 1.0f, 0.3f, 0.1f}; // Simulated sensor readings for wall on the right
    float outputB = model.predict(wallOnRight);                      // Get the model's prediction for this scenario
    printModelOutput("Wall on the Right", wallOnRight, outputB);

    // 4. Scenario C: Open road straight ahead
    std::vector<float> openRoad = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float outputC = model.predict(openRoad);
    printModelOutput("Clear Road (Should Go Straight)", openRoad, outputC);

    std::cout << "\n=======================================\n";
    return 0; // Exit the program successfully
}