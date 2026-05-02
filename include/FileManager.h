#pragma once
#include "User.h"
#include "Room.h"
#include <cstdio>   // rename() -- works on Windows and POSIX

// =============================================================================
//  Class: FileManager
//  Demonstrates: File handling (fstream), template functions
//  Key fix: ATOMIC writes — write to .tmp then rename, so readers always
//           see a complete consistent file, never a half-written one.
// =============================================================================
class FileManager {
private:
    std::string usersFile;
    std::string roomsFile;

    // Write to a temp file then atomically rename — prevents readers
    // from seeing a half-written file when another terminal is polling.
    static bool atomicWrite(const std::string& target, const std::string& data) {
        std::string tmp = target + ".tmp";
        {
            std::ofstream f(tmp, std::ios::trunc);
            if (!f.is_open()) return false;
            f << data;
            f.flush();          // flush to OS buffer
            f.close();          // close before rename
        }
        // On Windows, remove() the target first since rename() can't overwrite existing files
        remove(target.c_str());
        // rename() is atomic on POSIX — reader always sees old or new, never partial
        return (rename(tmp.c_str(), target.c_str()) == 0);
    }

public:
    explicit FileManager(const std::string& dataDir = "data")
        : usersFile(dataDir + "/users.dat"),
          roomsFile(dataDir + "/rooms.dat") {
        // Touch both files so they exist on first run
        std::ofstream(usersFile, std::ios::app).close();
        std::ofstream(roomsFile, std::ios::app).close();
    }

    // ── Save users (atomic) ───────────────────────────────────────────────
    void saveUsers(const std::vector<User>& users) {
        std::ostringstream oss;
        for (const auto& u : users)
            oss << u.serialize() << "\n";
        atomicWrite(usersFile, oss.str());
    }

    // ── Load users ────────────────────────────────────────────────────────
    std::vector<User> loadUsers() {
        std::vector<User> out;
        std::ifstream f(usersFile);
        if (!f.is_open()) return out;
        std::string line;
        while (std::getline(f, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            User u;
            if (u.deserialize(line)) out.push_back(u);
        }
        return out;
    }

    // ── Save rooms (atomic) ───────────────────────────────────────────────
    // Format per room:
    //   ROOM:<header fields separated by |>
    //   MEMBER:<roomId>|<username>
    //   MSG:<roomId>|<serialised message>
    void saveRooms(const std::vector<Room>& rooms) {
        std::ostringstream oss;
        for (const auto& r : rooms) {
            oss << "ROOM:"   << r.serializeHeader() << "\n";
            for (const auto& m   : r.getMembers())
                oss << "MEMBER:" << r.getRoomId() << "|" << m << "\n";
            for (const auto& msg : r.getMessages())
                oss << "MSG:"    << r.getRoomId() << "|" << msg.serialize() << "\n";
        }
        atomicWrite(roomsFile, oss.str());
    }

    // ── Load rooms ────────────────────────────────────────────────────────
    std::vector<Room> loadRooms() {
        std::vector<Room> rooms;
        std::ifstream f(roomsFile);
        if (!f.is_open()) return rooms;

        std::string line;
        while (std::getline(f, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;

            if (line.size() > 5 && line.substr(0,5) == "ROOM:") {
                Room r;
                if (r.deserializeHeader(line.substr(5)))
                    rooms.push_back(r);

            } else if (line.size() > 7 && line.substr(0,7) == "MEMBER:") {
                std::string rest = line.substr(7);
                size_t sep = rest.find('|');
                if (sep == std::string::npos) continue;
                std::string rid   = rest.substr(0, sep);
                std::string uname = rest.substr(sep + 1);
                for (auto& r : rooms)
                    if (r.getRoomId() == rid) { r.addMember(uname); break; }

            } else if (line.size() > 4 && line.substr(0,4) == "MSG:") {
                std::string rest = line.substr(4);
                size_t sep = rest.find('|');
                if (sep == std::string::npos) continue;
                std::string rid     = rest.substr(0, sep);
                std::string msgLine = rest.substr(sep + 1);
                Message msg;
                if (msg.deserialize(msgLine))
                    for (auto& r : rooms)
                        if (r.getRoomId() == rid) { r.addMessage(msg); break; }
            }
        }
        return rooms;
    }

    // ── Template: write any ISerializable to a file (append) ─────────────
    template<typename T>
    void appendToFile(const std::string& path, const T& item) {
        std::ofstream f(path, std::ios::app);
        f << item.serialize() << "\n";
    }

    // ── Template: load all ISerializable items from a file ────────────────
    template<typename T>
    std::vector<T> readFromFile(const std::string& path) {
        std::vector<T> out;
        std::ifstream f(path);
        if (!f.is_open()) return out;
        std::string line;
        while (std::getline(f, line)) {
            line = Utils::trim(line);
            if (line.empty()) continue;
            T item;
            if (item.deserialize(line)) out.push_back(item);
        }
        return out;
    }

    const std::string& getUsersFile() const { return usersFile; }
    const std::string& getRoomsFile() const { return roomsFile; }
};
