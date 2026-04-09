#pragma once

#include <cassert>
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
    Error(T value, std::string message, std::optional<Error<T>> reason = std::nullopt, std::source_location loc = std::source_location::current())
        : m_value(std::move(value))
        , m_message(std::move(message))
        , m_reason(reason ? std::make_shared<Error<T>>(std::move(*reason)) : nullptr)
        , m_loc(std::move(loc))
    {
    }

    Error(std::string message, std::optional<Error<T>> reason = std::nullopt, std::source_location loc = std::source_location::current())
        : Error({}, std::move(message), std::move(reason), std::move(loc))
    {
    }

    const auto& value() const { return m_value; }
    const auto& message() const { return m_message; }
    std::optional<Error<T>> reason() const { return m_reason ? std::make_optional(*m_reason) : std::nullopt; }
    const auto& loc() const { return m_loc; }

private:
    T m_value;
    std::string m_message;
    std::shared_ptr<Error<T>> m_reason;
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

template<typename T = Unit>
class AsReason {
public:
    AsReason(std::string message, std::source_location loc = std::source_location::current())
        : m_message(std::move(message))
        , m_loc(std::move(loc))
    {
    }

    Error<T> operator()(Error<T> err) && noexcept
    {
        assert(!m_moved);
        m_moved = true;
        return Error<T>(std::move(m_message), err, m_loc);
    }

private:
    std::string m_message;
    std::source_location m_loc;
    bool m_moved = false;
};

template<typename T = Unit>
class AsCritical {
public:
    AsCritical(std::source_location loc = std::source_location::current())
        : m_loc(std::move(loc))
    {
    }

    [[noreturn]] Error<T> operator()(Error<T> err) && noexcept
    {
        std::ostringstream ss;
        ss << "CRITICAL:\n"
           << tab << locToUrl(m_loc) << ": Error handled as critical here";
        printErrorRecursive(ss, err);
        printError(ss.str());
        std::abort();
    }

private:
    static std::string locToUrl(std::source_location loc)
    {
        return std::string("file://") + loc.file_name() + ":" + std::to_string(loc.line()) + ":" + std::to_string(loc.column());
    }

    static void printErrorRecursive(std::ostream& stream, const Error<T>& err)
    {
        stream << '\n'
               << tab << locToUrl(err.loc()) << ": " << err.message();
        if (const auto& reason = err.reason()) {
            printErrorRecursive(stream, *reason);
        }
    }

private:
    std::source_location m_loc;
    static constexpr const char* tab = "    ";
};

}
