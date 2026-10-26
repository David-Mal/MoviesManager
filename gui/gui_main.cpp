#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>

#include "mainwindow.h"
#include "repository/repository_factory.h"
#include "service/service.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ── Startup: choose repository types ─────────────────────────────────────
    QDialog dialog;
    dialog.setWindowTitle("Movies Manager — Setup");
    dialog.setMinimumWidth(300);

    auto *layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel("Choose storage formats:"));

    auto *form = new QFormLayout;

    auto *movieTypeCombo = new QComboBox;
    movieTypeCombo->addItem("Text file  (.txt)",  static_cast<int>(MovieRepoType::Txt));
    movieTypeCombo->addItem("SQLite database (.db)", static_cast<int>(MovieRepoType::Sql));
    form->addRow("Movies:", movieTypeCombo);

    auto *watchlistTypeCombo = new QComboBox;
    watchlistTypeCombo->addItem("CSV file  (.csv)",  static_cast<int>(WatchlistRepoType::Csv));
    watchlistTypeCombo->addItem("HTML file (.html)", static_cast<int>(WatchlistRepoType::Html));
    form->addRow("Watchlist:", watchlistTypeCombo);

    layout->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted)
        return 0;

    const auto movieType     = static_cast<MovieRepoType>(movieTypeCombo->currentData().toInt());
    const auto watchlistType = static_cast<WatchlistRepoType>(watchlistTypeCombo->currentData().toInt());

    // ── Build service from factory repos ─────────────────────────────────────
    std::unique_ptr<IRepository> movieRepo;
    std::unique_ptr<IRepository> watchlistRepo;

    try {
        movieRepo     = makeMovieRepository(movieType);
        watchlistRepo = makeWatchlistRepository(watchlistType);
    } catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Startup Error", e.what());
        return 1;
    }

    Service service(*movieRepo, *watchlistRepo);

    MainWindow window(service, watchlistType);
    window.show();

    return app.exec();
}
