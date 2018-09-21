#include "KeyboardManager.h"

#include <thread>
#include <iostream>
#include <limits>

KeyboardManager::KeyboardManager() : _state(State::DISABLED), _command(Command::NONE) {}

void KeyboardManager::start() {
    std::thread thread([this] {
        _state = State::WAITING_INPUT;
        while (true) {
            if (_state == State::WAITING_INPUT) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                _state = State::WAITING_COMMAND;
            } else if (_state == State::WAITING_COMMAND) {
                std::cout << "Command: ";

                std::string input;
                std::cin >> input;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

                if (input == "stop") {
                    _command = Command::STOP;
                    break;
                } else if (input == "cancel") {
                    _command = Command::NONE;
                    _state = State::WAITING_INPUT;
                }
            }
        }
    });
    thread.detach();
}

bool KeyboardManager::isWaitingCommand() const {
    return _state == State::WAITING_COMMAND;
}

bool KeyboardManager::isStop() const {
    return _command == Command::STOP;
}