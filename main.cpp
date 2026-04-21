#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <unordered_map>
#include <set>

// Use a more efficient file storage approach
// Store multiple indexes in buckets to reduce file count

class FileStorage {
private:
    static constexpr int BUCKET_COUNT = 100; // Reduce file count
    std::string base_dir = "data";

    int get_bucket_id(const std::string& index) {
        // Simple hash function
        int hash = 0;
        for (char c : index) {
            hash = (hash * 31 + c) % BUCKET_COUNT;
        }
        return hash;
    }

    std::string get_bucket_filename(int bucket_id) {
        return base_dir + "/bucket_" + std::to_string(bucket_id) + ".dat";
    }

    void ensure_directory_exists() {
        std::ifstream test(base_dir + "/.test");
        if (!test.good()) {
            std::system(("mkdir -p " + base_dir).c_str());
        }
    }

public:
    FileStorage() {
        ensure_directory_exists();
    }

    void insert(const std::string& index, int value) {
        int bucket_id = get_bucket_id(index);
        std::string filename = get_bucket_filename(bucket_id);

        // Read all data from bucket
        std::unordered_map<std::string, std::set<int>> bucket_data;

        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            // Read index count
            int index_count;
            if (infile.read(reinterpret_cast<char*>(&index_count), sizeof(int))) {
                for (int i = 0; i < index_count; i++) {
                    // Read index length and index
                    int index_len;
                    infile.read(reinterpret_cast<char*>(&index_len), sizeof(int));
                    std::string idx(index_len, '\0');
                    infile.read(&idx[0], index_len);

                    // Read value count for this index
                    int value_count;
                    infile.read(reinterpret_cast<char*>(&value_count), sizeof(int));

                    // Read values
                    for (int j = 0; j < value_count; j++) {
                        int val;
                        infile.read(reinterpret_cast<char*>(&val), sizeof(int));
                        bucket_data[idx].insert(val);
                    }
                }
            }
            infile.close();
        }

        // Insert the new value
        bucket_data[index].insert(value);

        // Write back to file
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            // Write index count
            int index_count = bucket_data.size();
            outfile.write(reinterpret_cast<const char*>(&index_count), sizeof(int));

            for (const auto& [idx, values] : bucket_data) {
                // Write index length and index
                int index_len = idx.length();
                outfile.write(reinterpret_cast<const char*>(&index_len), sizeof(int));
                outfile.write(idx.c_str(), index_len);

                // Write value count and values
                int value_count = values.size();
                outfile.write(reinterpret_cast<const char*>(&value_count), sizeof(int));
                for (int val : values) {
                    outfile.write(reinterpret_cast<const char*>(&val), sizeof(int));
                }
            }
            outfile.close();
        }
    }

    void remove(const std::string& index, int value) {
        int bucket_id = get_bucket_id(index);
        std::string filename = get_bucket_filename(bucket_id);

        if (!std::ifstream(filename).good()) {
            return;
        }

        // Read all data from bucket
        std::unordered_map<std::string, std::set<int>> bucket_data;

        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            // Read index count
            int index_count;
            if (infile.read(reinterpret_cast<char*>(&index_count), sizeof(int))) {
                for (int i = 0; i < index_count; i++) {
                    // Read index length and index
                    int index_len;
                    infile.read(reinterpret_cast<char*>(&index_len), sizeof(int));
                    std::string idx(index_len, '\0');
                    infile.read(&idx[0], index_len);

                    // Read value count for this index
                    int value_count;
                    infile.read(reinterpret_cast<char*>(&value_count), sizeof(int));

                    // Read values
                    for (int j = 0; j < value_count; j++) {
                        int val;
                        infile.read(reinterpret_cast<char*>(&val), sizeof(int));
                        bucket_data[idx].insert(val);
                    }
                }
            }
            infile.close();
        }

        // Remove the value
        auto it = bucket_data.find(index);
        if (it != bucket_data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                bucket_data.erase(it);
            }
        }

        // Write back to file
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.is_open()) {
            // Write index count
            int index_count = bucket_data.size();
            outfile.write(reinterpret_cast<const char*>(&index_count), sizeof(int));

            for (const auto& [idx, values] : bucket_data) {
                // Write index length and index
                int index_len = idx.length();
                outfile.write(reinterpret_cast<const char*>(&index_len), sizeof(int));
                outfile.write(idx.c_str(), index_len);

                // Write value count and values
                int value_count = values.size();
                outfile.write(reinterpret_cast<const char*>(&value_count), sizeof(int));
                for (int val : values) {
                    outfile.write(reinterpret_cast<const char*>(&val), sizeof(int));
                }
            }
            outfile.close();
        }
    }

    std::vector<int> find(const std::string& index) {
        int bucket_id = get_bucket_id(index);
        std::string filename = get_bucket_filename(bucket_id);

        if (!std::ifstream(filename).good()) {
            return {};
        }

        // Read all data from bucket
        std::unordered_map<std::string, std::set<int>> bucket_data;

        std::ifstream infile(filename, std::ios::binary);
        if (infile.is_open()) {
            // Read index count
            int index_count;
            if (infile.read(reinterpret_cast<char*>(&index_count), sizeof(int))) {
                for (int i = 0; i < index_count; i++) {
                    // Read index length and index
                    int index_len;
                    infile.read(reinterpret_cast<char*>(&index_len), sizeof(int));
                    std::string idx(index_len, '\0');
                    infile.read(&idx[0], index_len);

                    // Read value count for this index
                    int value_count;
                    infile.read(reinterpret_cast<char*>(&value_count), sizeof(int));

                    // Read values
                    for (int j = 0; j < value_count; j++) {
                        int val;
                        infile.read(reinterpret_cast<char*>(&val), sizeof(int));
                        bucket_data[idx].insert(val);
                    }
                }
            }
            infile.close();
        }

        // Return values for the requested index
        auto it = bucket_data.find(index);
        if (it != bucket_data.end()) {
            return std::vector<int>(it->second.begin(), it->second.end());
        }

        return {};
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