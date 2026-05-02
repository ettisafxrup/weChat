#pragma once
#include "AppState.h"

// =============================================================================
//  Class: UI  — all terminal screens + live chat loop
//  Demonstrates: Function overloading (showMenu, sendMessage variants)
// =============================================================================
class UI {
private:
    AppState& state;

    // ── Input helpers ──────────────────────────────────────────────────────

    std::string readLine(const std::string& prompt) {
        std::cout << YELLOW << "  " << prompt << RESET;
        std::cout.flush();
        std::string v;
        std::getline(std::cin, v);
        return Utils::trim(v);
    }

    std::string readPassword(const std::string& prompt) {
        std::cout << YELLOW << "  " << prompt << RESET;
        std::cout.flush();
        std::string v;
        std::getline(std::cin, v);
        return v;   // do NOT trim — password spaces are intentional
    }

    // ── Draw helpers ───────────────────────────────────────────────────────

    void drawBanner() {
        std::cout << CYAN
            << "  +----------------------------------------------------------+\n"
            << "  |" << RESET << BOLD
            << "    __        __   ___ ____ _   _    _  _____               " << RESET << CYAN << "|\n"
            << "  |" << RESET << BOLD
            << "    \\ \\      / /__/ __|  __| | | |  / \\|_   _|             " << RESET << CYAN << "|\n"
            << "  |" << RESET << BOLD
            << "     \\ \\ /\\ / / _ \\ |_| |__ | |_| / _ \\ | |               " << RESET << CYAN << "|\n"
            << "  |" << RESET << BOLD
            << "      \\ V  V /  __/  _|  __||  _|/ ___ \\| |               " << RESET << CYAN << "|\n"
            << "  |" << RESET << BOLD
            << "       \\_/\\_/ \\___|_| |____|_| /_/   \\_\\_|               " << RESET << CYAN << "|\n"
            << "  |" << RESET
            << "       " << GREEN << "Let's make Friends  --  C++ OOP Terminal Chat"
            << RESET << CYAN << "       |\n"
            << "  +----------------------------------------------------------+\n"
            << RESET;
    }

    // Full room screen — messages + menu bar.
    // Called only when a redraw is actually needed.
    void drawChatScreen(const Room* room) {
        Utils::clearScreen();

        // Header
        std::cout << CYAN
                  << "=============================================================\n"
                  << RESET
                  << BOLD << "  " << room->getRoomName() << RESET
                  << CYAN << "  [Code: " << YELLOW << room->getJoinCode() << CYAN << "]"
                  << "  Members: " << room->getMemberCount() << "\n"
                  << CYAN
                  << "=============================================================\n"
                  << RESET;

        // Messages (last 25)
        const auto& msgs = room->getMessages();
        int total = (int)msgs.size();
        int start = std::max(0, total - 25);
        if (msgs.empty()) {
            std::cout << YELLOW << "\n  No messages yet — say hello!\n" << RESET;
        } else {
            std::cout << "\n";
            for (int i = start; i < total; ++i)
                std::cout << "  " << msgs[i] << "\n";   // Message::operator<<
        }

        // Footer menu
        std::cout << "\n" << CYAN
                  << "-------------------------------------------------------------\n"
                  << RESET
                  << WHITE << "  [1]" << RESET << " Send message  "
                  << WHITE << "[2]" << RESET << " Members  "
                  << WHITE << "[3]" << RESET << " Room info  "
                  << WHITE << "[0]" << RESET << " Leave\n"
                  << CYAN
                  << "-------------------------------------------------------------\n"
                  << RESET;
    }

    // Print the input prompt line
    void showPrompt() {
        std::cout << YELLOW << "  Choice > " << RESET;
        std::cout.flush();
    }

public:
    explicit UI(AppState& s) : state(s) {}

    // =========================================================================
    //  WELCOME SCREEN
    // =========================================================================
    void showWelcome() {
        Utils::clearScreen();
        std::cout << "\n";
        drawBanner();
        std::cout << "\n"
                  << "    " << WHITE << "[1]" << RESET << " Login\n"
                  << "    " << WHITE << "[2]" << RESET << " Register\n"
                  << "    " << WHITE << "[3]" << RESET << " Exit\n\n"
                  << YELLOW << "  Choice > " << RESET;
        std::cout.flush();
    }

