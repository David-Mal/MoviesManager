#include "../../headers/ui/ui.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "../../headers/domain/MovieValidator.h"
#include "../../headers/repository/csv_repository.h"
#include "../../headers/repository/html_repository.h"

namespace {

constexpr unsigned short MODE_EXIT   = 0;
constexpr unsigned short MODE_ADMIN  = 1;
constexpr unsigned short MODE_USER   = 2;
constexpr unsigned short MODE_BROWSE = 3;
constexpr unsigned short MODE_SELECT = 4;

struct InputClosed {};

bool parseInt(const std::string& text, int& result) {
    try {
        size_t pos = 0;
        int value = std::stoi(text, &pos);
        if (pos != text.size()) return false;
        result = value;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string truncate(const std::string& text, size_t maxLength) {
    if (text.size() <= maxLength) return text;
    return text.substr(0, maxLength - 3) + "...";
}

void printMovies(const std::vector<Movie>& movies) {
    if (movies.empty()) {
        std::cout << "No movies found!\n";
        return;
    }

    std::cout << std::left << std::setw(4) << "ID"
              << " | " << std::setw(30) << "Title"
              << " | " << std::setw(12) << "Genre"
              << " | " << std::setw(4) << "Year"
              << " | " << "Likes\n";
    std::cout << std::string(70, '-') << '\n';

    for (size_t i = 0; i < movies.size(); i++) {
        const Movie& movie = movies[i];
        std::cout << std::left << std::setw(4) << i + 1
                  << " | " << std::setw(30) << truncate(movie.getTitle(), 30)
                  << " | " << std::setw(12) << movie.getGenre()
                  << " | " << std::setw(4) << movie.getYearOfRelease()
                  << " | " << movie.getLikesCount() << '\n';
        std::cout << "       Trailer: " << movie.getTrailer() << '\n';
    }
    std::cout << '\n';
}

void printMovieCard(const Movie& movie, size_t index, size_t total) {
    std::cout << "\n  [" << index + 1 << " / " << total << "]"
              << "\n  Title:   " << movie.getTitle()
              << "\n  Genre:   " << movie.getGenre()
              << "\n  Year:    " << movie.getYearOfRelease()
              << "\n  Likes:   " << movie.getLikesCount()
              << "\n  Trailer: " << movie.getTrailer() << "\n\n";
}

void printGenreList() {
    const auto& genres = MovieValidator::validGenres();
    std::cout << "Valid genres:\n";
    for (size_t i = 0; i < genres.size(); i++) {
        std::cout << std::left << std::setw(14) << genres[i];
        if ((i + 1) % 5 == 0) std::cout << '\n';
    }
    if (genres.size() % 5 != 0) std::cout << '\n';
}

bool confirm(const std::string& question) {
    std::string answer;
    std::cout << question << " (y/n): ";
    if (!std::getline(std::cin, answer))
        throw InputClosed{};
    return answer == "y" || answer == "Y";
}

} // namespace

UI::UI(Service& service, std::string watchlistFile)
    : service(service), watchlistFile(std::move(watchlistFile)) {}

std::unique_ptr<IRepository> UI::chooseWatchlistRepo(std::string& watchlistFile) {
    std::cout << "Choose the format for the WATCHLIST:\n";
    std::cout << "1. CSV\n";
    std::cout << "2. HTML\n";

    if (readCommand(1, 2) == 1) {
        watchlistFile = "watchlist.csv";
        return std::make_unique<CSVRepository>(watchlistFile);
    }
    watchlistFile = "watchlist.html";
    return std::make_unique<HtmlRepository>(watchlistFile);
}

std::string UI::readLine(const std::string& prompt, bool allowEmpty) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) throw InputClosed{};
        if (allowEmpty || !line.empty()) return line;
        std::cout << "This field cannot be empty!\n";
    }
}

int UI::readInt(const std::string& prompt) {
    int value;
    while (true) {
        if (parseInt(readLine(prompt), value)) return value;
        std::cout << "Invalid number!\n";
    }
}

int UI::readCommand(int min, int max) {
    while (true) {
        int command = readInt(">> Command: >");
        if (command >= min && command <= max) return command;
        std::cout << "Invalid command!\n";
    }
}

void UI::printSelectModeMenu() {
    std::cout << "||==========================||\n";
    std::cout << "1. Administrator mode\n";
    std::cout << "2. User mode\n";
    std::cout << "0. Exit\n";
    std::cout << "||==========================||\n\n";
}

void UI::printAdminMenu() {
    std::cout << "||==========ADMIN===========||\n";
    std::cout << "1. Add movie\n";
    std::cout << "2. Update movie\n";
    std::cout << "3. Remove movie\n";
    std::cout << "4. List movies\n";
    std::cout << "5. Undo\n";
    std::cout << "6. Redo\n";
    std::cout << "7. Generate random movies\n";
    std::cout << "8. Switch to User mode\n";
    std::cout << "0. Back\n";
    std::cout << "||==========================||\n\n";
}

