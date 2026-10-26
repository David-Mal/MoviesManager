#pragma once
#include "../../headers/repository/repository.h"

class FileRepository : public Repository {
private:
    std::string filename;
public:
    FileRepository(const std::string& filename): filename {filename} {
        loadMovies();
    };
    ~FileRepository() override;
    void loadMovies();
    void saveMovies();
    void addMovie(const Movie& movie) override;
    void removeMovie(const Movie& movie) override;
    void removeMovieByPosition(int) override;
    void updateMovie(int, const Movie& movie) override;
    void insertMovieAtPosition(int, const Movie& movie) override;
};