
#include "include/UI.h"

int main()
{
    Utils::enableAnsiColors(); // enable ANSI on Windows Terminal / modern CMD
    Utils::ensureDir("data");  // create data/ directory if it doesn't exist

    // Singleton: one AppState allocated with new inside getInstance()
    AppState &app = AppState::getInstance();
    UI ui(app);

    bool running = true;
    while (running)
    {
        if (!app.isLoggedIn())
        {
            // ── Auth screen ────────────────────────────────────────────────
            ui.showWelcome();
            std::string line;
            std::getline(std::cin, line);
            int ch = -1;
            try
            {
                ch = std::stoi(Utils::trim(line));
            }
            catch (...)
            {
            }

            switch (ch)
            {
            case 1:
                ui.showLogin();
                break;
            case 2:
                ui.showRegister();
                break;
            case 3:
                running = false;
                break;
            default:
                break;
            }
        }
        else
        {
            // ── Main menu ──────────────────────────────────────────────────
            ui.showMenu();
            std::string line;
            std::getline(std::cin, line);
            int ch = -1;
            try
            {
                ch = std::stoi(Utils::trim(line));
            }
            catch (...)
            {
            }

            switch (ch)
            {
            case 1:
            {
                std::string rid = ui.showCreateRoom(); // returns stable roomId
                if (!rid.empty())
                    ui.showChatRoom(rid);
                break;
            }
            case 2:
            {
                std::string rid = ui.showJoinRoom();
                if (!rid.empty())
                    ui.showChatRoom(rid);
                break;
            }
            case 3:
            {
                std::string rid = ui.showMyRooms();
                if (!rid.empty())
                    ui.showChatRoom(rid);
                break;
            }
            case 4:
                ui.showProfile();
                break;
            case 5:
                ui.showStats();
                break;
            case 6:
                app.logout();
                std::cout << GREEN << "\n  [OK] Logged out.\n"
                          << RESET;
                Utils::pressEnter();
                break;
            default:
                break;
            }
        }
    }

    // Destructor saves all data, then frees the singleton
    AppState::destroyInstance();
    Utils::clearScreen();
    std::cout << CYAN << "\n  Thanks for using weChat. Goodbye!\n\n"
              << RESET;
    return 0;
}
