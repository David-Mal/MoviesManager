#include "../../headers/domain/domain.h"
#include <iostream>
#include "../../headers/utils.h"

Movie::Movie(const std::string& title, const std::string& genre, const std::string& trailer, int yearOfRelease, int likesCount)
    : title(title), genre(genre), trailer(trailer), yearOfRelease(yearOfRelease), likesCount(likesCount) {}

std::string Movie::getTitle() const { return title; }
std::string Movie::getGenre() const { return genre; }
std::string Movie::getTrailer() const { return trailer; }
int Movie::getYearOfRelease() const { return yearOfRelease; }
int Movie::getLikesCount() const { return likesCount; }

void Movie::setTitle(const std::string& title) { this->title = title; }
void Movie::setGenre(const std::string& genre) { this->genre = genre; }
void Movie::setTrailer(const std::string& trailer) { this->trailer = trailer; }
void Movie::setYearOfRelease(int yearOfRelease) { this->yearOfRelease = yearOfRelease; }
void Movie::setLikesCount(int likesCount) { this->likesCount = likesCount; }

bool Movie::operator==(const Movie& other) const {
    return title == other.title && yearOfRelease == other.yearOfRelease;
}

bool Movie::operator!=(const Movie& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Movie& movie) {
    os << movie.getTitle() << " | " << movie.getGenre() << " | " << movie.getTrailer()
       << " | " << movie.getYearOfRelease() << " | " << movie.getLikesCount();
    return os;
}

std::istream& operator>>(std::istream& is, Movie& movie) {
    std::string line;
    if (!std::getline(is, line)) {
        return is;
    }

    std::vector<std::string> tokens = Utils::split(line, " | ");
    if (tokens.size() < 5) {
        is.setstate(std::ios::failbit);
        return is;
    }

    try {
        int year = std::stoi(tokens[3]);
        int likes = std::stoi(tokens[4]);
        MovieValidator::validateMovieIdentifiers(tokens[0], tokens[1], tokens[2], year, likes);
        movie.setTitle(tokens[0]);
        movie.setGenre(tokens[1]);
        movie.setTrailer(tokens[2]);
        movie.setYearOfRelease(year);
        movie.setLikesCount(likes);
    } catch (...) {
        is.setstate(std::ios::failbit);
    }

    return is;
}

std::string Movie::toString() const {
    return title + " (" + std::to_string(yearOfRelease) + ")";
}
