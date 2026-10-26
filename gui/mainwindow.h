#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <functional>
#include <vector>
#include "service/service.h"
#include "repository/repository_factory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Service &service, WatchlistRepoType watchlistType, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_addButton_clicked();
    void on_updateButton_clicked();
    void on_removeButton_clicked();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_generateButton_clicked();
    void on_switchToUserButton_clicked();
    void on_filterButton_clicked();
    void on_clearFilterButton_clicked();
    void on_addToWatchlistButton_clicked();
    void on_removeFromWatchlistButton_clicked();
    void on_openWatchlistButton_clicked();
    void on_switchToAdminButton_clicked();
    void on_moviesTable_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
    Service &service;
    WatchlistRepoType watchlistType;
    std::vector<Movie> userMovies_;

    void run(const std::function<void()> &action);
    void setupTables();
    void setupCombos();
    void refreshAdminMovies();
    void refreshUserMovies();
    void refreshWatchlist();
    void refreshAll();
    void populateTable(QTableWidget *table, const std::vector<Movie> &movies);
    void fillFormFromMovie(const Movie &m);
    void clearForm();
    int selectedAdminRow() const;
    int selectedUserRow() const;
    int selectedWatchlistRow() const;
};
