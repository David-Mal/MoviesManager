#pragma once
#include "repository.h"

class HtmlRepository: public Repository {
private:
    std::string filename;
    std::string csvFilename;

    static std::string deriveCsvPath(const std::string& htmlPath);

public:
    explicit HtmlRepository(const std::string& filename);
    ~HtmlRepository() override;
    void loadMovies();
    void saveMovies() const;
    void addMovie(const Movie& movie) override;
    void removeMovie(const Movie& movie) override;
    void updateMovie(int, const Movie&) override;
    void removeMovieByPosition(int) override;
    void insertMovieAtPosition(int, const Movie&) override;
};
