#pragma once
#include "repository.h"

class CSVRepository: public Repository {
private:
    std::string filename;
public:
    explicit CSVRepository(const std::string& filename) : filename{filename} { loadMovies(); }
    ~CSVRepository() override;
    void loadMovies();
    void saveMovies() const;
    void addMovie(const Movie& movie) override;
    void removeMovie(const Movie& movie) override;
    void updateMovie(int, const Movie&) override;
    void removeMovieByPosition(int) override;
    void insertMovieAtPosition(int, const Movie&) override;
};
