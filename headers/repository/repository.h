#pragma once
#include <vector>
#include "../domain/domain.h"
#include "IRepository.h"

class Repository : public IRepository {
protected:
    std::vector<Movie> movies;

    void loadFromCsvFile(const std::string& path);

public:
    Repository() = default;
    ~Repository() override;
    void addMovie(const Movie&) override;
    void removeMovie(const Movie&) override;
    void removeMovieByPosition(int) override;
    void insertMovieAtPosition(int, const Movie&) override;
    void updateMovie(int, const Movie&) override;
    bool validatePosition(int) const;
    std::vector<Movie> getMovies() const override;
    size_t getMoviesCount() const override;
    Movie getMovieByPosition(int) const override;
    int findMovie(const Movie&) const override;
};
