// =============================================================================
//  weChat -- Let's make Friends
//  Terminal-based C++ OOP Chat Application
//  Cross-platform: Windows (MSVC / MinGW / g++) | Linux | macOS
//
//  OOP Concepts Covered:
//   [OK] Classes & Objects
//   [OK] Array of Objects          -- vector<User>, vector<Room>, vector<Message>
//   [OK] Object References         -- AppState&, User&, Room&
//   [OK] Memory Allocation         -- new AppState() / delete in singleton
//   [OK] Default Constructor       -- all classes
//   [OK] Parameterised Constructor -- all classes
//   [OK] Copy Constructor          -- Message, User, Room, Person hierarchy
//   [OK] Destructors               -- all classes; virtual in hierarchy
//   [OK] Function Overloading      -- findRoom(str)/findRoom(int), sendMessage overloads, showMenu overloads
//   [OK] Operator Overloading      -- ==, <, +, +=, <<, >> (cin/cout)
//   [OK] Type Conversion           -- explicit operator string() in User, Room, Message
//   [OK] Static Members            -- totalMessages, totalRooms, totalAccounts, onlineCount, instance
//   [OK] friend Functions          -- operator<< / operator>> in Message, User, Person, Room
//   [OK] cin / cout overloading    -- operator>> (Message), operator<< (Message, User, Person, Room)
//   [OK] Single Inheritance        -- Contact <- Person,  AccountHolder <- Person
//   [OK] Multiple Inheritance      -- User <- Contact + AccountHolder
//   [OK] Multilevel Inheritance    -- Admin <- User <- Contact/AccountHolder <- Person
//   [OK] Hierarchical Inheritance  -- Room <- BaseRoom,  AnnouncementRoom <- BaseRoom
//   [OK] Virtual Base Class        -- virtual Person in Contact & AccountHolder (diamond fix)
//   [OK] Compile-time Polymorphism -- function overloading, operator overloading
//   [OK] Runtime Polymorphism      -- virtual displayInfo(), getRole(), getType()
//   [OK] Pointer to Derived Class  -- BaseRoom* br = room; state.displayRoomInfo(br)
//   [OK] Abstract Base Class       -- Person, BaseRoom, ISerializable (pure virtuals)
//   [OK] Virtual Functions         -- displayInfo(), getRole(), getType(), getMemberCount()
//   [OK] Templates                 -- Payload<T>, FileManager::appendToFile<T>, readFromFile<T>
//   [OK] File Handling             -- fstream + atomic rename in FileManager
// =============================================================================

#include "include/UI.h"

int main() {
    Utils::enableAnsiColors();   // enable ANSI on Windows Terminal / modern CMD
    Utils::ensureDir("data");    // create data/ directory if it doesn't exist

    // Singleton: one AppState allocated with new inside getInstance()
    AppState& app = AppState::getInstance();
    UI ui(app);

    bool running = true;
    while (running) {
        if (!app.isLoggedIn()) {
            // ── Auth screen ────────────────────────────────────────────────
            ui.showWelcome();
            std::string line;
            std::getline(std::cin, line);
            int ch = -1;
            try { ch = std::stoi(Utils::trim(line)); } catch (...) {}

            switch (ch) {
                case 1: ui.showLogin();    break;
                case 2: ui.showRegister(); break;
                case 3: running = false;   break;
                default: break;
            }
        } else {
            // ── Main menu ──────────────────────────────────────────────────
            ui.showMenu();
            std::string line;
            std::getline(std::cin, line);
            int ch = -1;
            try { ch = std::stoi(Utils::trim(line)); } catch (...) {}

            switch (ch) {
                case 1: {
                    std::string rid = ui.showCreateRoom();   // returns stable roomId
                    if (!rid.empty()) ui.showChatRoom(rid);
                    break;
                }
                case 2: {
                    std::string rid = ui.showJoinRoom();
                    if (!rid.empty()) ui.showChatRoom(rid);
                    break;
                }
                case 3: {
                    std::string rid = ui.showMyRooms();
                    if (!rid.empty()) ui.showChatRoom(rid);
                    break;
                }
                case 4: ui.showProfile(); break;
                case 5: ui.showStats();   break;
                case 6:
                    app.logout();
                    std::cout << GREEN << "\n  [OK] Logged out.\n" << RESET;
                    Utils::pressEnter();
                    break;
                default: break;
            }
        }
    }

    // Destructor saves all data, then frees the singleton
    AppState::destroyInstance();
    Utils::clearScreen();
    std::cout << CYAN << "\n  Thanks for using weChat. Goodbye!\n\n" << RESET;
    return 0;
}
