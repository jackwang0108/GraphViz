// Standard Library
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <filesystem>

// My Library
#include "SimpleGraph.h"

using namespace std;
namespace fs = std::filesystem;

const double Pi = 3.14159265358979323;

/**
 * @brief Prints a message to the console welcoming the user and
 * describing the program.
 */
void Welcome();

/**
 * @brief Get the values from is
 *
 * @tparam T value type
 * @param is input stream
 * @param num number of values to get
 * @return T value get from is
 */
template <typename T>
vector<T> getNumValues(istream &is, int num);

/**
 * @brief read test files from resDirPath
 *
 * @param resDirPath directory which stores test files
 * @return vector<fs::path> vector of test file
 */
vector<fs::path> readTestFileList(
    const fs::path &resDirPath = fs::path(__FILE__).parent_path().parent_path().append("res"));

/**
 * @brief load the graph from graphFilePath
 *
 * @param graphFilePath path of graph file
 * @return SimpleGraph grapg loaded
 */
SimpleGraph loadSimpleGraph(fs::path graphFilePath);

/**
 * @brief intialize the node position
 *
 * @param graph graph to initialize
 */
void initializeNodePosition(SimpleGraph &graph);

/**
 * @brief Set the x and y coordinates of nodes in graph
 *
 * @param graph graph to set
 */
void updateNodePosition(SimpleGraph &graph, double k_repel = 1e-3, double k_attract = 1e-3);

// Main method
int main()
{
    Welcome();
    vector<fs::path> testFileList = readTestFileList();
    for (size_t i = 0; i < testFileList.size(); i++)
    {
        cout << i << ": " << testFileList[i] << endl;
    }
    int selectedTestFileIndex = 1;
    cout << "Select a test file index (0-" << testFileList.size() - 1 << "): ";
    if (!(cin >> selectedTestFileIndex) || selectedTestFileIndex < 0 || (size_t)selectedTestFileIndex >= testFileList.size())
        throw domain_error("invalid test file index");
    cout << "You select: " << testFileList[selectedTestFileIndex] << endl;

    // Load the graph
    SimpleGraph graph = loadSimpleGraph(testFileList[selectedTestFileIndex]);

    // Assigning initial node positions
    initializeNodePosition(graph);

    InitGraphVisualizer(graph);

    sleep(5);

    double timeToRun = 50.0;
    double elapsedTime = 0.0;
    time_t startTime = time(nullptr);
    while ((elapsedTime = difftime(time(nullptr), startTime)) < timeToRun)
    {
        updateNodePosition(graph, 1e-3, 1e-3);
        DrawGraph(graph);
    }

    return 0;
}

void Welcome()
{
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

template <typename T>
vector<T> getNumValues(istream &is, int num)
{
    string line;
    // empty line, reach the end of the file
    if (!getline(is, line))
        return {};
    char remain;
    T value;
    istringstream iss(line);
    vector<T> valueList;
    while (num-- > 0)
    {
        if (!(iss >> value))
            throw domain_error("Get value from line failed!");
        valueList.push_back(value);
    }
    if (iss >> remain)
        throw domain_error("Invalid format");
    return valueList;
}

vector<fs::path> readTestFileList(const fs::path &resDirPath)
{
    cout << "Resource Directory: " << fs::absolute(resDirPath) << endl;
    if (!fs::exists(resDirPath) || !fs::is_directory(resDirPath))
        throw domain_error("Resource Directory doesn't exists or is not a directory!");
    vector<fs::path> testFilesList;
    for (const auto &testFile : fs::directory_iterator(resDirPath))
        if (fs::is_regular_file(testFile))
            testFilesList.push_back(fs::absolute(testFile));
    return testFilesList;
}

SimpleGraph loadSimpleGraph(fs::path graphFilePath)
{
    // check file
    if (!fs::exists(graphFilePath))
        throw domain_error("graph file doesn't exists!");
    if (!fs::is_regular_file(graphFilePath))
        throw domain_error("graph file format error!");

    // read file
    ifstream graphFile(graphFilePath);
    if (!graphFile)
        throw domain_error("failed to open graph file!");

    SimpleGraph graph;
    int nodeNum = getNumValues<int>(graphFile, 1)[0];
    graph.nodes.resize((size_t)nodeNum);
    while (!graphFile.eof())
    {
        vector<int> edge = getNumValues<int>(graphFile, 2);
        if (edge.size() == 2)
            graph.edges.push_back(
                Edge{.start = (size_t)edge[0], .end = (size_t)edge[1]});
    }
    return graph;
}

void initializeNodePosition(SimpleGraph &graph)
{
    size_t numNodes = graph.nodes.size();
    for (size_t k = 0; k < numNodes; k++)
    {
        graph.nodes[k].x = cos(2 * Pi * k / numNodes);
        graph.nodes[k].x = sin(2 * Pi * k / numNodes);
    }
}

void updateNodePosition(SimpleGraph &graph, double k_repel, double k_attract)
{
    size_t numNodes = graph.nodes.size();
    size_t numEdges = graph.edges.size();

    vector<Node> nodeDeltaFrame(numNodes);
    // Computing forces
    // repulsive force
    for (size_t n0 = 0; n0 < numNodes; n0++)
    {
        for (size_t n1 = 0; n1 < numNodes && n1 != n0; n1++)
        {
            double x_distance = graph.nodes[n1].x - graph.nodes[n0].x;
            double y_distance = graph.nodes[n1].y - graph.nodes[n0].y;
            double distance = sqrt(pow(x_distance, 2.0) + pow(y_distance, 2.0));
            double F_repel = k_repel / distance;
            double theta = atan2(y_distance, x_distance);
            nodeDeltaFrame[n0].x -= F_repel * cos(theta);
            nodeDeltaFrame[n0].y -= F_repel * sin(theta);
            nodeDeltaFrame[n1].x += F_repel * cos(theta);
            nodeDeltaFrame[n1].y += F_repel * sin(theta);
        }
    }
    // attractive forces
    for (size_t i = 0; i < numEdges; i++)
    {
        const auto [n0, n1] = graph.edges[i];
        double x_distance = graph.nodes[n1].x - graph.nodes[n0].x;
        double y_distance = graph.nodes[n1].y - graph.nodes[n0].y;
        double distance = pow(x_distance, 2.0) + pow(y_distance, 2.0);
        double F_attract = k_attract * distance;
        double theta = atan2(y_distance, x_distance);
        nodeDeltaFrame[n0].x += F_attract * cos(theta);
        nodeDeltaFrame[n0].y += F_attract * sin(theta);
        nodeDeltaFrame[n1].x -= F_attract * cos(theta);
        nodeDeltaFrame[n1].y -= F_attract * sin(theta);
    }

    // Moving Nodes According to the Forces
    for (size_t i = 0; i < numNodes; i++)
    {
        graph.nodes[i].x += nodeDeltaFrame[i].x;
        graph.nodes[i].y += nodeDeltaFrame[i].y;
    }
}
