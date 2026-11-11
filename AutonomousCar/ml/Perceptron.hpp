#pragma once
#include <vector>
#include <cmath>

class Perceptron
{
public:
    enum class ActivationFunction
    {
        Step,
        Sigmoid,
        Tanh,
        ReLU
    };

    std::vector<float> weights;
    float bias;

    Perceptron(size_t inputSize)
        : weights(inputSize, 0.0f), bias(0.0f), activationFunction(ActivationFunction::Step) {}

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

    float predict(const std::vector<float> &inputs) const
    {
        float sum = bias;
        for (size_t i = 0; i < weights.size(); ++i)
        {
            sum += weights[i] * inputs[i];
        }

        return activateFunction(sum, activationFunction);
    }

private:
    ActivationFunction activationFunction;
};