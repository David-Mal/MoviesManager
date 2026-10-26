#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QSpinBox>

#include "domain/MovieValidator.h"
#include "exceptions/exceptions.h"

static const QStringList kMovieHeaders = {"Title", "Genre", "Year", "Likes", "Trailer"};

MainWindow::MainWindow(Service &service, WatchlistRepoType watchlistType, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , service(service)
    , watchlistType(watchlistType)
{
    ui->setupUi(this);
    setWindowTitle("Movies Manager");
    setupTables();
    setupCombos();
    refreshAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─── helpers ──────────────────────────────────────────────────────────────────

void MainWindow::setupTables()
{
    auto configure = [](QTableWidget *t) {
        t->setColumnCount(5);
        t->setHorizontalHeaderLabels(kMovieHeaders);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        t->setSelectionBehavior(QAbstractItemView::SelectRows);
        t->setSelectionMode(QAbstractItemView::SingleSelection);
        t->horizontalHeader()->setStretchLastSection(true);
        t->verticalHeader()->setVisible(false);
    };
    configure(ui->moviesTable);
    configure(ui->filteredMoviesTable);

    ui->watchlistTable->setColumnCount(5);
    ui->watchlistTable->setHorizontalHeaderLabels(kMovieHeaders);
    ui->watchlistTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->watchlistTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->watchlistTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->watchlistTable->horizontalHeader()->setStretchLastSection(true);
    ui->watchlistTable->verticalHeader()->setVisible(false);
}

void MainWindow::setupCombos()
{
    for (const auto &g : MovieValidator::validGenres()) {
        const QString genre = QString::fromStdString(g);
        ui->genreCombo->addItem(genre);
        ui->filterGenreCombo->addItem(genre);
    }
    ui->filterGenreCombo->insertItem(0, "Any genre");
    ui->filterGenreCombo->setCurrentIndex(0);
}

void MainWindow::populateTable(QTableWidget *table, const std::vector<Movie> &movies)
{
    table->setRowCount(static_cast<int>(movies.size()));
    for (int row = 0; row < static_cast<int>(movies.size()); ++row) {
        const Movie &m = movies[row];
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m.getTitle())));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(m.getGenre())));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(m.getYearOfRelease())));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(m.getLikesCount())));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(m.getTrailer())));
    }
}

void MainWindow::refreshAdminMovies()
{
    populateTable(ui->moviesTable, service.getAllMovies());
}

void MainWindow::refreshUserMovies()
{
    populateTable(ui->filteredMoviesTable, userMovies_);
}

void MainWindow::refreshWatchlist()
{
    populateTable(ui->watchlistTable, service.getWatchlist());
}

void MainWindow::refreshAll()
{
    userMovies_ = service.getAllMovies();
    refreshAdminMovies();
    refreshUserMovies();
    refreshWatchlist();
    ui->statusbar->showMessage(
        QString("Catalog: %1 movies  |  Watchlist: %2 movies")
            .arg(service.getMoviesCount())
            .arg(service.getWatchlist().size()));
}

void MainWindow::run(const std::function<void()> &action)
{
    try {
        action();
        refreshAll();
    } catch (const MovieException &e) {
        QStringList errors;
        for (const auto &err : e.getErrors())
            errors << QString::fromStdString(err.what());
        QMessageBox::warning(this, "Invalid movie data", errors.join("\n"));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", QString::fromStdString(e.what()));
    }
}

void MainWindow::fillFormFromMovie(const Movie &m)
{
    ui->titleEdit->setText(QString::fromStdString(m.getTitle()));
    ui->trailerEdit->setText(QString::fromStdString(m.getTrailer()));
    ui->yearSpin->setValue(m.getYearOfRelease());
    ui->likesSpin->setValue(m.getLikesCount());

    const int idx = ui->genreCombo->findText(QString::fromStdString(m.getGenre()));
    if (idx >= 0)
        ui->genreCombo->setCurrentIndex(idx);
}

void MainWindow::clearForm()
{
    ui->titleEdit->clear();
    ui->trailerEdit->clear();
    ui->yearSpin->setValue(2024);
    ui->likesSpin->setValue(0);
    ui->genreCombo->setCurrentIndex(0);
}

int MainWindow::selectedAdminRow() const
{
    const QList<QTableWidgetItem*> sel = ui->moviesTable->selectedItems();
    return sel.isEmpty() ? -1 : ui->moviesTable->row(sel.first());
}

int MainWindow::selectedUserRow() const
{
    const QList<QTableWidgetItem*> sel = ui->filteredMoviesTable->selectedItems();
    return sel.isEmpty() ? -1 : ui->filteredMoviesTable->row(sel.first());
}

int MainWindow::selectedWatchlistRow() const
{
    const QList<QTableWidgetItem*> sel = ui->watchlistTable->selectedItems();
    return sel.isEmpty() ? -1 : ui->watchlistTable->row(sel.first());
}

// ─── admin slots ──────────────────────────────────────────────────────────────

void MainWindow::on_moviesTable_itemSelectionChanged()
{
    const int row = selectedAdminRow();
    if (row < 0) return;
    try {
        fillFormFromMovie(service.getMovieByPosition(row));
    } catch (...) {}
}

