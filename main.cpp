#include <iostream>

#include "StartUp.h"
#include "headers/domain/domain.h"
#include "repository/repository.h"
#include "repository/repository_factory.h"
#include "service/service.h"
#include "tests/tests.h"
#include "ui/ui.h"

int main() {

    runAllTests();
    try {
        MovieRepoType movieType = askMovieRepoType();
        WatchlistRepoType watchlistType = askWatchlistRepoType();

        auto movieRepo = makeMovieRepository(movieType);
        auto watchlistRepo = makeWatchlistRepository(watchlistType);

        Service service(*movieRepo, *watchlistRepo);
        UI ui(service, watchlistFilePath(watchlistType));
        ui.run();
    } catch (const std::exception& e) {
        std::cout << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
