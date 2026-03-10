#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <source_location>
#include <sstream>
#include <string>

namespace BadgerEngine {

struct Unit {};

template<typename T = Unit>
class [[nodiscard]] Error {
public:
    Error(T value, std::string message, std::optional<Error<Unit>> reason = std::nullopt, std::source_location loc = std::source_location::current())
        : m_value(std::move(value))
        , m_message(std::move(message))
        , m_reason(reason ? std::make_shared<Error<Unit>>(std::move(*reason)) : nullptr)
        , m_loc(std::move(loc))
    {
    }

    Error(std::string message, std::optional<Error<Unit>> reason = std::nullopt, std::source_location loc = std::source_location::current())
        : Error({}, std::move(message), std::move(reason), std::move(loc))
    {
    }

    const auto& value() const { return m_value; }
    const auto& message() const { return m_message; }
    std::optional<Error<Unit>> reason() const { return m_reason ? std::make_optional(*m_reason) : std::nullopt; }
    const auto& loc() const { return m_loc; }

private:
    T m_value;
    std::string m_message;
    std::shared_ptr<Error<Unit>> m_reason;
    std::source_location m_loc;
};

template<typename T, typename E = Unit>
using Expected = std::expected<T, Error<E>>;

template<typename E = Unit>
using Unexpected = std::unexpected<Error<E>>;

template<typename T = Unit>
auto unexpected(T value, std::string message, std::optional<Error<Unit>> reason = std::nullopt, std::source_location loc = std::source_location::current())
{
    return std::unexpected(Error(std::move(value), std::move(message), std::move(reason), std::move(loc)));
}

template<typename T = Unit>
auto unexpected(std::string message, std::optional<Error<Unit>> reason = std::nullopt, std::source_location loc = std::source_location::current())
{
    return std::unexpected(Error<T>(std::move(message), std::move(reason), std::move(loc)));
}

template<typename T = Unit>
auto unexpected(Error<T> err)
{
    return std::unexpected(err);
}

void printError(const std::string& err);

template<typename T>
void printErrorRecursive(std::ostream& stream, const Error<T>& err)
{
    const auto tab = "    ";
    stream << '\n'
           << tab << "file://" << err.loc().file_name() << ":" << err.loc().line() << ": " << err.message();
    if (const auto& reason = err.reason()) {
        printErrorRecursive(stream, *reason);
    }
}

template<typename T = Unit>
[[noreturn]] Error<T> handleAsCritical(Error<T> err)
{
    if (err.reason()) {
        std::ostringstream ss;
        ss << "CRITICAL:";
        printErrorRecursive(ss, err);
        printError(ss.str());
    } else {
        std::ostringstream ss;
        ss << "CRITICAL file://" << err.loc().file_name() << ":" << err.loc().line() << ": " << err.message();
        printError(ss.str());
    }
    std::abort();
}

}