void UI::printUserMenu() {
    std::cout << "||===========USER===========||\n";
    std::cout << "1. Browse movies (add to watchlist)\n";
    std::cout << "2. List movies by genre and year\n";
    std::cout << "3. List watchlist\n";
    std::cout << "4. Remove from watchlist\n";
    std::cout << "5. Open watchlist file\n";
    std::cout << "6. Switch to Admin mode\n";
    std::cout << "0. Back\n";
    std::cout << "||==========================||\n\n";
}

void UI::printUserBrowseMenu() {
    std::cout << "||=========BROWSE===========||\n";
    std::cout << "1. Add to watchlist\n";
    std::cout << "2. Skip\n";
    std::cout << "0. Go back\n";
    std::cout << "||==========================||\n\n";
}

void UI::addMovie() {
    std::string title   = readLine("Title: ");
    printGenreList();
    std::string genre   = MovieValidator::canonicalizeGenre(readLine("Genre: "));
    int year            = readInt("Year: ");
    int likes           = readInt("Likes: ");
    std::string trailer = readLine("Trailer link: ");

    service.addMovie(title, genre, year, likes, trailer);
    std::cout << "Movie added successfully!\n";
}

void UI::updateMovie() {
    if (service.getMoviesCount() == 0) {
        std::cout << "No movies in the database!\n";
        return;
    }

    listMovies();
    int index = readInt("Number of the movie to update: ") - 1;
    if (index < 0 || static_cast<size_t>(index) >= service.getMoviesCount()) {
        std::cout << "Movie does not exist!\n";
        return;
    }

    Movie old = service.getMovieByPosition(index);
    std::cout << "Leave a field empty to keep the current value.\n";

    auto askKeepInt = [](const std::string& label, int current) {
        int value;
        while (true) {
            std::string text = readLine("New " + label + " [" + std::to_string(current) + "]: ", true);
            if (text.empty()) return current;
            if (parseInt(text, value)) return value;
            std::cout << "Invalid number!\n";
        }
    };

    std::string title   = readLine("New title ["   + old.getTitle()   + "]: ", true);
    printGenreList();
    std::string genre   = readLine("New genre ["   + old.getGenre()   + "]: ", true);
    int year            = askKeepInt("year",  old.getYearOfRelease());
    int likes           = askKeepInt("likes", old.getLikesCount());
    std::string trailer = readLine("New trailer [" + old.getTrailer() + "]: ", true);

    if (title.empty())   title   = old.getTitle();
    if (!genre.empty())  genre   = MovieValidator::canonicalizeGenre(genre);
    if (genre.empty())   genre   = old.getGenre();
    if (trailer.empty()) trailer = old.getTrailer();

    if (!confirm("Apply changes to \"" + old.getTitle() + "\"?")) {
        std::cout << "Update cancelled.\n";
        return;
    }

    service.updateMovie(index, title, genre, year, likes, trailer);
    std::cout << "Movie updated successfully!\n";
}

void UI::removeMovie() {
    if (service.getMoviesCount() == 0) {
        std::cout << "No movies in the database!\n";
        return;
    }

    listMovies();
    int index = readInt("Number of the movie to remove: ") - 1;
    if (index < 0 || static_cast<size_t>(index) >= service.getMoviesCount()) {
        std::cout << "Movie does not exist!\n";
        return;
    }

    Movie m = service.getMovieByPosition(index);
    if (!confirm("Remove \"" + m.getTitle() + " (" + std::to_string(m.getYearOfRelease()) + ")\"?")) {
        std::cout << "Removal cancelled.\n";
        return;
    }

    service.removeMovie(index);
    std::cout << "Movie removed successfully!\n";
}

void UI::listMovies() {
    printMovies(service.getAllMovies());
}

void UI::undo() {
    service.undo();
    std::cout << "Undo successful!\n";
}

void UI::redo() {
    service.redo();
    std::cout << "Redo successful!\n";
}

void UI::generateMovies() const {
    int n = readInt("How many movies to generate: ");
    if (n <= 0) {
        std::cout << "The number must be positive!\n";
        return;
    }

    int added = service.generateNRandomMovies(n);
    if (added == 0)
        std::cout << "No new movies available: all seed movies are already in the catalog.\n";
    else if (added < n)
        std::cout << "Only " << added << " new movies were available. Added " << added << ".\n";
    else
        std::cout << added << " movies generated!\n";
}

