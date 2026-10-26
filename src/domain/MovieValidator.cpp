#include "../../headers/domain/MovieValidator.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <ctime>

const std::vector<std::string>& MovieValidator::validGenres() {
    static const std::vector<std::string> genres = {
        "Action", "Adventure", "Animation", "Biography", "Comedy", "Crime",
        "Documentary", "Drama", "Family", "Fantasy", "Film-Noir", "Game-Show",
        "History", "Horror", "Music", "Musical", "Mystery", "News", "Reality-TV",
        "Romance", "Sci-Fi", "Short", "Sport", "Talk-Show", "Thriller", "War", "Western"
    };
    return genres;
}

std::string MovieValidator::canonicalizeGenre(const std::string& input) {
    for (const auto& genre : validGenres()) {
        if (genre.size() == input.size() &&
            std::equal(genre.begin(), genre.end(), input.begin(),
                [](char a, char b) {
                    return std::tolower(static_cast<unsigned char>(a)) ==
                           std::tolower(static_cast<unsigned char>(b));
                }))
            return genre;
    }
    return input;
}

void MovieValidator::validateMovieTitle(const std::string& title) {
    if (title.size() < 2)
        throw MovieException("Movie Title must be at least 2 characters long");
}

void MovieValidator::validateMovieGenre(const std::string& genre) {
    const auto& genres = validGenres();
    if (std::find(genres.begin(), genres.end(), genre) == genres.end())
        throw MovieException("Invalid genre! Accepted genres are only those available on IMDB.");
}

void MovieValidator::validateMovieTrailer(const std::string& trailer) {
    const std::regex pattern(
        R"(((http|https)://)(www\.)?[a-zA-Z0-9@:%._\+~#?&//=]{2,256}\.[a-z]{2,6}\b([-a-zA-Z0-9@:%._\+~#?&//=]*))");
    if (!std::regex_match(trailer, pattern))
        throw MovieException("Invalid trailer link. Only valid URLs are accepted.");
}

void MovieValidator::validateMovieYearOfRelease(const int& year) {
    if (year < 1888)
        throw MovieException("The first movie ever released was in 1888.");
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    if (year > currentYear)
        throw MovieException("The year of released movie is too high. Max allowed is: " + std::to_string(currentYear) + ".");
}

void MovieValidator::validateMovieLikesCount(const int& likes) {
    if (likes < 0)
        throw MovieException("The number of likes cannot be negative.");
}

void MovieValidator::validateMovieIdentifiers(const std::string& title, const std::string& genre,
                                              const std::string& trailer, const int& year, const int& likes) {
    std::vector<std::invalid_argument> errors;

    auto collect = [&errors](auto validation) {
        try {
            validation();
        } catch (const MovieException& e) {
            for (const auto& error : e.getErrors())
                errors.push_back(error);
        }
    };

    collect([&] { validateMovieTitle(title); });
    collect([&] { validateMovieGenre(genre); });
    collect([&] { validateMovieYearOfRelease(year); });
    collect([&] { validateMovieLikesCount(likes); });
    collect([&] { validateMovieTrailer(trailer); });

    if (!errors.empty())
        throw MovieException(errors);
}