void MainWindow::on_addButton_clicked()
{
    run([this] {
        service.addMovie(
            ui->titleEdit->text().trimmed().toStdString(),
            ui->genreCombo->currentText().toStdString(),
            ui->yearSpin->value(),
            ui->likesSpin->value(),
            ui->trailerEdit->text().trimmed().toStdString());
        clearForm();
    });
}

void MainWindow::on_updateButton_clicked()
{
    const int row = selectedAdminRow();
    if (row < 0) {
        QMessageBox::information(this, "Update", "Select a movie to update.");
        return;
    }

    const Movie current = service.getMovieByPosition(row);
    const auto answer = QMessageBox::question(
        this, "Confirm Update",
        QString("Apply changes to \"%1\"?").arg(QString::fromStdString(current.getTitle())),
        QMessageBox::Yes | QMessageBox::No);
    if (answer != QMessageBox::Yes)
        return;

    run([this, row] {
        service.updateMovie(
            row,
            ui->titleEdit->text().trimmed().toStdString(),
            ui->genreCombo->currentText().toStdString(),
            ui->yearSpin->value(),
            ui->likesSpin->value(),
            ui->trailerEdit->text().trimmed().toStdString());
    });
}

void MainWindow::on_removeButton_clicked()
{
    const int row = selectedAdminRow();
    if (row < 0) {
        QMessageBox::information(this, "Remove", "Select a movie to remove.");
        return;
    }

    const Movie m = service.getMovieByPosition(row);
    const auto answer = QMessageBox::question(
        this, "Confirm Remove",
        QString("Remove \"%1 (%2)\"?")
            .arg(QString::fromStdString(m.getTitle()))
            .arg(m.getYearOfRelease()),
        QMessageBox::Yes | QMessageBox::No);
    if (answer != QMessageBox::Yes)
        return;

    run([this, row] {
        service.removeMovie(row);
        clearForm();
    });
}

void MainWindow::on_undoButton_clicked()
{
    run([this] { service.undo(); });
}

void MainWindow::on_redoButton_clicked()
{
    run([this] { service.redo(); });
}

void MainWindow::on_generateButton_clicked()
{
    run([this] {
        const int n = ui->generateNSpin->value();
        const int added = service.generateNRandomMovies(n);
        ui->statusbar->showMessage(QString("Generated %1 random movie(s).").arg(added));
    });
}

void MainWindow::on_switchToUserButton_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->userTab);
}

// ─── user slots ───────────────────────────────────────────────────────────────

void MainWindow::on_filterButton_clicked()
{
    const QString genreItem = ui->filterGenreCombo->currentText();
    const std::string genre = (genreItem == "Any genre") ? "" : genreItem.toStdString();
    const int year = ui->filterYearSpin->value();
    const int dirIdx = ui->filterDirectionCombo->currentIndex();

    YearDirection dir = YearDirection::ANY;
    if (year > 0) {
        if (dirIdx == 1) dir = YearDirection::BEFORE;
        else if (dirIdx == 2) dir = YearDirection::AFTER;
    }

    try {
        userMovies_ = service.filterMovies(genre, year, dir);
        refreshUserMovies();
        ui->statusbar->showMessage(
            QString("Filter returned %1 movie(s).").arg(userMovies_.size()));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Filter error", e.what());
    }
}

void MainWindow::on_clearFilterButton_clicked()
{
    ui->filterGenreCombo->setCurrentIndex(0);
    ui->filterYearSpin->setValue(0);
    ui->filterDirectionCombo->setCurrentIndex(0);
    userMovies_ = service.getAllMovies();
    refreshUserMovies();
    ui->statusbar->showMessage(QString("Showing all %1 movies.").arg(userMovies_.size()));
}

void MainWindow::on_addToWatchlistButton_clicked()
{
    const int row = selectedUserRow();
    if (row < 0 || row >= static_cast<int>(userMovies_.size())) {
        QMessageBox::information(this, "Watchlist", "Select a movie to add.");
        return;
    }

    const Movie &m = userMovies_[row];
    const int catalogIndex = service.findMovie(
        m.getTitle(), m.getGenre(),
        m.getYearOfRelease(), m.getLikesCount(), m.getTrailer());

    if (catalogIndex < 0) {
        QMessageBox::warning(this, "Watchlist", "Could not locate movie in catalog.");
        return;
    }

    run([this, catalogIndex] {
        service.addMovieToWatchlist(catalogIndex);
    });
}

void MainWindow::on_removeFromWatchlistButton_clicked()
{
    const int row = selectedWatchlistRow();
    if (row < 0) {
        QMessageBox::information(this, "Watchlist", "Select a movie to remove.");
        return;
    }

    const std::vector<Movie> watchlist = service.getWatchlist();
    const QString title = (row < static_cast<int>(watchlist.size()))
        ? QString::fromStdString(watchlist[row].getTitle())
        : QString("this movie");

    const auto answer = QMessageBox::question(
        this, "Confirm Remove",
        QString("Remove \"%1\" from watchlist?").arg(title),
        QMessageBox::Yes | QMessageBox::No);
    if (answer != QMessageBox::Yes)
        return;

    run([this, row] {
        service.removeMovieFromWatchlist(row);
    });
}

void MainWindow::on_openWatchlistButton_clicked()
{
    run([this] {
        Service::openWatchlist(watchlistFilePath(watchlistType));
    });
}

void MainWindow::on_switchToAdminButton_clicked()
{
    ui->tabWidget->setCurrentWidget(ui->adminTab);
}
