#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include "MovieValidator.h"

class Movie {
private:
    std::string title;
    std::string genre;
    std::string trailer;
    int yearOfRelease;
    int likesCount;

public:
    Movie(const std::string& title = "", const std::string& genre = "", const std::string& trailer = "", int yearOfRelease = 0, int likesCount = 0);

    std::string getTitle() const;
    std::string getGenre() const;
    std::string getTrailer() const;
    int getYearOfRelease() const;
    int getLikesCount() const;

    void setTitle(const std::string& title);
    void setGenre(const std::string& genre);
    void setTrailer(const std::string& trailer);
    void setYearOfRelease(int yearOfRelease);
    void setLikesCount(int likesCount);

    bool operator==(const Movie& other) const;
    bool operator!=(const Movie& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Movie& movie);
    friend std::istream& operator>>(std::istream& is, Movie& movie);

    std::string toString() const;
};
