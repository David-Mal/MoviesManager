#pragma once
#include <sqlite3.h>

#include "repository.h"

class DBRepository : public Repository {
private:
    sqlite3 *db = nullptr;
    std::string dbName;

public:
    DBRepository(const std::string& dbName) : dbName(dbName) {loadMovies();}
    ~DBRepository() override;
    void loadMovies();
    void saveMovies();
    void addMovie(const Movie& movie) override;
    void removeMovie(const Movie& movie) override;
    void removeMovieByPosition(int) override;
    void updateMovie(int, const Movie& movie) override;
    void insertMovieAtPosition(int, const Movie& movie) override;
};