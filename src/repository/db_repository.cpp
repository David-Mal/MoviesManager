#include "../../headers/repository/db_repository.h"

#include <memory>
#include <stdexcept>

static void openDb(sqlite3*& db, const std::string& name) {
    if (db) {
        return;
    }
    if (sqlite3_open(name.c_str(), &db) != SQLITE_OK) {
        const std::string message = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error("Failed to open the database: " + message);
    }
}

static void execOrThrow(sqlite3* db, const char* sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        const std::string error = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + error);
    }
}

static std::string columnText(sqlite3_stmt* stmt, int column) {
    const unsigned char* text = sqlite3_column_text(stmt, column);
    return text ? reinterpret_cast<const char*>(text) : "";
}

// finalizează automat statement-ul, chiar dacă se aruncă o excepție
using StmtGuard = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

static const char* CREATE_TABLE_SQL =
    "CREATE TABLE IF NOT EXISTS movies ("
    "Title TEXT, Genre TEXT, Trailer TEXT, "
    "YearOfRelease INTEGER, LikesCount INTEGER);";

DBRepository::~DBRepository() {
    sqlite3_close(db);
}

void DBRepository::loadMovies() {
    openDb(db, dbName);
    execOrThrow(db, CREATE_TABLE_SQL);
    const char* sql =
        "SELECT Title, Genre, Trailer, YearOfRelease, LikesCount "
        "FROM movies ORDER BY rowid;";
    sqlite3_stmt* raw = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("Failed to prepare the SQL statement: ") +
                                 sqlite3_errmsg(db));
    }
    StmtGuard stmt(raw, sqlite3_finalize);

    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        std::string title = columnText(stmt.get(), 0);
        std::string genre = columnText(stmt.get(), 1);
        std::string trailer = columnText(stmt.get(), 2);
        int year = sqlite3_column_int(stmt.get(), 3);
        int likes = sqlite3_column_int(stmt.get(), 4);

        Repository::addMovie(Movie(title, genre, trailer, year, likes));
    }
}

void DBRepository::saveMovies() {
    openDb(db, dbName);

    execOrThrow(db, "BEGIN TRANSACTION;");
    try {
        execOrThrow(db, "DROP TABLE IF EXISTS movies;");
        execOrThrow(db, CREATE_TABLE_SQL);

        sqlite3_stmt* raw = nullptr;
        const char* sql = "INSERT INTO movies VALUES (?, ?, ?, ?, ?);";
        if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
            throw std::runtime_error(std::string("Failed to prepare the SQL statement: ") +
                                     sqlite3_errmsg(db));
        }
        StmtGuard stmt(raw, sqlite3_finalize);

        for (const auto& movie : this->getMovies()) {
            const std::string title = movie.getTitle();
            const std::string genre = movie.getGenre();
            const std::string trailer = movie.getTrailer();

            sqlite3_bind_text(stmt.get(), 1, title.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt.get(), 2, genre.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt.get(), 3, trailer.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt.get(), 4, movie.getYearOfRelease());
            sqlite3_bind_int(stmt.get(), 5, movie.getLikesCount());

            if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
                throw std::runtime_error(std::string("Failed to save movie: ") +
                                         sqlite3_errmsg(db));
            }
            sqlite3_reset(stmt.get());
        }

        execOrThrow(db, "COMMIT;");
    } catch (...) {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        throw;
    }
}

void DBRepository::addMovie(const Movie& movie) {
    Repository::addMovie(movie);
    saveMovies();
}

void DBRepository::removeMovie(const Movie& movie) {
    Repository::removeMovie(movie);
    saveMovies();
}

void DBRepository::removeMovieByPosition(int pos) {
    Repository::removeMovieByPosition(pos);
    saveMovies();
}

void DBRepository::updateMovie(const int pos, const Movie& movie) {
    Repository::updateMovie(pos, movie);
    saveMovies();
}

void DBRepository::insertMovieAtPosition(const int pos, const Movie& movie) {
    Repository::insertMovieAtPosition(pos, movie);
    saveMovies();
}