#include "../../headers/service/service.h"

#include <algorithm>
#include <limits>
#include <fstream>
#include <cmath>
#include <memory>
#include <ctime>
#include <cstdlib>
#include "../../headers/domain/action.h"
#include <filesystem>
#include <random>
#include <stdexcept>
#include <iostream>

Service::Service(IRepository& repo, IRepository& watchlistRepo)
    : repo(repo), watchlistRepo(watchlistRepo) {}

void Service::clearRedoStack() {
    redoStack = std::stack<std::unique_ptr<Action>>();
}

void Service::addMovie(const std::string& title, const std::string& genre, int year,
                       int likes, const std::string& trailer) {
    MovieValidator::validateMovieIdentifiers(title, genre, trailer, year, likes);
    Movie movie(title, genre, trailer, year, likes);

    if (findMovie(title, genre, year, likes, trailer) != -1)
        throw std::invalid_argument("Movie already exists!");

    repo.addMovie(movie);

    undoStack.push(std::make_unique<ActionAdd>(movie, repo));
    clearRedoStack();
}

void Service::removeMovie(int index) {
    if (index < 0 || static_cast<size_t>(index) >= getMoviesCount())
        throw std::invalid_argument("Movie does not exist");

    auto movie = repo.getMovieByPosition(index);
    repo.removeMovie(movie);

    undoStack.push(std::make_unique<ActionRemove>(movie, repo, index));
    clearRedoStack();
}

void Service::updateMovie(int index, const std::string& title, const std::string& genre,
                          int year, int likes, const std::string& trailer) {
    MovieValidator::validateMovieIdentifiers(title, genre, trailer, year, likes);

    if (index < 0 || static_cast<size_t>(index) >= getMoviesCount())
        throw std::invalid_argument("Movie does not exist");

    Movie oldMovie = repo.getMovieByPosition(index);
    Movie newMovie(title, genre, trailer, year, likes);

    repo.updateMovie(index, newMovie);

    undoStack.push(std::make_unique<ActionUpdate>(oldMovie, newMovie, repo, index));
    clearRedoStack();
}

std::vector<Movie> Service::getAllMovies() const {
    return repo.getMovies();
}

size_t Service::getMoviesCount() const {
    return repo.getMoviesCount();
}

Movie Service::getMovieByPosition(int index) const {
    return repo.getMovieByPosition(index);
}

int Service::findMovie(const std::string& title, const std::string& genre, int year, int likes, const std::string& trailer) const {
    Movie movie(title, genre, trailer, year, likes);
    return repo.findMovie(movie);
}

int Service::generateNRandomMovies(int n) {
    struct RealMovie {
        std::string title;
        std::string genre;
        int year;
        std::string trailer;
    };
    std::vector<RealMovie> moviePool;

    #ifdef SEED_FILE_PATH
        std::ifstream file(SEED_FILE_PATH);
    #else
        std::ifstream file("movies_seed.txt");
    #endif
        if (!file.is_open()) {
            file.clear();
            file.open("movies_seed.txt");
        }
        if (!file.is_open()) {
            throw std::runtime_error("Could not open the seed file: movies_seed.txt");
        }
    auto trim = [](std::string text) {
        const char* whitespace = " \t\r\n";
        size_t start = text.find_first_not_of(whitespace);
        if (start == std::string::npos) return std::string();
        size_t end = text.find_last_not_of(whitespace);
        return text.substr(start, end - start + 1);
    };

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string title, genre, yearStr, trailer;

        std::getline(ss, title, '|');
        std::getline(ss, genre, '|');
        std::getline(ss, yearStr, '|');
        std::getline(ss, trailer, '|');

        title = trim(title);
        genre = trim(genre);
        yearStr = trim(yearStr);
        trailer = trim(trailer);

        if (title.empty() || genre.empty() || yearStr.empty() || trailer.empty())
            continue;

        try {
            moviePool.push_back({title, genre, std::stoi(yearStr), trailer});
        } catch (const std::exception&) {
            continue;
        }
    }

    std::erase_if(moviePool, [this](const RealMovie& movie) {
        return findMovie(movie.title, movie.genre, movie.year, 0, movie.trailer) != -1;
    });

    std::random_device rd;
    std::seed_seq seed{rd(), static_cast<unsigned int>(
        std::chrono::steady_clock::now().time_since_epoch().count())};
    std::mt19937 gen(seed);
    for (size_t i = moviePool.size(); i > 1; --i) {
        std::uniform_int_distribution<size_t> pick(0, i - 1);
        std::swap(moviePool[i - 1], moviePool[pick(gen)]);
    }

    std::uniform_int_distribution<> likesDist(1000, 150000);

    int added = 0;
    for (const RealMovie& movie : moviePool) {
        if (added >= n) break;
        try {
            addMovie(movie.title, movie.genre, movie.year, likesDist(gen), movie.trailer);
            added++;
        } catch (const std::exception& e) {
            std::cerr << "Failed to add seeded movie '" << movie.title << "': " << e.what() << '\n';
        }
    }
    return added;
}

