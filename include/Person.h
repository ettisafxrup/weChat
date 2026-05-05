#pragma once
#include "Utils.h"

// ── Abstract Base Class: Person ───────────────────────────────────────────
// Demonstrates: Abstract base class, pure virtual functions, virtual destructor
class Person
{
protected:
    std::string name;
    std::string email;

public:
    Person() : name(""), email("") {}
    Person(const std::string &n, const std::string &e) : name(n), email(e) {}
    Person(const Person &o) : name(o.name), email(o.email) {}
    Person &operator=(const Person &o)
    {
        if (this != &o)
        {
            name = o.name;
            email = o.email;
        }
        return *this;
    }
    virtual ~Person() {}

    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    void setName(const std::string &n) { name = n; }
    void setEmail(const std::string &e) { email = e; }

    virtual void displayInfo() const = 0; // pure virtual
    virtual std::string getRole() const { return "Person"; }

    friend std::ostream &operator<<(std::ostream &os, const Person &p)
    {
        os << CYAN << "[" << p.getRole() << "] " << RESET << p.name;
        return os;
    }
};

// ── Class: Contact  (virtual single inheritance from Person) ──────────────
// Demonstrates: Single inheritance, virtual base class
class Contact : virtual public Person
{
protected:
    std::string phone;

public:
    Contact() : Person(), phone("") {}
    Contact(const std::string &n, const std::string &e, const std::string &p)
        : Person(n, e), phone(p) {}
    Contact(const Contact &o) : Person(o), phone(o.phone) {}
    Contact &operator=(const Contact &o)
    {
        if (this != &o)
        {
            Person::operator=(o);
            phone = o.phone;
        }
        return *this;
    }
    virtual ~Contact() {}

    std::string getPhone() const { return phone; }
    void setPhone(const std::string &p) { phone = p; }

    void displayInfo() const override
    {
        std::cout << CYAN << "  Name : " << RESET << name << "\n"
                  << CYAN << "  Email: " << RESET << email << "\n"
                  << CYAN << "  Phone: " << RESET << phone << "\n";
    }
    std::string getRole() const override { return "Contact"; }
};

// ── Class: AccountHolder  (virtual single inheritance from Person) ────────
// Demonstrates: Single inheritance, static members, virtual base class
class AccountHolder : virtual public Person
{
protected:
    std::string username;
    std::string passwordHash;
    std::string createdAt;
    static int totalAccounts;

public:
    AccountHolder() : Person(), username(""), passwordHash(""), createdAt("") { ++totalAccounts; }

    AccountHolder(const std::string &n, const std::string &e,
                  const std::string &u, const std::string &ph)
        : Person(n, e), username(u), passwordHash(ph),
          createdAt(Utils::getFullTimestamp()) { ++totalAccounts; }

    AccountHolder(const AccountHolder &o)
        : Person(o), username(o.username), passwordHash(o.passwordHash),
          createdAt(o.createdAt) { ++totalAccounts; }

    AccountHolder &operator=(const AccountHolder &o)
    {
        if (this != &o)
        {
            Person::operator=(o);
            username = o.username;
            passwordHash = o.passwordHash;
            createdAt = o.createdAt;
        }
        return *this;
    }
    virtual ~AccountHolder() { --totalAccounts; }

    std::string getUsername() const { return username; }
    std::string getPasswordHash() const { return passwordHash; }
    std::string getCreatedAt() const { return createdAt; }
    void setCreatedAt(const std::string &t) { createdAt = t; }

    static int getTotalAccounts() { return totalAccounts; }

    static std::string hashPassword(const std::string &pw)
    {
        std::hash<std::string> h;
        return std::to_string(h(pw));
    }

    bool checkPassword(const std::string &pw) const
    {
        return passwordHash == hashPassword(pw);
    }

    void displayInfo() const override
    {
        std::cout << CYAN << "  Username: " << RESET << username << "\n"
                  << CYAN << "  Name    : " << RESET << name << "\n"
                  << CYAN << "  Email   : " << RESET << email << "\n"
                  << CYAN << "  Joined  : " << RESET << createdAt << "\n";
    }
    std::string getRole() const override { return "AccountHolder"; }
};

int AccountHolder::totalAccounts = 0;
