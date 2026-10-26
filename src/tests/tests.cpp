//
// Created by David on 10/23/2026.
//
#include "../../headers/tests/tests.h"

#include <cassert>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../headers/domain/domain.h"
#include "../../headers/domain/MovieValidator.h"
#include "../../headers/repository/repository.h"
#include "../../headers/service/service.h"


template <typename Exc, typename F>
static bool throwsException(F f) {
    try {
        f();
    } catch (const Exc&) {
        return true;
    }
    return false;
}

static const std::string TRAILER = "https://www.youtube.com/watch?v=abc123";

static Movie makeMovie(const std::string& title, const int year, const int likes = 100,
                       const std::string& genre = "Drama") {
    return Movie(title, genre, TRAILER, year, likes);
}

static void addTo(Service& service, const std::string& title, const int year,
                  const int likes = 100, const std::string& genre = "Drama") {
    service.addMovie(title, genre, year, likes, TRAILER);
}

static void testMovie() {
    Movie movie("Inception", "Sci-Fi", TRAILER, 2010, 1000);
    assert(movie.getTitle() == "Inception");
    assert(movie.getGenre() == "Sci-Fi");
    assert(movie.getTrailer() == TRAILER);
    assert(movie.getYearOfRelease() == 2010);
    assert(movie.getLikesCount() == 1000);

    movie.setTitle("Tenet");
    movie.setGenre("Action");
    movie.setTrailer("https://example.com/t");
    movie.setYearOfRelease(2020);
    movie.setLikesCount(5);
    assert(movie.getTitle() == "Tenet");
    assert(movie.getGenre() == "Action");
    assert(movie.getTrailer() == "https://example.com/t");
    assert(movie.getYearOfRelease() == 2020);
    assert(movie.getLikesCount() == 5);

    Movie a("Dune", "Sci-Fi", TRAILER, 2021, 10);
    Movie sameKey("Dune", "Drama", "https://other.com/x", 2021, 99999);
    Movie otherYear("Dune", "Sci-Fi", TRAILER, 1984, 10);
    Movie otherTitle("Dune 2", "Sci-Fi", TRAILER, 2021, 10);
    assert(a == sameKey);
    assert(!(a != sameKey));
    assert(a != otherYear);
    assert(a != otherTitle);

    assert(a.toString() == "Dune (2021)");

    std::ostringstream out;
    out << a;
    assert(out.str() == "Dune | Sci-Fi | " + TRAILER + " | 2021 | 10");
}


static void testValidator() {
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieTitle(""); }));
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieTitle("A"); }));
    MovieValidator::validateMovieTitle("It");

    assert(throwsException<MovieException>([] { MovieValidator::validateMovieGenre("Ab"); }));
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieGenre("Nonexistent"); }));
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieGenre("drama"); }));
    MovieValidator::validateMovieGenre("Drama");
    MovieValidator::validateMovieGenre("Sci-Fi");

    assert(throwsException<MovieException>([] { MovieValidator::validateMovieTrailer("not a link"); }));
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieTrailer("www.youtube.com/x"); }));
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieTrailer(""); }));
    MovieValidator::validateMovieTrailer(TRAILER);

    const std::time_t t = std::time(nullptr);
    const int currentYear = std::localtime(&t)->tm_year + 1900;
    assert(throwsException<MovieException>([] { MovieValidator::validateMovieYearOfRelease(1887); }));
    assert(throwsException<MovieException>([=] { MovieValidator::validateMovieYearOfRelease(currentYear + 1); }));
    MovieValidator::validateMovieYearOfRelease(1888);
    MovieValidator::validateMovieYearOfRelease(currentYear);

    assert(throwsException<MovieException>([] { MovieValidator::validateMovieLikesCount(-1); }));
    MovieValidator::validateMovieLikesCount(0);

    MovieValidator::validateMovieIdentifiers("Inception", "Sci-Fi", TRAILER, 2010, 100);

    try {
        MovieValidator::validateMovieIdentifiers("A", "xx", "bad", 1000, -5);
        assert(false);
    } catch (MovieException& e) {
        assert(e.getErrors().size() == 5);
    }

    try {
        MovieValidator::validateMovieIdentifiers("Inception", "Sci-Fi", TRAILER, 2010, -1);
        assert(false);
    } catch (MovieException& e) {
        assert(e.getErrors().size() == 1);
    }
}


