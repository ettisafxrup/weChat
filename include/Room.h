#pragma once
#include "Message.h"
#include "Utils.h"
#include <vector>

// ── Abstract Base Class: BaseRoom ─────────────────────────────────────────
// Demonstrates: Abstract class, pure virtual functions, pointer to derived
class BaseRoom {
protected:
    std::string roomId;
    std::string roomName;
    std::string createdBy;
    std::string createdAt;
public:
    BaseRoom() : roomId(""), roomName(""), createdBy(""), createdAt("") {}

    BaseRoom(const std::string& id, const std::string& nm,
             const std::string& creator)
        : roomId(id), roomName(nm), createdBy(creator),
          createdAt(Utils::getFullTimestamp()) {}

    BaseRoom(const BaseRoom& o)
        : roomId(o.roomId), roomName(o.roomName),
          createdBy(o.createdBy), createdAt(o.createdAt) {}

    BaseRoom& operator=(const BaseRoom& o) {
        if (this != &o) {
            roomId = o.roomId; roomName = o.roomName;
            createdBy = o.createdBy; createdAt = o.createdAt;
        }
        return *this;
    }

    virtual ~BaseRoom() {}

    std::string getRoomId()    const { return roomId; }
    std::string getRoomName()  const { return roomName; }
    std::string getCreatedBy() const { return createdBy; }
    std::string getCreatedAt() const { return createdAt; }
    void setCreatedAt(const std::string& t) { createdAt = t; }

    virtual void        displayInfo()    const = 0;   // pure virtual
    virtual int         getMemberCount() const = 0;   // pure virtual
    virtual std::string getType()        const { return "Base"; }

    bool operator==(const BaseRoom& o) const { return roomId == o.roomId; }

    friend std::ostream& operator<<(std::ostream& os, const BaseRoom& r) {
        os << CYAN << "[" << r.roomId << "] " << RESET
           << BOLD << r.roomName << RESET
           << YELLOW << "  Code:" << r.getType() << RESET // overridden below
           << "  (by " << r.createdBy << ")";
        return os;
    }
};

// ── Class: Room  (concrete — inherits BaseRoom) ───────────────────────────
// Demonstrates: Array of objects (messages/members), constructors, destructor,
//   copy ctor, operator overloading, static members, serialization
class Room : public BaseRoom {
private:
    std::string           joinCode;
    std::vector<std::string> members;   // array of username strings
    std::vector<Message>     messages;  // array of Message objects
    int                   maxMembers;
    bool                  isPrivate;
    static int            totalRooms;

public:
    // Default constructor
    Room() : BaseRoom(), joinCode(Utils::generateCode(6)),
             maxMembers(50), isPrivate(false) { ++totalRooms; }

    // Parameterised constructor
    Room(const std::string& nm, const std::string& creator,
         bool priv = false, int maxM = 50)
        : BaseRoom(Utils::generateCode(8), nm, creator),
          joinCode(Utils::generateCode(6)),
          maxMembers(maxM), isPrivate(priv) { ++totalRooms; }

    // Copy constructor
    Room(const Room& o)
        : BaseRoom(o), joinCode(o.joinCode), members(o.members),
          messages(o.messages), maxMembers(o.maxMembers),
          isPrivate(o.isPrivate) { ++totalRooms; }

    // Copy-assignment
    Room& operator=(const Room& o) {
        if (this != &o) {
            BaseRoom::operator=(o);
            joinCode   = o.joinCode;
            members    = o.members;
            messages   = o.messages;
            maxMembers = o.maxMembers;
            isPrivate  = o.isPrivate;
        }
        return *this;
    }

    // Destructor
    ~Room() override { --totalRooms; }

    // Accessors
    std::string getJoinCode()   const { return joinCode; }
    bool        getIsPrivate()  const { return isPrivate; }
    int         getMaxMembers() const { return maxMembers; }
    void setJoinCode(const std::string& c) { joinCode = c; }

    const std::vector<std::string>& getMembers()  const { return members; }
    const std::vector<Message>&     getMessages() const { return messages; }

    static int getTotalRooms() { return totalRooms; }

