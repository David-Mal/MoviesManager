#include "StartUp.h"
#include <iostream>
#include <limits>

static int readChoice(int min, int max) {
    int value;
    while (true) {
        std::cout << ">> Command: >";
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        if (std::cin.eof()) throw std::runtime_error("Input closed");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid command!" << std::endl;
    }
}

MovieRepoType askMovieRepoType() {
    std::cout << "Choose the type of repository for MOVIES:\n1. TXT\n2. SQL\n";
    return readChoice(1, 2) == 1 ? MovieRepoType::Txt : MovieRepoType::Sql;
}

WatchlistRepoType askWatchlistRepoType() {
    std::cout << "Choose the type of repository for WATCHLIST:\n1. CSV\n2. HTML\n";
    return readChoice(1, 2) == 1 ? WatchlistRepoType::Csv : WatchlistRepoType::Html;
}