static void testRepository() {
    Repository repo;
    Movie a = makeMovie("Alpha", 2001);
    Movie b = makeMovie("Beta", 2002);
    Movie c = makeMovie("Gamma", 2003);

    assert(repo.getMoviesCount() == 0);
    assert(repo.getMovies().empty());

    repo.addMovie(a);
    repo.addMovie(b);
    repo.addMovie(c);
    assert(repo.getMoviesCount() == 3);
    assert(repo.findMovie(a) == 0);
    assert(repo.findMovie(b) == 1);
    assert(repo.findMovie(c) == 2);
    assert(repo.findMovie(makeMovie("Missing", 1999)) == -1);

    assert(repo.getMovieByPosition(0) == a);
    assert(repo.getMovieByPosition(2) == c);

    Movie b2 = makeMovie("Beta2", 2002, 555);
    repo.updateMovie(1, b2);
    assert(repo.getMoviesCount() == 3);
    assert(repo.getMovieByPosition(1) == b2);
    assert(repo.getMovieByPosition(1).getLikesCount() == 555);
    assert(repo.getMovieByPosition(0) == a);
    assert(repo.getMovieByPosition(2) == c);

    Movie d = makeMovie("Delta", 2004);
    repo.insertMovieAtPosition(1, d);
    assert(repo.getMoviesCount() == 4);
    assert(repo.getMovieByPosition(1) == d);
    assert(repo.getMovieByPosition(2) == b2);

    Movie e = makeMovie("Epsilon", 2005);
    repo.insertMovieAtPosition(repo.getMoviesCount(), e);
    assert(repo.getMoviesCount() == 5);
    assert(repo.getMovieByPosition(4) == e);

    repo.removeMovie(e);
    assert(repo.getMoviesCount() == 4);
    assert(repo.findMovie(e) == -1);

    repo.removeMovieByPosition(0);
    assert(repo.getMoviesCount() == 3);
    assert(repo.findMovie(a) == -1);
    assert(repo.getMovieByPosition(0) == d);

    assert(throwsException<std::exception>([&] { repo.removeMovieByPosition(-1); }));
    assert(throwsException<std::exception>([&] { repo.removeMovieByPosition(99); }));
    assert(throwsException<std::exception>([&] { repo.updateMovie(99, a); }));
    assert(throwsException<std::exception>([&] { repo.insertMovieAtPosition(99, a); }));
    assert(throwsException<std::exception>([&] { repo.removeMovie(makeMovie("Missing", 1999)); }));
}


static void testServiceCrud() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    assert(service.getMoviesCount() == 0);

    addTo(service, "Inception", 2010, 1000, "Sci-Fi");
    addTo(service, "Whiplash", 2014, 500, "Drama");
    assert(service.getMoviesCount() == 2);
    assert(service.getAllMovies().size() == 2);
    assert(service.getMovieByPosition(0).getTitle() == "Inception");
    assert(service.getMovieByPosition(1).getTitle() == "Whiplash");

    assert(service.findMovie("Inception", "Sci-Fi", 2010, 1000, TRAILER) == 0);
    assert(service.findMovie("Whiplash", "Drama", 2014, 500, TRAILER) == 1);
    assert(service.findMovie("Missing", "Drama", 1999, 1, TRAILER) == -1);

    assert(throwsException<std::exception>([&] { addTo(service, "Inception", 2010, 1, "Drama"); }));
    assert(service.getMoviesCount() == 2);

    addTo(service, "Inception", 2020, 1, "Drama");
    assert(service.getMoviesCount() == 3);

    service.updateMovie(1, "Whiplash", "Drama", 2014, 9999, TRAILER);
    assert(service.getMoviesCount() == 3);
    assert(service.getMovieByPosition(1).getLikesCount() == 9999);
    assert(service.getMovieByPosition(0).getTitle() == "Inception");

    assert(throwsException<std::exception>([&] { service.updateMovie(-1, "Abc", "Drama", 2000, 1, TRAILER); }));
    assert(throwsException<std::exception>([&] { service.updateMovie(99, "Abc", "Drama", 2000, 1, TRAILER); }));

    service.removeMovie(0);
    assert(service.getMoviesCount() == 2);
    assert(service.getMovieByPosition(0).getTitle() == "Whiplash");

    assert(throwsException<std::exception>([&] { service.removeMovie(-1); }));
    assert(throwsException<std::exception>([&] { service.removeMovie(99); }));
    assert(service.getMoviesCount() == 2);
}

