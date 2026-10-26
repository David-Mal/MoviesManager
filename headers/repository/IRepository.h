#pragma once
#include <vector>
#include "../domain/domain.h"

class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void addMovie(const Movie&) = 0;
    virtual void removeMovie(const Movie&) = 0;
    virtual void removeMovieByPosition(int) = 0;
    virtual void insertMovieAtPosition(int, const Movie&) = 0;
    virtual void updateMovie(int, const Movie&) = 0;
    virtual std::vector<Movie> getMovies() const = 0;
    virtual size_t getMoviesCount() const = 0;
    virtual Movie getMovieByPosition(int) const = 0;
    virtual int findMovie(const Movie&) const = 0;
};
