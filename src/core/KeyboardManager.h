#ifndef RGS_KEYBOARDMANAGER_H
#define RGS_KEYBOARDMANAGER_H

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
    bool _isAvailable;
    State _state;
    Command _command;

public:
    static KeyboardManager* getInstance() {
        static auto instance = new KeyboardManager();
        return instance;
    }

    bool isAvailable() const;

    void setAvailable(bool isAvailable);

    void start();

    bool isWaitingCommand() const;

    bool isStop() const;

private:
    KeyboardManager();

};


#endif //RGS_KEYBOARDMANAGER_H
