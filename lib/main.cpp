#include "Indexer.h"
#include "Searcher.h"

void ParseIndexer(size_t argc, char* argv[]) {
    std::string root;
    std::string output;
    for (int i = 2; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument.substr(0, 5) == "root=") {
            root = argument.substr(5);
        } else if (argument.substr(0, 7) == "output=") {
            output = argument.substr(7);
        }
    }
    auto index = Indexer(root, output);
    index.MakeIndex();
}

void ParseSearcher(size_t argc, char* argv[]) {
    std::string index;
    size_t k = 5;
    float b = 0.75;
    for (int i = 2; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument.substr(0, 6) == "index=") {
            index = argument.substr(6);
        }
    }
    auto searcher = Searcher(index, k, b);
    searcher.RunSearchApp();
}

int main(int argc, char* argv[]) {
    if (static_cast<std::string_view>(argv[1]) == "indexer") {
        ParseIndexer(argc, argv);
    } else if (static_cast<std::string_view>(argv[1]) == "searcher") {
        ParseSearcher(argc, argv);
    }
    return EXIT_SUCCESS;
}