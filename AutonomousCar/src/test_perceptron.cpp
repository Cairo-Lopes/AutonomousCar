#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Layer.hpp"
#include "Perceptron.hpp"

enum class Steering
{
    Left,
    Straight,
    Right
};

enum class SpeedAction
{
    Reduce,
    Keep,
    Increase
};

struct RuleAction
{
    Steering steering;
    SpeedAction speed;
};

const char *toString(Steering steering)
{
    switch (steering)
    {
    case Steering::Left:
        return "LEFT";
    case Steering::Right:
        return "RIGHT";
    case Steering::Straight:
    default:
        return "STRAIGHT";
    }
}

const char *toString(SpeedAction speed)
{
    switch (speed)
    {
    case SpeedAction::Reduce:
        return "REDUCE";
    case SpeedAction::Increase:
        return "INCREASE";
    case SpeedAction::Keep:
    default:
        return "KEEP";
    }
}

Steering steeringFromOutput(float output)
{
    if (output < -0.2f)
        return Steering::Left;
    if (output > 0.2f)
        return Steering::Right;
    return Steering::Straight;
}

SpeedAction speedFromOutput(float output)
{
    if (output < 0.37f)
        return SpeedAction::Reduce;
    if (output > 0.67f)
        return SpeedAction::Increase;
    return SpeedAction::Keep;
}

RuleAction applyDrivingRules(const std::vector<float> &sensors)
{
    const float leftClearance = std::min(sensors[0], sensors[1]);
    const float rightClearance = std::min(sensors[3], sensors[4]);
    constexpr float sideDifferenceThreshold = 0.1f;

    Steering steering = Steering::Straight;
    if (leftClearance + sideDifferenceThreshold < rightClearance)
        steering = Steering::Right;
    else if (rightClearance + sideDifferenceThreshold < leftClearance)
        steering = Steering::Left;

    SpeedAction speed = SpeedAction::Keep;
    if (sensors[2] < 0.4f)
        speed = SpeedAction::Reduce;
    else if (sensors[2] > 0.8f)
        speed = SpeedAction::Increase;

    return {steering, speed};
}

void printVector(const char *label, const std::vector<float> &values)
{
    std::cout << label << "[ ";
    for (float value : values)
        std::cout << std::fixed << std::setprecision(3) << value << " ";
    std::cout << "]";
}

std::vector<float> runLayerChain(
    const std::vector<float> &inputs,
    const Layer &firstLayer,
    const Layer &secondLayer,
    const Layer &outputLayer)
{
    const std::vector<float> firstActivations = firstLayer.forward(inputs);
    const std::vector<float> secondActivations = secondLayer.forward(firstActivations);
    const std::vector<float> outputs = outputLayer.forward(secondActivations);

    printVector("Input (5): ", inputs);
    std::cout << "\n";
    printVector("Layer 1 (5 -> 5): ", firstActivations);
    std::cout << "\n";
    printVector("Layer 2 (5 -> 4): ", secondActivations);
    std::cout << "\n";
    printVector("Layer 3 (4 -> 2): ", outputs);
    std::cout << "\n";
    return outputs;
}