static void testServiceValidation() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    assert(throwsException<MovieException>([&] { service.addMovie("A", "Drama", 2000, 1, TRAILER); }));
    assert(throwsException<MovieException>([&] { service.addMovie("Valid", "Invalid", 2000, 1, TRAILER); }));
    assert(throwsException<MovieException>([&] { service.addMovie("Valid", "Drama", 1000, 1, TRAILER); }));
    assert(throwsException<MovieException>([&] { service.addMovie("Valid", "Drama", 2000, -1, TRAILER); }));
    assert(throwsException<MovieException>([&] { service.addMovie("Valid", "Drama", 2000, 1, "bad"); }));
    assert(service.getMoviesCount() == 0);

    addTo(service, "Valid", 2000);
    assert(throwsException<MovieException>([&] { service.updateMovie(0, "V", "Drama", 2000, 1, TRAILER); }));
    assert(service.getMovieByPosition(0).getTitle() == "Valid");
}


static void testServiceFilter() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Old Drama",  1980, 1, "Drama");
    addTo(service, "New Drama",  2020, 1, "Drama");
    addTo(service, "Old Action", 1990, 1, "Action");
    addTo(service, "New Action", 2022, 1, "Action");

    assert(service.filterMovies("", -1, YearDirection::BEFORE).size() == 4);

    auto dramas = service.filterMovies("Drama", -1, YearDirection::BEFORE);
    assert(dramas.size() == 2);

    assert(service.filterMovies("", 2000, YearDirection::BEFORE).size() == 2);
    assert(service.filterMovies("", 2000, YearDirection::AFTER).size() == 2);

    assert(service.filterMovies("", 1980, YearDirection::BEFORE).empty());
    assert(service.filterMovies("", 1980, YearDirection::AFTER).size() == 3);

    auto result = service.filterMovies("Action", 2000, YearDirection::AFTER);
    assert(result.size() == 1);
    assert(result[0].getTitle() == "New Action");

    assert(service.filterMovies("Horror", -1, YearDirection::BEFORE).empty());
    assert(service.filterMovies("Drama", 1900, YearDirection::BEFORE).empty());
}


static void testServiceWatchlist() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Alpha", 2001);
    addTo(service, "Beta",  2002);
    addTo(service, "Gamma", 2003);

    assert(service.getWatchlist().empty());

    service.addMovieToWatchlist(0);
    service.addMovieToWatchlist(2);
    assert(service.getWatchlist().size() == 2);
    assert(service.getWatchlist()[0].getTitle() == "Alpha");
    assert(service.getWatchlist()[1].getTitle() == "Gamma");
    assert(service.getMoviesCount() == 3);

    assert(throwsException<std::exception>([&] { service.addMovieToWatchlist(-1); }));
    assert(throwsException<std::exception>([&] { service.addMovieToWatchlist(99); }));
    assert(service.getWatchlist().size() == 2);

    assert(throwsException<std::exception>([&] { service.addMovieToWatchlist(0); }));
    assert(service.getWatchlist().size() == 2);

    service.removeMovieFromWatchlist(0);
    assert(service.getWatchlist().size() == 1);
    assert(service.getWatchlist()[0].getTitle() == "Gamma");
    assert(service.getMoviesCount() == 3);

    assert(throwsException<std::exception>([&] { service.removeMovieFromWatchlist(99); }));

    service.undo();
    assert(service.getMoviesCount() == 2);
    assert(service.getWatchlist().size() == 1);
}


static void testUndoRedoEmpty() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    assert(throwsException<std::runtime_error>([&] { service.undo(); }));
    assert(throwsException<std::runtime_error>([&] { service.redo(); }));
}

static void testUndoRedoAdd() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Alpha", 2001);
    addTo(service, "Beta",  2002);
    assert(service.getMoviesCount() == 2);

    service.undo();
    assert(service.getMoviesCount() == 1);
    assert(service.getMovieByPosition(0).getTitle() == "Alpha");

    service.undo();
    assert(service.getMoviesCount() == 0);

    assert(throwsException<std::runtime_error>([&] { service.undo(); }));

    service.redo();
    assert(service.getMoviesCount() == 1);
    assert(service.getMovieByPosition(0).getTitle() == "Alpha");

    service.redo();
    assert(service.getMoviesCount() == 2);
    assert(service.getMovieByPosition(1).getTitle() == "Beta");

    assert(throwsException<std::runtime_error>([&] { service.redo(); }));
}

