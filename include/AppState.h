#pragma once
#include "User.h"
#include "Room.h"
#include "FileManager.h"

// =============================================================================
//  Class: AppState  (Singleton)
//  Demonstrates: Memory allocation (new/delete), pointer to derived class,
//                runtime polymorphism (BaseRoom*), static members,
//                function overloading (findRoom, sendMessage)
// =============================================================================
class AppState {
private:
    std::vector<User>  users;       // array of objects
    std::vector<Room>  rooms;       // array of objects
    std::string        currentUsername; // stable identity (no dangling pointer risk)
    FileManager        fm;
    static AppState*   instance;    // static member — singleton pointer

    // Private constructor (singleton pattern)
    AppState() : fm("data") {
        users = fm.loadUsers();
        rooms = fm.loadRooms();
    }

    // Re-anchor currentUser pointer after a users-vector rebuild
    User* findCurrentUser() {
        if (currentUsername.empty()) return nullptr;
        for (auto& u : users)
            if (u.getUsername() == currentUsername) return &u;
        return nullptr;
    }

public:
    // ── Singleton ─────────────────────────────────────────────────────────
    static AppState& getInstance() {
        if (!instance) instance = new AppState(); // explicit memory allocation
        return *instance;
    }
    static void destroyInstance() {
        delete instance;   // explicit deallocation
        instance = nullptr;
    }

    AppState(const AppState&)            = delete;
    AppState& operator=(const AppState&) = delete;

    ~AppState() {
        User* u = findCurrentUser();
        if (u) u->logout();
        fm.saveUsers(users);
        fm.saveRooms(rooms);
    }

    // ═════════════════════════════════════════════════════════════════════
    //  USER MANAGEMENT
    // ═════════════════════════════════════════════════════════════════════

    bool registerUser(const std::string& name,  const std::string& email,
                      const std::string& phone, const std::string& username,
                      const std::string& password) {
        // Reload users first so we don't miss a registration from another terminal
        users = fm.loadUsers();
        for (const auto& u : users)
            if (u.getUsername() == username) return false;   // taken
        users.emplace_back(name, email, phone, username, password);
        fm.saveUsers(users);
        return true;
    }

    bool login(const std::string& username, const std::string& password) {
        // Always reload from disk so credentials are fresh
        users = fm.loadUsers();
        for (auto& u : users) {
            if (u.getUsername() == username && u.checkPassword(password)) {
                currentUsername = username;
                u.login();
                fm.saveUsers(users);
                return true;
            }
        }
        return false;
    }

    void logout() {
        User* u = findCurrentUser();
        if (u) { u->logout(); fm.saveUsers(users); }
        currentUsername.clear();
    }

    // Returns pointer into users vector — valid until users is rebuilt
    User* getCurrentUser() { return findCurrentUser(); }
    bool  isLoggedIn()     { return !currentUsername.empty(); }

    // ═════════════════════════════════════════════════════════════════════
    //  ROOM MANAGEMENT
    // ═════════════════════════════════════════════════════════════════════

    Room* createRoom(const std::string& name, bool isPrivate = false) {
        User* u = findCurrentUser();
        if (!u) return nullptr;
        // Reload rooms so we don't clobber another terminal's new rooms
        rooms = fm.loadRooms();
        rooms.emplace_back(name, u->getUsername(), isPrivate);
        rooms.back().addMember(u->getUsername());
        u->incrementRoomCount();
        fm.saveRooms(rooms);
        fm.saveUsers(users);
        // Return pointer to the last element — stable until next rooms rebuild
        return &rooms.back();
    }

    // Join by 6-char code — function overloading variant 1
    Room* joinRoom(const std::string& code) {
        User* u = findCurrentUser();
        if (!u) return nullptr;
        // Reload so we see the room if it was created in another terminal
        rooms = fm.loadRooms();
        for (auto& r : rooms) {
            if (r.getJoinCode() == code) {
                bool wasNew = r.addMember(u->getUsername());
                if (wasNew) {
                    u->incrementRoomCount();
                    fm.saveRooms(rooms);
                    fm.saveUsers(users);
                }
                return &r;
            }
        }
        return nullptr;
    }

    // Find room by roomId string — function overloading variant 2
    Room* findRoom(const std::string& roomId) {
        for (auto& r : rooms)
            if (r.getRoomId() == roomId) return &r;
        return nullptr;
    }

    // Find room by index — function overloading variant 3
    Room* findRoom(int idx) {
        if (idx < 0 || idx >= (int)rooms.size()) return nullptr;
        return &rooms[idx];
    }

    // ═════════════════════════════════════════════════════════════════════
    //  MESSAGING
    // ═════════════════════════════════════════════════════════════════════

    // Send a message and persist immediately
    bool sendMessage(const std::string& roomId, const std::string& content) {
        User* u = findCurrentUser();
        if (!u) return false;
        Room* r = findRoom(roomId);
        if (!r || !r->hasMember(u->getUsername())) return false;
        Message msg(u->getUsername(), content);
        *r += msg;              // operator+= on Room
        fm.saveRooms(rooms);
        return true;
    }

    // Function overloading: send via Room pointer
    bool sendMessage(Room* room, const std::string& content) {
        if (!room) return false;
        return sendMessage(room->getRoomId(), content);
    }

    // ═════════════════════════════════════════════════════════════════════
    //  LIVE RELOAD  (called from the chat polling loop)
    // ═════════════════════════════════════════════════════════════════════

    // Reload ONE room from disk in-place.
    // Returns true if messages or members changed (caller should redraw).
    // Uses in-place assignment so existing Room* pointers stay valid.
    bool reloadRoom(const std::string& roomId) {
        std::vector<Room> fresh = fm.loadRooms();
        for (const auto& fr : fresh) {
            if (fr.getRoomId() != roomId) continue;
            for (auto& r : rooms) {
                if (r.getRoomId() != roomId) continue;
                bool changed = (fr.getMessages().size() != r.getMessages().size()
                             || fr.getMembers().size()  != r.getMembers().size());
                if (changed) r = fr;   // copy-assign in place — pointer stays valid
                return changed;
            }
            // Room not found in our in-memory list yet — add it
            rooms.push_back(fr);
            return true;
        }
        return false;
    }

    // Reload the complete rooms list (used when returning to main menu).
    // After this, callers must re-acquire any Room* via findRoom().
    void reloadAllRooms() {
        rooms = fm.loadRooms();
        // Also reload user data so roomCount etc. is current
        std::string uname = currentUsername;
        users = fm.loadUsers();
        currentUsername = uname;   // keep login state
    }

    // ═════════════════════════════════════════════════════════════════════
    //  ACCESSORS
    // ═════════════════════════════════════════════════════════════════════

    const std::vector<User>& getUsers() const { return users; }
    const std::vector<Room>& getRooms() const { return rooms; }

    // Returns pointers to rooms the current user is a member of.
    // Valid until next reloadAllRooms() call.
    std::vector<Room*> getMyRooms() {
        std::vector<Room*> result;
        if (currentUsername.empty()) return result;
        for (auto& r : rooms)
            if (r.hasMember(currentUsername))
                result.push_back(&r);
        return result;
    }

    // Polymorphic display — demonstrates pointer-to-base-class + virtual dispatch
    void displayRoomInfo(BaseRoom* br) const {
        if (br) br->displayInfo();   // virtual dispatch
    }
};

AppState* AppState::instance = nullptr;
