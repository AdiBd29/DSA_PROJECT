#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;

string getInputSignal(string line, size_t openBracketPosition) {
    if (line.empty()) {
        return line;
    }
    
    std::string result = line.substr(openBracketPosition + 1, line.find(')') - openBracketPosition - 1);
    return result;
}

// Function to read the bench file and build the graph
unordered_map<string, vector<pair<string, int>>> constructGraphFromBench(const string &filename) {
    if (filename.empty()) {
        cerr << "Error in input filename" << endl;
        exit(1);
    }
    
    ifstream benchFile(filename);

    if (!benchFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    string line;
    std::unordered_map<std::string, int> nodeCount;
    unordered_map<string, vector<pair<string, int>>> graph;

    std::vector<std::string> lines;
    while (std::getline(benchFile, line)) {
        lines.push_back(line);
    }
    
    for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
        line = *it;

        // Skip empty lines 
        if (line.empty()) {
            continue;
        }

        //skip lines that do not contain an '='
        size_t equalsPosition = line.find('=');
        if (equalsPosition == std::string::npos) {
            continue;
        }
        
        std::string token;
        std::vector<std::string> inputGates;
        size_t commaPosition = 0;
        size_t openBracketPosition = line.find('(');
        
        // Extract the input string (everything between '(' and ')')
        //std::string inputString = line.substr(openBracketPosition + 1, line.find(')') - openBracketPosition - 1);
        std::string inputSignal = getInputSignal(line, openBracketPosition);

        // Extract the output signal
        std::string outputSignal = line.substr(0, equalsPosition);
        outputSignal.erase(std::remove_if(outputSignal.begin(), outputSignal.end(), ::isspace), outputSignal.end());

        // Split the input string by ','
        while ((commaPosition = inputSignal.find(',')) != std::string::npos) {
            token = inputSignal.substr(0, commaPosition);
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            inputGates.push_back(token);
            inputSignal.erase(0, commaPosition + 1);
        }
        inputSignal.erase(std::remove_if(inputSignal.begin(), inputSignal.end(), ::isspace), inputSignal.end());
        inputGates.push_back(inputSignal);

        // Update the connection count for each input gate
        for (const auto &inputGate : inputGates) {
            nodeCount[inputGate]++;
            // Add the connection with weight 1 to the connections map
            graph[outputSignal].push_back({inputGate, std::max(1, nodeCount[outputSignal])});
        }
    }
    benchFile.close();

    return graph;
}

// Function to display the connections in the graph
// void displayGraph(const unordered_map<string, vector<pair<string, int>>> &graph) {
//     cout << "Connections in the graph:" << endl;
//     for (const auto &entry : graph) {
//         const string &output = entry.first;
//         const vector<pair<string, int>> &inputs = entry.second;

//         cout << output << " <- ";
//         for (const auto &input : inputs) {
//             cout << input.first << "(" << input.second << ") ";
//         }
//         cout << endl;
//     }
// }


std::pair<std::vector<std::string>, int> getShortestPath(std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> &graph,
                                                  const std::string &start, const std::string &end) {
    
    std::priority_queue<std::pair<int, std::pair<std::string, std::vector<std::string>>>> queue;
    std::unordered_map<std::string, int> distance;

    queue.push({0, {start, {}}});

    while (!queue.empty()) {
        auto top = queue.top();
        queue.pop();

        int cost = -top.first;
        std::string node = top.second.first;
        std::vector<std::string> path = top.second.second;

        if (distance.find(node) != distance.end()) {
            continue;
        }

        distance[node] = cost;
        path.push_back(node);

        if (node == end) {
            std::reverse(path.begin(), path.end()); 
            return {path, cost};
        }

        for (auto neighbor : graph[node]) {
            std::string next_node = neighbor.first;
            int weight = neighbor.second;

            if (distance.find(next_node) == distance.end()) {
                queue.push({-cost - weight, {next_node, path}});
            }
        }
    }

    return {{}, 0}; // No path found
}


// use ./iscas ../input_bench/c17.bench G3gat G23gat to generate out

int main(int argc, char *argv[]) {
    // Check if the number of arguments is correct
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <bench_file> <source_node> <destination_node>" << endl;
        return 1;
    }

    // Forming the filenames and node strings
    //string benchFilename = "../input_bench/"+string(argv[1]);
    string sourceWithPrefix = string(argv[2]);
    string destinationWithPrefix = string(argv[3]);

    // Read the bench file and build the graph
    unordered_map<string, vector<pair<string, int>>> graph = constructGraphFromBench(benchFilename);

    // Check if the graph is empty (error reading the file)
    if (graph.empty()) {
        cerr << "Error reading the benchmark file." << endl;
        return 1;
    }

    // Display the connections in the graph
    //displayGraph(graph);
 
    // Find the shortest path from source to destination
    vector<string> shortest_path;
    int cost;
    tie(shortest_path, cost) = getShortestPath(graph, destinationWithPrefix, sourceWithPrefix);

    // Display the result
    if (!shortest_path.empty()) {
        cout << "Shortest path from " << sourceWithPrefix << " to " << destinationWithPrefix << ": ";
        for (const auto &node : shortest_path) {
            cout << node << " -->";

        }
        cout <<"destination" <<"\nCost: " << cost << endl;
    } else {
        cout << "No path found from " << sourceWithPrefix << " to " << destinationWithPrefix << "." << endl;
    }

    return 0;

}