    // =========================================================================
    //  LOGIN
    // =========================================================================
    bool showLogin() {
        Utils::printHeader("LOGIN");
        std::string uname = readLine("Username  : ");
        std::string pass  = readPassword("Password  : ");
        std::cout << "\n";

        if (state.login(uname, pass)) {
            std::cout << GREEN << "  [OK] Welcome back, " << BOLD << uname
                      << RESET << GREEN << "!\n" << RESET;
            return true;  // go straight to dashboard — no pressEnter here
        }
        std::cout << RED << "  [ERR] Invalid username or password.\n" << RESET;
        Utils::pressEnter();
        return false;
    }

    // =========================================================================
    //  REGISTER
    // =========================================================================
    bool showRegister() {
        Utils::printHeader("CREATE ACCOUNT");
        std::string name  = readLine("Full Name  : ");
        std::string email = readLine("Email      : ");
        std::string phone = readLine("Phone      : ");
        std::string uname = readLine("Username   : ");
        std::string pass  = readPassword("Password   : ");
        std::string pass2 = readPassword("Confirm pw : ");
        std::cout << "\n";

        if (name.empty() || uname.empty() || pass.empty()) {
            std::cout << RED << "  [ERR] Name, username and password are required.\n" << RESET;
            Utils::pressEnter(); return false;
        }
        if (pass != pass2) {
            std::cout << RED << "  [ERR] Passwords do not match.\n" << RESET;
            Utils::pressEnter(); return false;
        }
        if (state.registerUser(name, email, phone, uname, pass)) {
            std::cout << GREEN << "  [OK] Account created! You can now log in.\n" << RESET;
            return true;  // go back to welcome — no pressEnter
        }
        std::cout << RED << "  [ERR] Username '" << uname << "' is already taken.\n" << RESET;
        Utils::pressEnter(); return false;
    }

    // =========================================================================
    //  DASHBOARD   (function overloading: with and without title arg)
    // =========================================================================
    void showMenu()                        { showMenu("DASHBOARD"); }
    void showMenu(const std::string& title) {
        state.reloadAllRooms();   // sync rooms + users from disk
        Utils::printHeader(title);

        User* u = state.getCurrentUser();
        if (!u) return;

        // Polymorphism demo: access via Person* base pointer
        Person* p = u;
        std::cout << GREEN  << "  Logged in as : " << CYAN << BOLD
                  << p->getName() << RESET << " (" << u->getUsername() << ")\n"
                  << GREEN  << "  Role         : " << RESET << u->getRole() << "\n"
                  << GREEN  << "  Rooms joined : " << RESET << u->getRoomCount() << "\n"
                  << GREEN  << "  Users online : " << RESET << User::getOnlineCount() << "\n"
                  << GREEN  << "  Total rooms  : " << RESET << Room::getTotalRooms()  << "\n\n";
        Utils::printLine();
        std::cout
            << "  " << WHITE << "[1]" << RESET << " Create a Room\n"
            << "  " << WHITE << "[2]" << RESET << " Join a Room by code\n"
            << "  " << WHITE << "[3]" << RESET << " My Rooms\n"
            << "  " << WHITE << "[4]" << RESET << " View Profile\n"
            << "  " << WHITE << "[5]" << RESET << " Statistics\n"
            << "  " << WHITE << "[6]" << RESET << " Logout\n\n"
            << YELLOW << "  Choice > " << RESET;
        std::cout.flush();
    }

