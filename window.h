#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

class GraphWindow {
public:
    GraphWindow(unsigned int width, unsigned int height, const std::string& title)
            : window(sf::VideoMode(width, height), title), window_width(width), window_height(height) {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("Font.ttf")) {
            std::cerr << "Failed to load font\n";
        }
    }

    bool isOpen() const {
        return window.isOpen();
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    void addDataPoint(int lineID, double value) {
        if (dataSets.find(lineID) == dataSets.end()) {
            dataSets[lineID] = std::vector<double>();
            colors[lineID] = generateColor(lineID);
        }
        dataSets[lineID].push_back(value);
    }

    void setLearningRate(int lineID, double learningRate) {
        learningRates[lineID] = learningRate;
    }

    void render() {
        window.clear(sf::Color::Black);
        drawGraph();
        drawAxesLabels();
        drawAxisTitles();
        drawKey();
        window.display();
    }

private:
    sf::RenderWindow window;
    float window_width;
    float window_height;
    sf::Font font;
    std::map<int, std::vector<double>> dataSets;
    std::map<int, double> learningRates;
    std::map<int, sf::Color> colors;

    void drawGraph() {
        if (dataSets.empty()) return;

        // Find global max and min for scaling
        double globalMax = -std::numeric_limits<double>::infinity();
        double globalMin = std::numeric_limits<double>::infinity();
        for (const auto& [id, data] : dataSets) {
            double maxVal = *std::max_element(data.begin(), data.end());
            double minVal = *std::min_element(data.begin(), data.end());
            if (maxVal > globalMax) globalMax = maxVal;
            if (minVal < globalMin) globalMin = minVal;
        }

        double midVal1 = globalMin + (globalMax - globalMin) * 0.25;
        double midVal2 = globalMin + (globalMax - globalMin) * 0.5;
        double midVal3 = globalMin + (globalMax - globalMin) * 0.75;

        // Draw axes with extra spacing for labels
        sf::Vertex xAxis[] = {
                sf::Vertex(sf::Vector2f(100, window_height - 80)),
                sf::Vertex(sf::Vector2f(window_width - 80, window_height - 80))
        };
        sf::Vertex yAxis[] = {
                sf::Vertex(sf::Vector2f(100, 50)),
                sf::Vertex(sf::Vector2f(100, window_height - 80))
        };

        window.draw(xAxis, 2, sf::Lines);
        window.draw(yAxis, 2, sf::Lines);

        double yScale = (window_height - 160) / (globalMax - globalMin);

        // Draw each data set
        for (const auto& [id, data] : dataSets) {
            double xSpacing = (window_width - 180) / static_cast<double>(data.size());
            std::vector<sf::Vertex> points;

            for (size_t i = 0; i < data.size(); ++i) {
                double x = 100 + i * xSpacing;
                double y = window_height - 80 - (data[i] - globalMin) * yScale;
                points.emplace_back(sf::Vertex(sf::Vector2f(x, y), colors[id]));
            }

            if (!points.empty()) {
                window.draw(&points[0], points.size(), sf::LinesStrip);
            }
        }
    }

    void drawAxesLabels() {
        if (dataSets.empty()) return;

        double globalMax = -std::numeric_limits<double>::infinity();
        double globalMin = std::numeric_limits<double>::infinity();
        for (const auto& [id, data] : dataSets) {
            double maxVal = *std::max_element(data.begin(), data.end());
            double minVal = *std::min_element(data.begin(), data.end());
            if (maxVal > globalMax) globalMax = maxVal;
            if (minVal < globalMin) globalMin = minVal;
        }

        double midVal1 = globalMin + (globalMax - globalMin) * 0.25;
        double midVal2 = globalMin + (globalMax - globalMin) * 0.5;
        double midVal3 = globalMin + (globalMax - globalMin) * 0.75;

        drawYAxisLabel(globalMin, 100, window_height - 80);
        drawYAxisLabel(midVal1, 100, window_height - 80 - (midVal1 - globalMin) * ((window_height - 160) / (globalMax - globalMin)));
        drawYAxisLabel(midVal2, 100, window_height - 80 - (midVal2 - globalMin) * ((window_height - 160) / (globalMax - globalMin)));
        drawYAxisLabel(midVal3, 100, window_height - 80 - (midVal3 - globalMin) * ((window_height - 160) / (globalMax - globalMin)));
        drawYAxisLabel(globalMax, 100, 50);

        // Draw up to 6 evenly spaced x-axis labels based on the longest data set
        size_t maxDataSize = 0;
        for (const auto& [id, data] : dataSets) {
            maxDataSize = std::max(maxDataSize, data.size());
        }

        size_t numLabels = std::min<size_t>(6, maxDataSize);
        if (numLabels > 1) {
            for (size_t i = 0; i < numLabels; ++i) {
                size_t index = i * (maxDataSize - 1) / (numLabels - 1);
                drawXAxisLabel(500 * index, 100 + index * ((window_width - 180) / static_cast<double>(maxDataSize)), window_height - 70);
            }
        }
    }

    void drawAxisTitles() {
        // Draw Y-axis title "COST"
        sf::Text yAxisTitle;
        yAxisTitle.setFont(font);
        yAxisTitle.setString("COST");
        yAxisTitle.setCharacterSize(20);
        yAxisTitle.setFillColor(sf::Color::White);
        yAxisTitle.setPosition(10, (window_height / 2) - (yAxisTitle.getLocalBounds().width / 2)); // Center vertically and move left
        yAxisTitle.setRotation(-90); // Rotate for vertical text
        window.draw(yAxisTitle);

        // Draw X-axis title "RUNS"
        sf::Text xAxisTitle;
        xAxisTitle.setFont(font);
        xAxisTitle.setString("RUNS");
        xAxisTitle.setCharacterSize(20);
        xAxisTitle.setFillColor(sf::Color::White);
        xAxisTitle.setPosition((window_width / 2) - 30, window_height - 40);
        window.draw(xAxisTitle);
    }

    void drawKey() {
        sf::Text keyTitle;
        keyTitle.setFont(font);
        keyTitle.setString("Learning Rates");
        keyTitle.setCharacterSize(16);
        keyTitle.setFillColor(sf::Color::White);
        keyTitle.setPosition(window_width - 180, 50);
        window.draw(keyTitle);

        int lineCount = 0;
        for (const auto& [id, rate] : learningRates) {
            sf::RectangleShape colorBox(sf::Vector2f(15, 15));
            colorBox.setFillColor(colors[id]);
            colorBox.setPosition(window_width - 180, 80 + lineCount * 30);
            window.draw(colorBox);

            sf::Text labelText;
            labelText.setFont(font);
            labelText.setString("LR = " + formatLabel(rate));
            labelText.setCharacterSize(14);
            labelText.setFillColor(sf::Color::White);
            labelText.setPosition(window_width - 155, 78 + lineCount * 30);
            window.draw(labelText);

            lineCount++;
        }
    }

    sf::Color generateColor(int index) {
        // Generate a color based on the index
        static std::vector<sf::Color> colorPalette = {
                sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Cyan, sf::Color::Magenta, sf::Color::Yellow
        };
        return colorPalette[index % colorPalette.size()];
    }

    void drawYAxisLabel(double value, float x, float y) {
        sf::Text label;
        label.setFont(font);
        label.setString(formatLabel(value));
        label.setCharacterSize(16); // Increase character size for better visibility
        label.setFillColor(sf::Color::White);
        label.setPosition(x - 60, y - 10); // Move left to avoid overlapping with "COST"
        window.draw(label);
    }

    void drawXAxisLabel(int run, float x, float y) {
        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(run));
        label.setCharacterSize(16); // Increase character size for better visibility
        label.setFillColor(sf::Color::White);
        label.setPosition(x - 15, y); // Adjust positioning for larger text
        window.draw(label);
    }

    std::string formatLabel(double value) {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << value;
        return stream.str();
    }
};
