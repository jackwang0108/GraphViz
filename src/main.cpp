// Standard Library
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

// My Library
#include "SimpleGraph.h"

using namespace std;
namespace fs = std::filesystem;

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

// Main method
int main()
{
    Welcome();
    vector<fs::path> testFileList = readTestFileList();
    for (size_t i = 0; i < testFileList.size(); i++)
    {
        cout << i << ": " << testFileList[i] << endl;
    }
    int selectedTestFileIndex = 0;
    cout << "You select: " << testFileList[selectedTestFileIndex] << endl;

    ifstream testFile(testFileList[selectedTestFileIndex], ios::in);
    if (!testFile)
    {
        std::cerr << "Failed to open testFile";
    }

    int nodeNum = getNumValues<int>(testFile, 1)[0];
    cout << "Node num " << nodeNum << endl;

    SimpleGraph graph;
    graph.nodes = vector<Node>(nodeNum, {0, 0});
    while (!testFile.eof())
    {
        vector<int> edge = getNumValues<int>(testFile, 2);
        // except last line
        if (edge.size() == 2)
            graph.edges.push_back(Edge{(size_t)edge[0], (size_t)edge[1]});
    }

    for (auto e : graph.edges)
        cout << "Edge: " << e.start << " <---> " << e.end << endl;

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