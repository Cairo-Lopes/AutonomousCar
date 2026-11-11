#pragma once
#include <vector>
#include <cmath>
#include <cstdlib>

class Layer
{
public:
    enum class ActivationFunction
    {
        Step,
        Sigmoid,
        Tanh,
        ReLU
    };

    int numInputs;
    int numNeurons;

    std::vector<std::vector<float>> weights; // [numInputs x numNeurons]
    std::vector<float> biases;               // [numNeurons]

    Layer(int inputs, int neurons) : numInputs(inputs), numNeurons(neurons), activationFunction(ActivationFunction::Tanh)
    {
        weights.resize(inputs, std::vector<float>(neurons));
        biases.resize(neurons, 0.0f);
        randomize();
    }

    void setActivationFunction(ActivationFunction function)
    {
        activationFunction = function;
    }
    float activateFunction(float value, ActivationFunction function) const
    {
        switch (function)
        {
        case ActivationFunction::Sigmoid:
            return 1.0f / (1.0f + std::exp(-value));
        case ActivationFunction::Tanh:
            return std::tanh(value);
        case ActivationFunction::ReLU:
            return value > 0.0f ? value : 0.0f;
        case ActivationFunction::Step:
        default:
            return value >= 0.0f ? 1.0f : 0.0f;
        }
    }

    void randomize()
    {
        // Randomize weights in [-1.0, 1.0]
        for (auto &row : weights)
        {
            for (float &w : row)
            {
                w = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            }
        }

        // Randomize biases in [-1.0, 1.0]
        for (float &b : biases)
        {
            b = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        }
    }

    // Process inputs through this layer
    std::vector<float> forward(const std::vector<float> &inputs) const
    {
        std::vector<float> outputs(numNeurons, 0.0f);
        for (int j = 0; j < numNeurons; ++j)
        {
            float sum = biases[j];
            for (int i = 0; i < numInputs; ++i)
            {
                sum += inputs[i] * weights[i][j];
            }
            outputs[j] = activateFunction(sum, activationFunction);
        }
        return outputs;
    }

private:
    ActivationFunction activationFunction;
};