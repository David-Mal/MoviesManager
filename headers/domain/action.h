#pragma once
#include "domain.h"
#include "repository/IRepository.h"

class Action {
public:
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual ~Action() = default;
};

class ActionAdd : public Action {
private:
    Movie addedMovie;
    IRepository& repo;

public:
    ActionAdd(const Movie& movie, IRepository& repository)
        : addedMovie(movie), repo(repository) {}

    void undo() override {
        int index = repo.findMovie(addedMovie);
        repo.removeMovieByPosition(index);
    }

    void redo() override {
        repo.addMovie(addedMovie);
    }
};

class ActionRemove : public Action {
private:
    Movie removedMovie;
    IRepository& repo;
    int position;

public:
    ActionRemove(const Movie& movie, IRepository& repository, int position)
        : removedMovie(movie), repo(repository), position(position) {}

    void undo() override { repo.insertMovieAtPosition(position, removedMovie); }
    void redo() override { repo.removeMovieByPosition(position); }
};

class ActionUpdate : public Action {
private:
    Movie oldMovie;
    Movie updatedMovie;
    IRepository& repo;
    int position;

public:
    ActionUpdate(const Movie& oldMovie, const Movie& updatedMovie, IRepository& repository, int position)
        : oldMovie(oldMovie), updatedMovie(updatedMovie), repo(repository), position(position) {}

    void undo() override { repo.updateMovie(position, oldMovie); }
    void redo() override { repo.updateMovie(position, updatedMovie); }
};
