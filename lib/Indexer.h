#pragma once
#include <unordered_map>
#include <cstring>
#include <algorithm>
#include "VarInt.h"
#include "InfoStructures.h"

class Indexer {
public:
    Indexer(const std::string& root, const std::string& out);
    void MakeIndex();

private:
    void TraverseDirectory();
    void WriteToFile();
    size_t ParseLine(std::string& line, size_t line_number);

    std::filesystem::path root_directory_;
    std::filesystem::path output_;
    std::unordered_map<std::string, std::vector<FullInfo>> index_table_;
    std::vector<std::pair<std::string, size_t>> file_indexes_;
    size_t current_index_ = 0;
    bool is_chunked_;
    VarInt<size_t> var_int_;
};