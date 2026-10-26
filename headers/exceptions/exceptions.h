#pragma once
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

class MyException : public std::exception {
private:
    std::string message;
public:
    explicit MyException(const std::string& message_in) : message(message_in) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class RepoException : public MyException {
public:
    explicit RepoException(const std::string& message_in) : MyException(message_in) {}
};

class MovieException : public std::exception {
private:
    std::vector<std::invalid_argument> err;
    std::string message;

    void buildMessage() {
        for (const auto& e : err) {
            if (!message.empty()) message += "; ";
            message += e.what();
        }
    }

public:
    explicit MovieException(std::vector<std::invalid_argument> errors)
        : err(std::move(errors)) { buildMessage(); }

    explicit MovieException(const std::string& error)
        : err{std::invalid_argument(error)} { buildMessage(); }

    const std::vector<std::invalid_argument>& getErrors() const noexcept { return err; }
    const char* what() const noexcept override { return message.c_str(); }
};
