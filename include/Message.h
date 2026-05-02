#pragma once
#include "Utils.h"

// ── Template: Payload<T> ──────────────────────────────────────────────────
// Demonstrates: Templates, generic programming
template <typename T>
class Payload {
private:
    T data;
public:
    Payload() = default;
    explicit Payload(const T& d) : data(d) {}
    T    getData()        const { return data; }
    void setData(const T& d)    { data = d; }
    bool equals(const T& other) const { return data == other; }
};

// ── Abstract Base: ISerializable ──────────────────────────────────────────
// Demonstrates: Abstract base class, pure virtual functions
class ISerializable {
public:
    virtual std::string serialize()              const = 0;
    virtual bool        deserialize(const std::string&)  = 0;
    virtual ~ISerializable() = default;
};

// ── Class: Message ────────────────────────────────────────────────────────
// Demonstrates: Constructors/Destructors, Copy Constructor, Operator
//               overloading, friend functions (cin/cout), static members
class Message : public ISerializable {
private:
    std::string sender;
    std::string content;
    std::string timestamp;
    static int  totalMessages;

public:
    // Default constructor
    Message() : sender(""), content(""), timestamp("") { ++totalMessages; }

    // Parameterised constructor
    Message(const std::string& s, const std::string& c)
        : sender(s), content(c), timestamp(Utils::getTimestamp()) { ++totalMessages; }

    // Copy constructor
    Message(const Message& o)
        : sender(o.sender), content(o.content), timestamp(o.timestamp) { ++totalMessages; }

    // Copy-assignment
    Message& operator=(const Message& o) {
        if (this != &o) { sender = o.sender; content = o.content; timestamp = o.timestamp; }
        return *this;
    }

    // Destructor
    ~Message() { --totalMessages; }

    // Accessors
    std::string getSender()    const { return sender; }
    std::string getContent()   const { return content; }
    std::string getTimestamp() const { return timestamp; }
    void setTimestamp(const std::string& ts) { timestamp = ts; }

    // Static member accessor
    static int getTotalMessages() { return totalMessages; }

    // Operator +  — concatenate content
    Message operator+(const Message& o) const { return Message(sender, content + " " + o.content); }

    // Operator == — compare by sender+content
    bool operator==(const Message& o) const { return sender == o.sender && content == o.content; }

    // Operator <  — sort by timestamp
    bool operator<(const Message& o) const { return timestamp < o.timestamp; }

    // friend: cout overloading ─────────────────────────────────────────────
    friend std::ostream& operator<<(std::ostream& os, const Message& m) {
        os << GREEN << "[" << m.timestamp << "] "
           << CYAN << BOLD << m.sender << RESET
           << WHITE << ": " << m.content << RESET;
        return os;
    }

    // friend: cin overloading ──────────────────────────────────────────────
    friend std::istream& operator>>(std::istream& is, Message& m) {
        std::cout << YELLOW << "  Message: " << RESET;
        std::getline(is, m.content);
        m.timestamp = Utils::getTimestamp();
        return is;
    }

    // Type conversion: Message -> string
    explicit operator std::string() const {
        return "[" + timestamp + "] " + sender + ": " + content;
    }

    // ISerializable ────────────────────────────────────────────────────────
    std::string serialize() const override {
        // Use ||| as delimiter (unlikely to appear in chat)
        return sender + "|||" + content + "|||" + timestamp;
    }

    bool deserialize(const std::string& line) override {
        size_t p1 = line.find("|||");
        if (p1 == std::string::npos) return false;
        size_t p2 = line.find("|||", p1 + 3);
        if (p2 == std::string::npos) return false;
        sender    = line.substr(0, p1);
        content   = line.substr(p1 + 3, p2 - p1 - 3);
        timestamp = line.substr(p2 + 3);
        return !sender.empty();
    }
};

int Message::totalMessages = 0;
