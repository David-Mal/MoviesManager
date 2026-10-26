#include "../../headers/repository/file_repository.h"
#include <fstream>
#include <iostream>

FileRepository::~FileRepository() { saveMovies(); }

void FileRepository::loadMovies() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    while (file.peek() != std::ifstream::traits_type::eof()) {
        Movie movie;
        if (file >> movie) {
            try {
                Repository::addMovie(movie);
            } catch (const std::exception& e) {
                std::cerr << "Skipping invalid movie: " << e.what() << '\n';
            }
        }
    }
}

void FileRepository::saveMovies() {
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("File could not be opened");
    for (const auto& movie : getMovies())
        file << movie << '\n';
}

void FileRepository::addMovie(const Movie& movie) {
    Repository::addMovie(movie);
    saveMovies();
}

void FileRepository::removeMovie(const Movie& movie) {
    Repository::removeMovie(movie);
    saveMovies();
}

void FileRepository::removeMovieByPosition(int pos) {
    Repository::removeMovieByPosition(pos);
    saveMovies();
}

void FileRepository::updateMovie(int pos, const Movie& movie) {
    Repository::updateMovie(pos, movie);
    saveMovies();
}

void FileRepository::insertMovieAtPosition(int pos, const Movie& movie) {
    Repository::insertMovieAtPosition(pos, movie);
    saveMovies();
}