void UI::userFilterMovies() {
    printGenreList();
    std::string genre = readLine("Genre (empty for any): ", true);
    if (!genre.empty()) genre = MovieValidator::canonicalizeGenre(genre);

    int year = -1;
    YearDirection direction = YearDirection::BEFORE;

    while (true) {
        std::string text = readLine("Year (empty for any): ", true);
        if (text.empty()) break;
        if (parseInt(text, year)) break;
        year = -1;
        std::cout << "Invalid number!\n";
    }

    if (year != -1) {
        std::cout << "1. Movies released before " << year << '\n';
        std::cout << "2. Movies released after "  << year << '\n';
        direction = readCommand(1, 2) == 1 ? YearDirection::BEFORE : YearDirection::AFTER;
    }

    printMovies(service.filterMovies(genre, year, direction));
}

void UI::userGetWatchlist() {
    printMovies(service.getWatchlist());
}

void UI::removeFromWatchlist() {
    std::vector<Movie> watchlist = service.getWatchlist();
    if (watchlist.empty()) {
        std::cout << "The watchlist is empty!\n";
        return;
    }

    printMovies(watchlist);
    int index = readInt("Number of the movie to remove: ") - 1;
    if (index < 0 || static_cast<size_t>(index) >= watchlist.size()) {
        std::cout << "Movie does not exist!\n";
        return;
    }

    service.removeMovieFromWatchlist(index);
    std::cout << "Removed from watchlist!\n";
}

void UI::openWatchlistFile() {
    if (watchlistFile.empty()) {
        std::cout << "The watchlist is not stored in a file.\n";
        return;
    }
    service.openWatchlist(watchlistFile);
}

void UI::adminUI(unsigned short& mode) {
    printAdminMenu();
    unsigned short command = static_cast<unsigned short>(readCommand(0, 8));

    switch (command) {
        case 0: mode = MODE_SELECT; break;
        case 1: addMovie();         break;
        case 2: updateMovie();      break;
        case 3: removeMovie();      break;
        case 4: listMovies();       break;
        case 5: undo();             break;
        case 6: redo();             break;
        case 7: generateMovies();   break;
        case 8: mode = MODE_USER;   break;
    }
}

void UI::userUI(unsigned short& mode) {
    printUserMenu();
    unsigned short command = static_cast<unsigned short>(readCommand(0, 6));

    switch (command) {
        case 0: mode = MODE_SELECT;          break;
        case 1: mode = MODE_BROWSE;          break;
        case 2: userFilterMovies();          break;
        case 3: userGetWatchlist();          break;
        case 4: removeFromWatchlist();       break;
        case 5: openWatchlistFile();         break;
        case 6: mode = MODE_ADMIN;           break;
    }
}

void UI::browseUI(unsigned short& mode) {
    printGenreList();
    std::string genre = readLine("Genre (empty for all): ", true);
    if (!genre.empty()) genre = MovieValidator::canonicalizeGenre(genre);

    std::vector<Movie> movies = service.filterMovies(genre, -1, YearDirection::BEFORE);

    if (movies.empty()) {
        std::cout << "No movies found!\n";
        mode = MODE_USER;
        return;
    }

    size_t i = 0;
    while (true) {
        printMovieCard(movies[i], i, movies.size());
        printUserBrowseMenu();
        unsigned short command = static_cast<unsigned short>(readCommand(0, 2));

        if (command == 0) {
            mode = MODE_USER;
            return;
        }

        if (command == 1) {
            const Movie& m = movies[i];
            int index = service.findMovie(m.getTitle(), m.getGenre(),
                                          m.getYearOfRelease(), m.getLikesCount(),
                                          m.getTrailer());
            try {
                service.addMovieToWatchlist(index);
                std::cout << "Added to watchlist!\n";
            } catch (const std::invalid_argument& e) {
                std::cout << e.what() << '\n';
            }
        } else {
            std::cout << "Movie skipped!\n";
        }

        i = (i + 1) % movies.size();
    }
}

void UI::run() {
    unsigned short mode = MODE_SELECT;

    while (mode != MODE_EXIT) {
        try {
            if (mode == MODE_SELECT) {
                printSelectModeMenu();
                switch (readCommand(0, 2)) {
                    case 0: mode = MODE_EXIT;  break;
                    case 1: mode = MODE_ADMIN; break;
                    case 2: mode = MODE_USER;  break;
                }
            } else if (mode == MODE_ADMIN) {
                adminUI(mode);
            } else if (mode == MODE_USER) {
                userUI(mode);
            } else if (mode == MODE_BROWSE) {
                browseUI(mode);
            }
        } catch (const InputClosed&) {
            std::cout << "\nInput closed.\n";
            mode = MODE_EXIT;
        } catch (const MovieException& e) {
            std::cout << "Invalid movie:\n";
            for (const auto& error : e.getErrors())
                std::cout << "  - " << error.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }

    std::cout << "Goodbye!\n";
}
