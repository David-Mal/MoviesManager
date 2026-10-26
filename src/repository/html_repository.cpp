#include "../../headers/repository/html_repository.h"
#include <fstream>

std::string HtmlRepository::deriveCsvPath(const std::string& htmlPath) {
    constexpr std::string_view ext = ".html";
    if (htmlPath.size() > ext.size() &&
        htmlPath.compare(htmlPath.size() - ext.size(), ext.size(), ext) == 0)
        return htmlPath.substr(0, htmlPath.size() - ext.size()) + ".csv";
    return htmlPath + ".csv";
}

HtmlRepository::HtmlRepository(const std::string& filename)
    : filename(filename), csvFilename(deriveCsvPath(filename)) {
    loadMovies();
}

HtmlRepository::~HtmlRepository() {
    saveMovies();
}

void HtmlRepository::loadMovies() {
    loadFromCsvFile(csvFilename);
}

void HtmlRepository::saveMovies() const {
    // Write HTML for viewing
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("File could not be opened");

    file << R"(<!DOCTYPE html>
<html>
<head>
    <title>Movies-Watchlist</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f4f7f6;
            color: #333;
            margin: 40px;
        }
        h2 {
            text-align: center;
            color: #2c3e50;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 25px 0;
            font-size: 1.1em;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            background-color: white;
            border-radius: 8px;
            overflow: hidden;
        }
        th {
            background-color: #3498db;
            color: #ffffff;
            text-align: left;
            padding: 15px;
        }
        td {
            padding: 12px 15px;
            border-bottom: 1px solid #dddddd;
        }
        tr:nth-of-type(even) { background-color: #f9f9f9; }
        tr:hover { background-color: #f1f1f1; transition: 0.2s; }
        a {
            display: inline-block;
            padding: 8px 12px;
            background-color: #e74c3c;
            color: white;
            text-decoration: none;
            border-radius: 4px;
            font-weight: bold;
        }
        a:hover { background-color: #c0392b; }
    </style>
</head>
<body>
    <h2>My Movie Watchlist</h2>
    <table>
        <tr>
            <th>Title</th>
            <th>Genre</th>
            <th>Year of Release</th>
            <th>Likes Count</th>
            <th>Trailer</th>
        </tr>
)";

    for (const auto& movie : getMovies()) {
        file << "\t\t<tr>\n"
             << "\t\t\t<td>" << movie.getTitle()        << "</td>\n"
             << "\t\t\t<td>" << movie.getGenre()         << "</td>\n"
             << "\t\t\t<td>" << movie.getYearOfRelease() << "</td>\n"
             << "\t\t\t<td>" << movie.getLikesCount()    << "</td>\n"
             << "\t\t\t<td><a href=\"" << movie.getTrailer() << "\" target=\"_blank\">Play Trailer</a></td>\n"
             << "\t\t</tr>\n";
    }

    file << R"(    </table>
</body>
</html>)";

    // Write companion CSV as the persistent data source
    std::ofstream csv(csvFilename);
    for (const auto& movie : getMovies()) {
        csv << movie.getTitle()         << ','
            << movie.getYearOfRelease() << ','
            << movie.getGenre()         << ','
            << movie.getTrailer()       << ','
            << movie.getLikesCount()    << '\n';
    }
}

void HtmlRepository::addMovie(const Movie& movie) {
    Repository::addMovie(movie);
    saveMovies();
}

void HtmlRepository::removeMovie(const Movie& movie) {
    Repository::removeMovie(movie);
    saveMovies();
}

void HtmlRepository::removeMovieByPosition(int pos) {
    Repository::removeMovieByPosition(pos);
    saveMovies();
}

void HtmlRepository::updateMovie(int pos, const Movie& movie) {
    Repository::updateMovie(pos, movie);
    saveMovies();
}

void HtmlRepository::insertMovieAtPosition(int pos, const Movie& movie) {
    Repository::insertMovieAtPosition(pos, movie);
    saveMovies();
}
