#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

class FileStorage {
private:
    std::string base_dir = "data";

    std::string get_filename(const std::string& index) {
        // Create a safe filename from index
        std::string safe_index;
        for (char c : index) {
            if (std::isalnum(c)) {
                safe_index += c;
            } else {
                safe_index += '_';
            }
        }
        return base_dir + "/" + safe_index + ".dat";
    }

public:
    FileStorage() {
        // Create data directory if it doesn't exist
        if (!fs::exists(base_dir)) {
            fs::create_directory(base_dir);
        }
    }

    void insert(const std::string& index, int value) {
        std::string filename = get_filename(index);
        std::vector<int> values;

        // Read existing values
        if (fs::exists(filename)) {
            std::ifstream infile(filename, std::ios::binary);
            if (infile.is_open()) {
                int val;
                while (infile.read(reinterpret_cast<char*>(&val), sizeof(int))) {
                    values.push_back(val);
                }
                infile.close();
            }
        }

        // Check if value already exists
        auto it = std::lower_bound(values.begin(), values.end(), value);
        if (it != values.end() && *it == value) {
            return; // Value already exists
        }

        // Insert value in sorted order
        values.insert(it, value);

        // Write back to file
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            for (int val : values) {
                outfile.write(reinterpret_cast<const char*>(&val), sizeof(int));
            }
            outfile.close();
        }
    }

    void remove(const std::string& index, int value) {
        std::string filename = get_filename(index);

        if (!fs::exists(filename)) {
            return; // File doesn't exist
        }

        std::vector<int> values;

        // Read existing values
        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            int val;
            while (infile.read(reinterpret_cast<char*>(&val), sizeof(int))) {
                if (val != value) {
                    values.push_back(val);
                }
            }
            infile.close();
        }

        // Write back to file
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            for (int val : values) {
                outfile.write(reinterpret_cast<const char*>(&val), sizeof(int));
            }
            outfile.close();
        }

        // If no values left, delete the file
        if (values.empty()) {
            fs::remove(filename);
        }
    }

    std::vector<int> find(const std::string& index) {
        std::string filename = get_filename(index);
        std::vector<int> values;

        if (!fs::exists(filename)) {
            return values;
        }

        // Read values from file
        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            int val;
            while (infile.read(reinterpret_cast<char*>(&val), sizeof(int))) {
                values.push_back(val);
            }
            infile.close();
        }

        return values;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    FileStorage storage;
    int n;
    std::cin >> n;
    std::cin.ignore(); // Consume newline

    for (int i = 0; i < n; i++) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);

        std::string command;
        iss >> command;

        if (command == "insert") {
            std::string index;
            int value;
            iss >> index >> value;
            storage.insert(index, value);
        } else if (command == "delete") {
            std::string index;
            int value;
            iss >> index >> value;
            storage.remove(index, value);
        } else if (command == "find") {
            std::string index;
            iss >> index;
            std::vector<int> values = storage.find(index);

            if (values.empty()) {
                std::cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) std::cout << " ";
                    std::cout << values[j];
                }
                std::cout << "\n";
            }
        }
    }

    return 0;
}