std::vector<Movie> Service::filterMovies(const std::string& genre, int year, YearDirection direction) const {
    std::vector<Movie> allMovies = repo.getMovies();
    std::vector<Movie> filteredMovies;

    std::copy_if(allMovies.begin(), allMovies.end(), std::back_inserter(filteredMovies),
                 [&genre, year, direction](const Movie& movie) {
                     bool matchesGenre = genre.empty() || movie.getGenre() == genre;
                     bool matchesYear = true;

                     if (direction == YearDirection::BEFORE && year != -1)
                         matchesYear = movie.getYearOfRelease() < year;
                     else if (direction == YearDirection::AFTER && year != -1)
                         matchesYear = movie.getYearOfRelease() > year;

                     return matchesGenre && matchesYear;
                 });
    return filteredMovies;
}

void Service::addMovieToWatchlist(int index) {
    if (index < 0 || static_cast<size_t>(index) >= getMoviesCount())
        throw std::invalid_argument("Movie does not exist");

    Movie movie = repo.getMovieByPosition(index);

    if (watchlistRepo.findMovie(movie) != -1)
        throw std::invalid_argument("Movie is already in the watchlist!");

    watchlistRepo.addMovie(movie);
}

void Service::removeMovieFromWatchlist(int index) {
    const Movie movie = watchlistRepo.getMovieByPosition(index);
    watchlistRepo.removeMovie(movie);
}

void Service::openWatchlist(const std::string& filename) {
    namespace fs = std::filesystem;

    if (!fs::exists(filename))
        throw std::runtime_error("The watchlist file does not exist yet: " + filename);

    const std::string path = fs::absolute(filename).make_preferred().string();

#if defined(_WIN32)
    const std::string command = "start \"\" \"" + path + "\"";
#elif defined(__APPLE__)
    const std::string command = "open \"" + path + "\"";
#else
    const std::string command = "xdg-open \"" + path + "\"";
#endif

    if (std::system(command.c_str()) != 0)
        throw std::runtime_error("Could not open the watchlist file.");
}

std::vector<Movie> Service::getWatchlist() const {
    return watchlistRepo.getMovies();
}

void Service::undo() {
    if (undoStack.empty())
        throw std::runtime_error("Nothing to undo.");

    std::unique_ptr<Action> action = std::move(undoStack.top());
    undoStack.pop();
    try {
        action->undo();
    } catch (...) {
        undoStack.push(std::move(action));
        throw;
    }
    redoStack.push(std::move(action));
}

void Service::redo() {
    if (redoStack.empty())
        throw std::runtime_error("Nothing to redo.");

    std::unique_ptr<Action> action = std::move(redoStack.top());
    redoStack.pop();
    try {
        action->redo();
    } catch (...) {
        redoStack.push(std::move(action));
        throw;
    }
    undoStack.push(std::move(action));
}