    // Member management
    bool addMember(const std::string& u) {
        if ((int)members.size() >= maxMembers) return false;
        for (auto& m : members) if (m == u) return false;
        members.push_back(u);
        return true;
    }
    bool removeMember(const std::string& u) {
        auto it = std::find(members.begin(), members.end(), u);
        if (it == members.end()) return false;
        members.erase(it); return true;
    }
    bool hasMember(const std::string& u) const {
        return std::find(members.begin(), members.end(), u) != members.end();
    }
    int getMemberCount() const override { return (int)members.size(); }

    // Message management — function overloading
    void addMessage(const Message& msg)  { messages.push_back(msg); }
    void addMessage(const std::string& sender, const std::string& content) {
        messages.emplace_back(sender, content);
    }
    void clearMessages() { messages.clear(); }

    // Operator += — add a message
    Room& operator+=(const Message& msg) { addMessage(msg); return *this; }

    // Type conversion: Room -> string
    explicit operator std::string() const { return roomName; }

    // Virtual displayInfo
    void displayInfo() const override {
        std::cout << CYAN << "  Room    : " << RESET << BOLD << roomName << RESET << "\n"
                  << CYAN << "  Code    : " << RESET << YELLOW << BOLD << joinCode << RESET << "\n"
                  << CYAN << "  Creator : " << RESET << createdBy << "\n"
                  << CYAN << "  Created : " << RESET << createdAt << "\n"
                  << CYAN << "  Members : " << RESET << members.size() << "/" << maxMembers << "\n"
                  << CYAN << "  Private : " << RESET << (isPrivate ? "Yes" : "No") << "\n"
                  << CYAN << "  Messages: " << RESET << messages.size() << "\n";
    }

    std::string getType() const override { return joinCode; }  // used in BaseRoom<<

    // friend: cout override for room list display
    friend std::ostream& operator<<(std::ostream& os, const Room& r) {
        os << WHITE << "[" << r.joinCode << "] " << RESET
           << BOLD << r.roomName << RESET
           << CYAN << "  (" << r.members.size() << " members)" << RESET
           << "  by " << r.createdBy;
        return os;
    }

    // Serialization
    std::string serializeHeader() const {
        return roomId + "|" + roomName + "|" + createdBy + "|" +
               createdAt + "|" + joinCode + "|" +
               std::to_string(maxMembers) + "|" +
               std::to_string((int)isPrivate);
    }

    bool deserializeHeader(const std::string& line) {
        std::istringstream ss(line);
        std::string t;
        std::vector<std::string> p;
        while (std::getline(ss, t, '|')) p.push_back(t);
        if (p.size() < 7) return false;
        roomId     = p[0]; roomName = p[1]; createdBy = p[2];
        createdAt  = p[3]; joinCode = p[4];
        maxMembers = std::stoi(p[5]);
        isPrivate  = (bool)std::stoi(p[6]);
        return !roomId.empty();
    }
};

int Room::totalRooms = 0;

// ── Class: AnnouncementRoom  (hierarchical inheritance from BaseRoom) ─────
// Demonstrates: Hierarchical inheritance — Room and AnnouncementRoom both
//   inherit BaseRoom independently
class AnnouncementRoom : public BaseRoom {
private:
    std::vector<std::string> announcements;
    std::string              adminUser;
public:
    AnnouncementRoom() : BaseRoom(), adminUser("") {}
    AnnouncementRoom(const std::string& nm, const std::string& creator)
        : BaseRoom(Utils::generateCode(8), nm, creator), adminUser(creator) {}

    void addAnnouncement(const std::string& msg) {
        announcements.push_back("[" + Utils::getTimestamp() + "] " + msg);
    }

    void displayInfo() const override {
        std::cout << MAGENTA << "  [Announcement Room] " << RESET
                  << roomName << "  by " << createdBy << "\n"
                  << "  Announcements: " << announcements.size() << "\n";
    }

    int         getMemberCount() const override { return 0; }
    std::string getType()        const override { return "AnnouncementRoom"; }

    const std::vector<std::string>& getAnnouncements() const { return announcements; }
};