static void testUndoRedoRemove() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Film A", 2001);
    addTo(service, "Film B", 2002);
    addTo(service, "Film C", 2003);
    addTo(service, "Film D", 2004);

    service.removeMovie(1);
    assert(service.getMoviesCount() == 3);
    assert(service.getMovieByPosition(1).getTitle() == "Film C");

    service.undo();
    assert(service.getMoviesCount() == 4);
    assert(service.getMovieByPosition(0).getTitle() == "Film A");
    assert(service.getMovieByPosition(1).getTitle() == "Film B");
    assert(service.getMovieByPosition(2).getTitle() == "Film C");
    assert(service.getMovieByPosition(3).getTitle() == "Film D");

    service.redo();
    assert(service.getMoviesCount() == 3);
    assert(service.getMovieByPosition(1).getTitle() == "Film C");

    service.undo();
    service.removeMovie(0);
    service.removeMovie(2);
    assert(service.getMoviesCount() == 2);
    service.undo();
    assert(service.getMovieByPosition(2).getTitle() == "Film D");
    service.undo();
    assert(service.getMovieByPosition(0).getTitle() == "Film A");
    assert(service.getMoviesCount() == 4);
}

static void testUndoRedoUpdate() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Film A", 2001, 10);
    addTo(service, "Film B", 2002, 20);
    addTo(service, "Film C", 2003, 30);

    service.updateMovie(1, "B2", "Comedy", 2010, 999, TRAILER);
    assert(service.getMovieByPosition(1).getTitle() == "B2");
    assert(service.getMovieByPosition(1).getGenre() == "Comedy");
    assert(service.getMovieByPosition(1).getYearOfRelease() == 2010);
    assert(service.getMovieByPosition(1).getLikesCount() == 999);

    service.undo();
    assert(service.getMoviesCount() == 3);
    assert(service.getMovieByPosition(0).getTitle() == "Film A");
    assert(service.getMovieByPosition(1).getTitle() == "Film B");
    assert(service.getMovieByPosition(1).getGenre() == "Drama");
    assert(service.getMovieByPosition(1).getYearOfRelease() == 2002);
    assert(service.getMovieByPosition(1).getLikesCount() == 20);
    assert(service.getMovieByPosition(2).getTitle() == "Film C");

    service.redo();
    assert(service.getMovieByPosition(1).getTitle() == "B2");
    assert(service.getMovieByPosition(1).getLikesCount() == 999);
    assert(service.getMovieByPosition(0).getTitle() == "Film A");
    assert(service.getMovieByPosition(2).getTitle() == "Film C");
}

static void testUndoRedoMixedSequence() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Film A", 2001);
    addTo(service, "Film B", 2002);
    service.updateMovie(0, "A2", "Drama", 2001, 5, TRAILER);
    service.removeMovie(1);
    assert(service.getMoviesCount() == 1);

    service.undo();
    assert(service.getMoviesCount() == 2);
    service.undo();
    assert(service.getMovieByPosition(0).getTitle() == "Film A");
    service.undo();
    assert(service.getMoviesCount() == 1);
    service.undo();
    assert(service.getMoviesCount() == 0);

    service.redo();
    service.redo();
    service.redo();
    service.redo();
    assert(service.getMoviesCount() == 1);
    assert(service.getMovieByPosition(0).getTitle() == "A2");
}

static void testRedoClearedAfterNewAction() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Film A", 2001);
    addTo(service, "Film B", 2002);
    service.undo();

    addTo(service, "Film C", 2003);
    assert(throwsException<std::runtime_error>([&] { service.redo(); }));
    assert(service.getMoviesCount() == 2);
    assert(service.getMovieByPosition(1).getTitle() == "Film C");

    service.undo();
    service.removeMovie(0);
    assert(throwsException<std::runtime_error>([&] { service.redo(); }));

    service.undo();
    service.undo();
    addTo(service, "Film D", 2004);
    service.updateMovie(0, "D2", "Drama", 2004, 1, TRAILER);
    assert(throwsException<std::runtime_error>([&] { service.redo(); }));
}

static void testFailedActionsDoNotEnterHistory() {
    Repository repo, watchlist;
    Service service(repo, watchlist);

    addTo(service, "Film A", 2001);

    assert(throwsException<std::exception>([&] { service.removeMovie(99); }));
    assert(throwsException<std::exception>([&] { service.updateMovie(99, "Zzz", "Drama", 2000, 1, TRAILER); }));
    assert(throwsException<std::exception>([&] { addTo(service, "Film A", 2001); }));

    service.undo();
    assert(service.getMoviesCount() == 0);
    assert(throwsException<std::runtime_error>([&] { service.undo(); }));
}

void runAllTests() {
    testMovie();
    testValidator();
    testRepository();
    testServiceCrud();
    testServiceValidation();
    testServiceFilter();
    testServiceWatchlist();
    testUndoRedoEmpty();
    testUndoRedoAdd();
    testUndoRedoRemove();
    testUndoRedoUpdate();
    testUndoRedoMixedSequence();
    testRedoClearedAfterNewAction();
    testFailedActionsDoNotEnterHistory();

    std::cout << "All tests passed!\n";
}
