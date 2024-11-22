
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <fstream>

#include "neural_network.h"

using namespace std;
using  namespace Rebecca;

string DIR = "../network/";

int main() {

    // Paths to MNIST files
     string imageFilePath = "set1-images.idx3-ubyte";
     string labelFilePath = "set1-labels.idx1-ubyte";

    // Read images and labels
    int numImages = 200000;
    int numRows = 28;
    int numCols = 28;

    // setting up vectors for images and labels
    vector< vector<double>> images = readMNISTImages(imageFilePath, numImages, numRows, numCols);
    vector<int> labels = readMNISTLabels(labelFilePath, numImages);

    int input_layer = 784;
    int output_layer = 10;
    int number_hidden_layers = 2;
    int number_node_per_hidden = 128;

    vector<double> startingWeights = generateStartingWeights(input_layer, number_hidden_layers, number_node_per_hidden, output_layer);
    vector<double> startingBiases = generateStartingBiases(number_hidden_layers, number_node_per_hidden, output_layer);

    int count = 0;
    ThreadNetworks allNetworks(5, 0.1, 1, startingWeights, startingBiases, input_layer,
              number_hidden_layers,number_node_per_hidden,
              output_layer, DIR +"outputWeights4.bin", DIR +"outputBiases4.bin");

    GraphWindow window_(1000, 600, "REBECCA", &allNetworks);

    allNetworks.SetWindow(window_);

    while (window_.run_network) {
        int i = getRandom(0, images.size());

        vector<double> correct_label_output(10, 0.0);
        correct_label_output[labels[i]] = 1.0;

        allNetworks.runThreading(images[i], correct_label_output);

        count++;

        if (count == 100) {
            allNetworks.PrintCost();
            count = 0;
        }

        // Render
        window_.render();
        window_.handleEvents();
    }

    return 0;
}