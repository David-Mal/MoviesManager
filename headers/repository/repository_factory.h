#pragma once
#include <memory>
#include <string>
#include "IRepository.h"

enum class MovieRepoType { Txt, Sql };
enum class WatchlistRepoType { Csv, Html };

std::unique_ptr<IRepository> makeMovieRepository(MovieRepoType type);
std::unique_ptr<IRepository> makeWatchlistRepository(WatchlistRepoType type);
std::string watchlistFilePath(WatchlistRepoType type);