void printResponsePlot(
    const std::vector<float> &leftSensorReadings,
    const std::vector<float> &perceptronSteering,
    const std::vector<float> &layerSteering)
{
    constexpr int plotHeight = 11;
    constexpr int zeroRow = (plotHeight - 1) / 2;
    const size_t plotWidth = leftSensorReadings.size() * 4;
    std::vector<std::vector<char>> plot(plotHeight, std::vector<char>(plotWidth, ' '));

    const auto rowForOutput = [](float output)
    {
        return static_cast<int>(std::lround((1.0f - std::clamp(output, -1.0f, 1.0f)) * 5.0f));
    };

    const auto addPoint = [&plot](int row, size_t column, char marker)
    {
        char &cell = plot[row][column];
        if (cell == ' ')
            cell = marker;
        else if ((cell == 'P' && marker == 'L') || (cell == 'L' && marker == 'P'))
            cell = '*';
        else
            cell = '+';
    };

    for (size_t column = 0; column < leftSensorReadings.size(); ++column)
    {
        const size_t plotColumn = column * 4 + 2;
        addPoint(rowForOutput(perceptronSteering[column]), plotColumn, 'P');
        addPoint(rowForOutput(layerSteering[column]), plotColumn, 'L');
    }

    std::cout << "\n--- Steering response plot ---\n";
    std::cout << "Y: steering output (-1 to 1); X: left sensor clearance (lower means closer to wall)\n";
    for (int row = 0; row < plotHeight; ++row)
    {
        const float yValue = 1.0f - row * 0.2f;
        std::cout << std::fixed << std::setprecision(1) << std::showpos << yValue
                  << std::noshowpos << (row == zeroRow ? " +" : " |");
        for (char cell : plot[row])
            std::cout << (row == zeroRow && cell == ' ' ? '-' : cell);
        std::cout << '\n';
    }

    std::cout << "     ";
    for (float reading : leftSensorReadings)
        std::cout << std::fixed << std::setprecision(1) << std::setw(4) << reading;
    std::cout << "\n     Left sensor clearance\n";
    std::cout << "Legend: P = Perceptron, L = Layer, * = both overlap\n";
}

void runVehicleScenario(
    const std::string &name,
    const std::vector<float> &sensors,
    const Perceptron &perceptron,
    const Layer &layer)
{
    const RuleAction expected = applyDrivingRules(sensors);
    const float perceptronOutput = perceptron.predict(sensors);
    const std::vector<float> layerOutputs = layer.forward(sensors);

    std::cout << "\n" << name << "\n";
    printVector("Sensors [L90, L45, F0, R45, R90]: ", sensors);
    std::cout << "\nRule-based expected action: steer " << toString(expected.steering)
              << ", speed " << toString(expected.speed) << "\n";
    std::cout << "Perceptron: steering output " << std::fixed << std::setprecision(3)
              << perceptronOutput << " -> " << toString(steeringFromOutput(perceptronOutput))
              << " (steering only)\n";
    std::cout << "Layer(5, 2): steering " << layerOutputs[0] << " -> "
              << toString(steeringFromOutput(layerOutputs[0]))
              << ", speed " << layerOutputs[1] << " -> "
              << toString(speedFromOutput(layerOutputs[1])) << "\n";
}