    // =========================================================================
    //  CREATE ROOM
    // =========================================================================
    // Returns roomId string (stable across vector rebuilds)
    std::string showCreateRoom() {
        Utils::printHeader("CREATE ROOM");
        std::string name = readLine("Room name    : ");
        if (name.empty()) {
            std::cout << RED << "  [ERR] Room name cannot be empty.\n" << RESET;
            Utils::pressEnter(); return "";
        }
        std::string privStr = readLine("Private? (y/n): ");
        bool priv = (!privStr.empty() && (privStr[0]=='y' || privStr[0]=='Y'));

        Room* r = state.createRoom(name, priv);
        if (!r) {
            std::cout << RED << "  [ERR] Failed to create room.\n" << RESET;
            Utils::pressEnter(); return "";
        }

        std::string rid = r->getRoomId();
        std::cout << GREEN << "\n  [OK] Room created!\n\n" << RESET
                  << YELLOW
                  << "  +----------------------------------------------+\n"
                  << "  |   Share this code so friends can join:       |\n"
                  << "  |                                              |\n"
                  << "  |      " << BOLD << CYAN << std::setw(6) << r->getJoinCode()
                  << RESET << YELLOW << "                                    |\n"
                  << "  |                                              |\n"
                  << "  +----------------------------------------------+\n\n" << RESET
                  << GREEN << "  Entering room now...\n" << RESET;
        return rid;
    }

    // =========================================================================
    //  JOIN ROOM
    // =========================================================================
    std::string showJoinRoom() {
        Utils::printHeader("JOIN A ROOM");
        std::string code = readLine("Enter room code : ");
        if (code.empty()) { Utils::pressEnter(); return ""; }

        // Uppercase the code so it's case-insensitive
        std::transform(code.begin(), code.end(), code.begin(), ::toupper);

        Room* r = state.joinRoom(code);
        if (r) {
            std::string rid = r->getRoomId();
            std::cout << GREEN << "\n  [OK] Joined: " << BOLD
                      << r->getRoomName() << RESET << "\n"
                      << GREEN << "  Entering room now...\n" << RESET;
            return rid;
        }
        std::cout << RED << "\n  [ERR] No room found with code '" << code
                  << "'. Double-check and try again.\n" << RESET;
        Utils::pressEnter(); return "";
    }

    // =========================================================================
    //  MY ROOMS LIST
    // =========================================================================
    std::string showMyRooms() {
        Utils::printHeader("MY ROOMS");
        auto myRooms = state.getMyRooms();   // vector of Room* into stable rooms vector
        if (myRooms.empty()) {
            std::cout << YELLOW
                      << "  You haven't joined any rooms yet.\n"
                      << "  Create one or ask a friend for their room code!\n" << RESET;
            Utils::pressEnter(); return "";
        }
        std::cout << "\n";
        for (int i = 0; i < (int)myRooms.size(); ++i) {
            // Uses Room::operator<< (friend function)
            std::cout << "  " << WHITE << "[" << (i+1) << "]" << RESET
                      << " " << *myRooms[i] << "\n";
        }
        std::cout << "\n  " << WHITE << "[0]" << RESET << " Back to menu\n\n"
                  << YELLOW << "  Choice > " << RESET;
        std::cout.flush();

        std::string line;
        std::getline(std::cin, line);
        int ch = -1;
        try { ch = std::stoi(line); } catch (...) {}

        if (ch <= 0 || ch > (int)myRooms.size()) return "";
        return myRooms[ch - 1]->getRoomId();   // return stable ID, not pointer
    }

