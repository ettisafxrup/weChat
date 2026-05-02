#pragma once
#include "Person.h"
#include "Message.h"

// ── Class: User ───────────────────────────────────────────────────────────
// Demonstrates: Multiple inheritance (Contact + AccountHolder, both inherit
//   virtual Person → diamond solved), multilevel chain, static members,
//   operator overloading, type conversion, friend functions
class User : public Contact, public AccountHolder {
private:
    bool isOnline;
    int  roomCount;
    static int onlineCount;

public:
    // Default constructor
    User() : Person(), Contact(), AccountHolder(), isOnline(false), roomCount(0) {}

    // Parameterised constructor
    User(const std::string& n, const std::string& e,
         const std::string& ph, const std::string& u, const std::string& pw)
        : Person(n, e),
          Contact(n, e, ph),
          AccountHolder(n, e, u, AccountHolder::hashPassword(pw)),
          isOnline(false), roomCount(0) {}

    // Copy constructor
    User(const User& o)
        : Person(o), Contact(o), AccountHolder(o),
          isOnline(o.isOnline), roomCount(o.roomCount) {}

    // Copy-assignment
    User& operator=(const User& o) {
        if (this != &o) {
            Contact::operator=(o);
            AccountHolder::operator=(o);
            isOnline  = o.isOnline;
            roomCount = o.roomCount;
        }
        return *this;
    }

    virtual ~User() { if (isOnline) --onlineCount; }

    // Online management
    void login()  { if (!isOnline) { isOnline = true;  ++onlineCount; } }
    void logout() { if (isOnline)  { isOnline = false; --onlineCount; } }
    bool getIsOnline()  const { return isOnline; }
    int  getRoomCount() const { return roomCount; }
    void incrementRoomCount() { ++roomCount; }
    void setRoomCount(int c)  { roomCount = c; }

    static int getOnlineCount() { return onlineCount; }

    // Virtual function override (runtime polymorphism)
    void displayInfo() const override {
        std::cout << CYAN << "+-- User Profile " << std::string(44, '-') << "+\n" << RESET;
        std::cout << CYAN << "  Username: " << RESET << username  << "\n"
                  << CYAN << "  Name    : " << RESET << name      << "\n"
                  << CYAN << "  Email   : " << RESET << email     << "\n"
                  << CYAN << "  Phone   : " << RESET << phone     << "\n"
                  << CYAN << "  Joined  : " << RESET << createdAt << "\n"
                  << CYAN << "  Status  : " << RESET
                  << (isOnline ? GREEN "Online" RESET : RED "Offline" RESET) << "\n"
                  << CYAN << "  Rooms   : " << RESET << roomCount << "\n";
        std::cout << CYAN << "+" << std::string(61, '-') << "+\n" << RESET;
    }
    std::string getRole() const override { return "User"; }

    // Operator == — compare by username
    bool operator==(const User& o) const { return username == o.username; }

    // friend: cout overloading
    friend std::ostream& operator<<(std::ostream& os, const User& u) {
        os << CYAN << u.username << RESET
           << (u.isOnline ? GREEN " [online]" RESET : "");
        return os;
    }

    // Type conversion: User -> string
    explicit operator std::string() const { return username; }

    // Serialization: name|email|phone|username|passwordHash|createdAt|roomCount
    std::string serialize() const {
        return name + "|" + email + "|" + phone + "|" +
               username + "|" + passwordHash + "|" + createdAt + "|" +
               std::to_string(roomCount);
    }

    bool deserialize(const std::string& line) {
        std::istringstream ss(line);
        std::vector<std::string> p;
        std::string t;
        while (std::getline(ss, t, '|')) p.push_back(t);
        if (p.size() < 6) return false;
        name         = p[0];
        email        = p[1];
        phone        = p[2];
        username     = p[3];
        passwordHash = p[4];
        createdAt    = p[5];
        roomCount    = (p.size() >= 7) ? std::stoi(p[6]) : 0;
        return !username.empty();
    }
};

int User::onlineCount = 0;

// ── Class: Admin ─────────────────────────────────────────────────────────
// Demonstrates: Multilevel inheritance (Admin <- User <- Contact/AccountHolder <- Person)
//               Hierarchical inheritance (Admin and User both descend from Person)
class Admin : public User {
private:
    std::string adminLevel;
public:
    Admin() : Person(), User(), adminLevel("standard") {}

    Admin(const std::string& n, const std::string& e, const std::string& ph,
          const std::string& u, const std::string& pw,
          const std::string& level = "standard")
        : Person(n, e), User(n, e, ph, u, pw), adminLevel(level) {}

    Admin(const Admin& o) : Person(o), User(o), adminLevel(o.adminLevel) {}

    Admin& operator=(const Admin& o) {
        if (this != &o) { User::operator=(o); adminLevel = o.adminLevel; }
        return *this;
    }
    ~Admin() override {}

    std::string getAdminLevel() const { return adminLevel; }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << MAGENTA << "  Admin Level: " << RESET << adminLevel << "\n";
    }
    std::string getRole() const override { return "Admin"; }
};