int main()
{
    std::cout << "=======================================\n";
    std::cout << "       PERCEPTRON AND LAYER TESTS      \n";
    std::cout << "=======================================\n";

    std::cout << "\n=== GROUP 1: MODEL EXAMPLES ===\n";
    std::cout << "Fixed illustrative weights show how forward/predict works; they are not trained.\n";

    Perceptron examplePerceptron(5);
    examplePerceptron.weights = {-0.8f, -0.5f, 0.0f, 0.5f, 0.8f};
    examplePerceptron.bias = 0.0f;
    examplePerceptron.setActivationFunction(Perceptron::ActivationFunction::Tanh);

    Layer firstLayer(5, 5);
    Layer secondLayer(5, 4);
    Layer outputLayer(4, 2);
    firstLayer.weights = {
        {-0.8f, -0.5f, 0.0f, 0.5f, 0.8f},
        {-0.6f, -0.3f, 0.0f, 0.3f, 0.6f},
        {0.1f, 0.2f, 0.8f, 0.2f, 0.1f},
        {0.2f, 0.1f, 0.2f, 0.6f, 0.3f},
        {0.3f, 0.2f, 0.1f, 0.4f, 0.7f}};
    firstLayer.biases = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    secondLayer.weights = {
        {-0.5f, 0.0f, 0.2f, 0.5f},
        {-0.4f, 0.1f, 0.1f, 0.4f},
        {0.2f, 0.8f, 0.8f, 0.2f},
        {0.1f, 0.2f, -0.1f, 0.3f},
        {0.2f, 0.1f, 0.3f, -0.1f}};
    secondLayer.biases = {0.0f, 0.0f, 0.0f, 0.0f};
    outputLayer.weights = {
        {-0.8f, 0.0f},
        {-0.4f, 0.0f},
        {0.4f, 1.0f},
        {0.8f, 1.0f}};
    outputLayer.biases = {0.0f, 0.0f};
    firstLayer.setActivationFunction(Layer::ActivationFunction::Tanh);
    secondLayer.setActivationFunction(Layer::ActivationFunction::Tanh);
    outputLayer.setActivationFunction(Layer::ActivationFunction::Tanh);

    const std::vector<float> exampleSensors = {0.1f, 0.3f, 1.0f, 1.0f, 1.0f};
    printVector("\nExample inputs: ", exampleSensors);
    std::cout << "\nPerceptron prediction: " << std::fixed << std::setprecision(3)
              << examplePerceptron.predict(exampleSensors) << "\n";
    std::cout << "\nLayer chain: 5 inputs -> 5 -> 4 -> 2 neurons\n";
    const std::vector<float> exampleOutputs =
        runLayerChain(exampleSensors, firstLayer, secondLayer, outputLayer);
    std::cout << "Final outputs: outputs[0] = steering, outputs[1] = speed signal (example only): "
              << exampleOutputs[0] << ", " << exampleOutputs[1] << "\n";

    std::cout << "\n=== GROUP 2: FIVE-SENSOR VEHICLE RULE SIMULATION ===\n";
    std::cout << "Sensor values are normalized clearances: 1 = clear, 0 = obstacle is very close.\n";
    std::cout << "Rules: turn away from the closer side; front < 0.4 reduce speed, "
                 "0.4-0.8 keep speed, front > 0.8 increase speed.\n";

    Perceptron steeringModel(5);
    steeringModel.weights = {-0.8f, -0.5f, 0.0f, 0.5f, 0.8f};
    steeringModel.bias = 0.0f;
    steeringModel.setActivationFunction(Perceptron::ActivationFunction::Tanh);

    Layer vehicleLayer(5, 2);
    vehicleLayer.weights = {
        {-0.8f, 0.0f},
        {-0.5f, 0.0f},
        {0.0f, 1.0f},
        {0.5f, 0.0f},
        {0.8f, 0.0f}};
    vehicleLayer.biases = {0.0f, 0.0f};
    vehicleLayer.setActivationFunction(Layer::ActivationFunction::Tanh);

    runVehicleScenario("Wall close on the left", {0.1f, 0.3f, 1.0f, 1.0f, 1.0f}, steeringModel, vehicleLayer);
    runVehicleScenario("Wall close on the right", {1.0f, 1.0f, 1.0f, 0.3f, 0.1f}, steeringModel, vehicleLayer);
    runVehicleScenario("Clear road", {1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, steeringModel, vehicleLayer);
    runVehicleScenario("Obstacle directly ahead", {1.0f, 0.8f, 0.2f, 0.8f, 1.0f}, steeringModel, vehicleLayer);
    runVehicleScenario("Left wall and moderate front clearance", {0.1f, 0.3f, 0.6f, 1.0f, 1.0f}, steeringModel, vehicleLayer);

    std::cout << "\n--- Sweeping toward a left-side wall ---\n";
    std::vector<float> sensors = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<float> leftClearances;
    std::vector<float> perceptronOutputs;
    std::vector<float> layerOutputs;
    for (int step = 0; step < 10; ++step)
    {
        sensors[0] = std::max(0.0f, sensors[0] - 0.1f);
        sensors[1] = std::max(0.0f, sensors[1] - 0.1f);
        const float perceptronOutput = steeringModel.predict(sensors);
        const std::vector<float> outputs = vehicleLayer.forward(sensors);
        const RuleAction expected = applyDrivingRules(sensors);

        leftClearances.push_back(sensors[0]);
        perceptronOutputs.push_back(perceptronOutput);
        layerOutputs.push_back(outputs[0]);

        printVector("Sensors: ", sensors);
        std::cout << " | rule: steer " << toString(expected.steering) << ", speed "
                  << toString(expected.speed)
                  << " | Perceptron steer " << toString(steeringFromOutput(perceptronOutput))
                  << " | Layer steer " << toString(steeringFromOutput(outputs[0]))
                  << ", speed " << toString(speedFromOutput(outputs[1])) << "\n";
    }
    printResponsePlot(leftClearances, perceptronOutputs, layerOutputs);

    std::cout << "\n=======================================\n";
    return 0;
}
