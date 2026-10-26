#pragma once
#include <string>
#include <vector>
#include "domain/domain.h"
#include "repository/IRepository.h"
#include "service/service.h"

class UI {
private:
    Service& service;
    std::string watchlistFile;

    static std::string readLine(const std::string& prompt, bool allowEmpty = false);
    static int readInt(const std::string& prompt);
    static int readCommand(int min, int max);

public:
    explicit UI(Service& service, std::string watchlistFile = "");
    ~UI() = default;

    void run();
    void printAdminMenu();
    void printSelectModeMenu();

    void addMovie();
    void removeMovie();
    void updateMovie();
    void listMovies();
    void undo();
    void redo();
    void generateMovies() const;

    void printUserMenu();
    void printUserBrowseMenu();

    void adminUI(unsigned short& mode);
    void userUI(unsigned short& mode);
    void browseUI(unsigned short& mode);

    void userFilterMovies();
    void userGetWatchlist();
    void removeFromWatchlist();
    void openWatchlistFile();

    static std::unique_ptr<IRepository> chooseWatchlistRepo(std::string&);
};
