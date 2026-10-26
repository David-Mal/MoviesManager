#pragma once
#include <string>
#include <vector>
#include "exceptions/exceptions.h"

class MovieValidator {
public:
    static const std::vector<std::string>& validGenres();
    static std::string canonicalizeGenre(const std::string& input);

    static void validateMovieTitle(const std::string&);
    static void validateMovieGenre(const std::string&);
    static void validateMovieTrailer(const std::string&);
    static void validateMovieYearOfRelease(const int&);
    static void validateMovieLikesCount(const int&);
    static void validateMovieIdentifiers(const std::string&, const std::string&,
                                        const std::string&, const int&, const int&);
};
