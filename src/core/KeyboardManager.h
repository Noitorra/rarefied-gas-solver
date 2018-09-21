#ifndef RGS_KEYBOARDMANAGER_H
#define RGS_KEYBOARDMANAGER_H

#include <atomic>

class KeyboardManager {
public:
    enum class State {
        DISABLED,
        WAITING_INPUT,
        WAITING_COMMAND
    };

    enum class Command {
        NONE,
        STOP
    };

private:
    std::atomic<State> _state;
    std::atomic<Command> _command;

public:
    static KeyboardManager* getInstance() {
        static auto instance = new KeyboardManager();
        return instance;
    }

    void start();

    bool isWaitingCommand() const;

    bool isStop() const;

private:
    KeyboardManager();

};


#endif //RGS_KEYBOARDMANAGER_H