    // =========================================================================
    //  CHAT ROOM — live polling loop
    //
    //  Design:
    //    * Redraw only when something actually changed (new message/member)
    //      or the user just performed an action — NO pointless screen flicker.
    //    * Poll stdin in 500 ms slices (up to 4 = 2 s total between redraws).
    //    * On each slice: check if new data arrived on disk.
    //    * Input is read only when the user presses Enter — non-destructive.
    // =========================================================================
    void showChatRoom(const std::string& roomId) {
        if (roomId.empty()) return;

        bool inRoom     = true;
        bool needRedraw = true;   // draw immediately on entry

        while (inRoom) {
            // Always re-acquire the pointer — safe after any in-place reload
            Room* room = state.findRoom(roomId);
            if (!room) {
                std::cout << RED << "\n  [ERR] Room no longer exists.\n" << RESET;
                Utils::pressEnter(); break;
            }

            if (needRedraw) {
                drawChatScreen(room);
                showPrompt();
                needRedraw = false;
            }

            // ── Poll stdin in 500 ms slices ──────────────────────────────
            // Each slice: check for new file data → if changed, redraw.
            // If input arrives, break out to handle it.
            bool inputReady = false;
            for (int slice = 0; slice < 4 && !inputReady; ++slice) {
                if (Utils::stdinReady(500)) {
                    inputReady = true;
                } else {
                    // Check for new messages/members from other terminals
                    if (state.reloadRoom(roomId)) {
                        needRedraw = true;
                        break;   // break inner loop → outer loop will redraw
                    }
                }
            }

            if (needRedraw) continue;   // go to top — redraw, then poll again
            if (!inputReady)  continue;  // 2 s passed, nothing new — keep waiting

            // ── User pressed Enter: read their choice ────────────────────
            std::string line;
            std::getline(std::cin, line);
            line = Utils::trim(line);
            int ch = -1;
            try { ch = std::stoi(line); } catch (...) {}

            switch (ch) {
                // ── [1] Send message ─────────────────────────────────────
                case 1: {
                    std::cout << YELLOW << "\n  Your message (blank = cancel):\n"
                              << "  >> " << RESET;
                    std::cout.flush();
                    std::string msg;
                    std::getline(std::cin, msg);
                    msg = Utils::trim(msg);
                    if (!msg.empty()) {
                        if (!state.sendMessage(roomId, msg)) {
                            std::cout << RED << "  [ERR] Could not send — are you still a member?\n" << RESET;
                        }
                    }
                    needRedraw = true;
                    break;
                }

                // ── [2] Members list ─────────────────────────────────────
                case 2: {
                    state.reloadRoom(roomId);
                    room = state.findRoom(roomId);
                    if (!room) break;
                    Utils::printHeader("MEMBERS  --  " + room->getRoomName());
                    const auto& members = room->getMembers();
                    std::cout << CYAN << "  " << members.size()
                              << " member(s):\n\n" << RESET;
                    for (const auto& m : members)
                        std::cout << "    * " << CYAN << m << RESET << "\n";
                    Utils::pressEnter();
                    needRedraw = true;
                    break;
                }

                // ── [3] Room info (polymorphic dispatch) ──────────────────
                case 3: {
                    state.reloadRoom(roomId);
                    room = state.findRoom(roomId);
                    if (!room) break;
                    Utils::printHeader("ROOM INFO");
                    // Pointer-to-derived-class stored as BaseRoom* → virtual dispatch
                    BaseRoom* br = room;
                    state.displayRoomInfo(br);
                    Utils::pressEnter();
                    needRedraw = true;
                    break;
                }

                // ── [0] Leave ────────────────────────────────────────────
                case 0:
                    inRoom = false;
                    break;

                default:
                    // Unknown input — just redraw with a hint
                    std::cout << RED << "  Enter 0, 1, 2, or 3.\n" << RESET;
                    needRedraw = true;
                    break;
            }
        }
        Utils::clearScreen();
    }

    // =========================================================================
    //  PROFILE
    // =========================================================================
    void showProfile() {
        Utils::printHeader("MY PROFILE");
        User* u = state.getCurrentUser();
        if (!u) return;
        // Runtime polymorphism: call via Person* base pointer
        Person* p = u;
        p->displayInfo();
        std::cout << "\n" << CYAN << "  Messages sent (session): "
                  << RESET << Message::getTotalMessages() << "\n";
        Utils::pressEnter();
    }

    // =========================================================================
    //  STATISTICS
    // =========================================================================
    void showStats() {
        state.reloadAllRooms();
        Utils::printHeader("STATISTICS");
        std::cout
            << CYAN << "  Registered users  : " << RESET << AccountHolder::getTotalAccounts() << "\n"
            << CYAN << "  Users online now  : " << RESET << User::getOnlineCount()             << "\n"
            << CYAN << "  Total chat rooms  : " << RESET << Room::getTotalRooms()              << "\n"
            << CYAN << "  My rooms joined   : " << RESET << (state.getCurrentUser()
                                                             ? state.getCurrentUser()->getRoomCount() : 0) << "\n"
            << CYAN << "  Messages (session): " << RESET << Message::getTotalMessages()        << "\n";
        Utils::pressEnter();
    }
};
