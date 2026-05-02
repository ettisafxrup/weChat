# weChat — Let's make Friends 💬

### A Terminal-Based C++ OOP Chat Application

---

## 🚀 Quick Start

```bash
# Compile
make

# Run
./weChat
```

Or without make:
run build files (build.bat if Windows CMD and build.sh if Shell) and you're good to go!

---

## 📁 Project Architecture

```
weChat/
├── main.cpp                  ← Entry point, main application loop
├── 🔵 object
│   ├── main.o ← Object file forked from main.cpp
├── 🔺 release
│   ├── weChat_v1.0.exe ← All the versions creating after every compile you do.
    ...
    ...
├── 📂 data/                     ← Auto-created at runtime
│   ├── users.dat             ← Persisted user accounts
│   └── rooms.dat             ← Persisted rooms + members + messages
└── 📂 include/
    ├── Utils.h               ← Colors, helpers, timestamp, code gen
    ├── Message.h             ← Message class + Payload<T> template + ISerializable
    ├── Person.h              ← Person (abstract) → Contact, AccountHolder
    ├── User.h                ← User (multiple inheritance) → Admin (multilevel)
    ├── Room.h                ← BaseRoom (abstract) → Room, AnnouncementRoom
    ├── FileManager.h         ← File I/O with template methods
    ├── AppState.h            ← Singleton state manager + polymorphism
    └── UI.h                  ← All terminal screens and user interaction
```

---

## ✅ C++ OOP Concepts Covered

| Concept                       | Where                                                              |
| ----------------------------- | ------------------------------------------------------------------ |
| **Classes & Objects**         | All header files                                                   |
| **Array of Objects**          | `vector<User>`, `vector<Room>`, `vector<Message>` in AppState/Room |
| **Object References**         | `AppState&`, `Room&`, function parameters                          |
| **Memory Allocation**         | `new AppState()` / `delete instance` in singleton                  |
| **Default Constructor**       | All classes                                                        |
| **Parameterised Constructor** | All classes                                                        |
| **Copy Constructor**          | Message, User, Room, Person, Contact, AccountHolder                |
| **Destructor**                | All classes; virtual in hierarchy                                  |
| **Function Overloading**      | `addMessage()`, `findRoom()`, `showMenu()`, `sendMessage()`        |
| **Operator Overloading**      | `==`, `<`, `+`, `+=`, `<<`, `>>` (cin/cout), explicit cast         |
| **Type Conversion**           | `User→string`, `Room→string`, `Message→string`                     |
| **Static Members**            | `totalMessages`, `totalRooms`, `totalAccounts`, `onlineCount`      |
| **Friend Functions**          | `operator<<` and `operator>>` in Message, User, Person, Room       |
| **cin/cout Overloading**      | `operator>>` for Message input; `operator<<` for display           |
| **Single Inheritance**        | `Contact ← Person`, `AccountHolder ← Person`                       |
| **Multiple Inheritance**      | `User ← Contact + AccountHolder`                                   |
| **Multilevel Inheritance**    | `Admin ← User ← (Contact,AccountHolder) ← Person`                  |
| **Hierarchical Inheritance**  | `Room ← BaseRoom`, `AnnouncementRoom ← BaseRoom`                   |
| **Virtual Base Class**        | `virtual Person` in Contact & AccountHolder (solves diamond)       |
| **Compile-time Polymorphism** | Function overloading, operator overloading                         |
| **Runtime Polymorphism**      | `virtual displayInfo()`, `getRole()`, `getType()`                  |
| **Pointer to Derived Class**  | `BaseRoom* br = room; br->displayInfo();` in AppState              |
| **Abstract Base Class**       | `Person`, `BaseRoom`, `ISerializable` (pure virtual methods)       |
| **Virtual Functions**         | `displayInfo()`, `getRole()`, `getType()`, `getMemberCount()`      |
| **Templates**                 | `Payload<T>`, `FileManager::appendToFile<T>`, `readFromFile<T>`    |
| **File Handling**             | `fstream` in FileManager — users.dat, rooms.dat                    |

---

## 🖥 Features

- **User Accounts**: Register with name, email, phone, username & password
- **Secure Storage**: Passwords stored as hash (std::hash), never plaintext
- **Create Rooms**: Create public or private chat rooms
- **Join Rooms**: Join any room using the 6-character room code
- **Real-time Chat**: Send and receive messages inside rooms
- **Member Lists**: View all members in a room
- **Profile View**: See your own account details
- **Statistics**: View global stats (users, rooms, messages)
- **Full Persistence**: All data saved to files, reloaded on next run

---

## 💡 How to Use

1. **Start** the app → choose Register or Login
2. **Register** → fill in your details
3. **Login** → enter username & password
4. **Dashboard** → create a room or join one
5. **Create Room** → note the join code shown on screen
6. **Share the code** with a friend → they use "Join Room"
7. **Chat** → select a room from "My Rooms" and start messaging

---

## 🔧 Requirements

- g++ with C++17 support (`g++ --version`)
- Linux / macOS / WSL on Windows
- Terminal with ANSI color support (most modern terminals)
