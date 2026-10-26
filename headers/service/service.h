#pragma once
#include "repository/IRepository.h"
#include <stack>
#include <memory>
#include "domain/action.h"

enum class YearDirection {
    BEFORE,
    AFTER,
    ANY
};

class Service {
private:
    IRepository& repo;
    IRepository& watchlistRepo;
    std::stack<std::unique_ptr<Action>> undoStack;
    std::stack<std::unique_ptr<Action>> redoStack;

    void clearRedoStack();

public:
    Service(IRepository& repo, IRepository& watchlistRepo);
    void addMovie(const std::string& title, const std::string& genre, int year, int likes, const std::string& trailer);
    void removeMovie(int index);
    void updateMovie(int index, const std::string& title, const std::string& genre, int year, int likes, const std::string& trailer);
    [[nodiscard]] std::vector<Movie> getAllMovies() const;
    [[nodiscard]] size_t getMoviesCount() const;
    [[nodiscard]] Movie getMovieByPosition(int index) const;
    [[nodiscard]] int findMovie(const std::string& title, const std::string& genre, int year, int likes, const std::string& trailer) const;
    int generateNRandomMovies(int n);
    [[nodiscard]] std::vector<Movie> filterMovies(const std::string& genre, int year, YearDirection direction) const;
    void addMovieToWatchlist(int index);
    void removeMovieFromWatchlist(int index);
    static void openWatchlist(const std::string& filename);
    [[nodiscard]] std::vector<Movie> getWatchlist() const;
    void undo();
    void redo();
};
