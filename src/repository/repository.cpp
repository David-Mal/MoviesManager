#include "../../headers/repository/repository.h"
#include "../../headers/exceptions/exceptions.h"
#include <fstream>
#include <iostream>

Repository::~Repository() = default;

void Repository::addMovie(const Movie& movie) {
    movies.push_back(movie);
}

void Repository::removeMovie(const Movie& movie) {
    const auto it = std::ranges::find(movies, movie);
    if (it == movies.end())
        throw RepoException("Movie not found\n");
    movies.erase(it);
}

void Repository::removeMovieByPosition(int position) {
    if (!validatePosition(position))
        throw RepoException("Index out of range\n");
    movies.erase(movies.begin() + position);
}

void Repository::insertMovieAtPosition(int position, const Movie& movie) {
    if (position < 0 || static_cast<size_t>(position) > movies.size())
        throw RepoException("Index out of range\n");
    movies.insert(movies.begin() + position, movie);
}

void Repository::updateMovie(int position, const Movie& updatedMovie) {
    if (!validatePosition(position))
        throw RepoException("Index out of range\n");
    movies[position] = updatedMovie;
}

bool Repository::validatePosition(int position) const {
    return position >= 0 && static_cast<size_t>(position) < movies.size();
}

std::vector<Movie> Repository::getMovies() const {
    return movies;
}

size_t Repository::getMoviesCount() const {
    return movies.size();
}

Movie Repository::getMovieByPosition(int position) const {
    if (!validatePosition(position))
        throw RepoException("Index out of range\n");
    return movies[position];
}

int Repository::findMovie(const Movie& movie) const {
    auto position = std::ranges::find(movies, movie);
    if (position == movies.end())
        return -1;
    return static_cast<int>(position - movies.begin());
}

// CSV format: title,year,genre,trailer,likes
// Parsed right-to-left so titles containing commas are handled correctly.
void Repository::loadFromCsvFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t p4 = line.rfind(',');
        if (p4 == std::string::npos || p4 == 0) continue;
        size_t p3 = line.rfind(',', p4 - 1);
        if (p3 == std::string::npos || p3 == 0) continue;
        size_t p2 = line.rfind(',', p3 - 1);
        if (p2 == std::string::npos || p2 == 0) continue;
        size_t p1 = line.rfind(',', p2 - 1);
        if (p1 == std::string::npos) continue;

        std::string title    = line.substr(0, p1);
        std::string yearStr  = line.substr(p1 + 1, p2 - p1 - 1);
        std::string genre    = line.substr(p2 + 1, p3 - p2 - 1);
        std::string trailer  = line.substr(p3 + 1, p4 - p3 - 1);
        std::string likesStr = line.substr(p4 + 1);

        try {
            int year  = std::stoi(yearStr);
            int likes = std::stoi(likesStr);
            Repository::addMovie(Movie(title, genre, trailer, year, likes));
        } catch (const std::exception& e) {
            std::cerr << "Skipping invalid watchlist entry: " << e.what() << '\n';
        }
    }
}
