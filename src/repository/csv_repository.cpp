#include "../../headers/repository/csv_repository.h"
#include <fstream>

CSVRepository::~CSVRepository() {
    saveMovies();
}

void CSVRepository::loadMovies() {
    loadFromCsvFile(filename);
}

void CSVRepository::saveMovies() const {
    std::ofstream file(filename);
    for (const auto& movie : getMovies()) {
        file << movie.getTitle()        << ','
             << movie.getYearOfRelease() << ','
             << movie.getGenre()         << ','
             << movie.getTrailer()       << ','
             << movie.getLikesCount()    << '\n';
    }
}

void CSVRepository::addMovie(const Movie& movie) {
    Repository::addMovie(movie);
    saveMovies();
}

void CSVRepository::removeMovie(const Movie& movie) {
    Repository::removeMovie(movie);
    saveMovies();
}

void CSVRepository::removeMovieByPosition(int pos) {
    Repository::removeMovieByPosition(pos);
    saveMovies();
}

void CSVRepository::updateMovie(int pos, const Movie& movie) {
    Repository::updateMovie(pos, movie);
    saveMovies();
}

void CSVRepository::insertMovieAtPosition(int pos, const Movie& movie) {
    Repository::insertMovieAtPosition(pos, movie);
    saveMovies();
}
