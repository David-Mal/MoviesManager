#include "repository/repository_factory.h"
#include <memory>
#include "repository/csv_repository.h"
#include "repository/db_repository.h"
#include "repository/file_repository.h"
#include "repository/html_repository.h"

std::unique_ptr<IRepository> makeMovieRepository(const MovieRepoType type) {
    switch (type) {
        case MovieRepoType::Txt: return std::make_unique<FileRepository>("../movies.txt");
        case MovieRepoType::Sql: return std::make_unique<DBRepository>("../movies.db");
    }
    throw std::invalid_argument("Unknown movie repository type");
}

std::string watchlistFilePath(const WatchlistRepoType type) {
    return type == WatchlistRepoType::Csv ? "../watchlist.csv" : "../watchlist.html";
}

std::unique_ptr<IRepository> makeWatchlistRepository(const WatchlistRepoType type) {
    switch (type) {
        case WatchlistRepoType::Csv:  return std::make_unique<CSVRepository>(watchlistFilePath(type));
        case WatchlistRepoType::Html: return std::make_unique<HtmlRepository>(watchlistFilePath(type));
    }
    throw std::invalid_argument("Unknown watchlist repository type");
}
