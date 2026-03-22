#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const auto Usage = "Usage: bin2h [input-path] --name [variable-name] [--constexpr] [--extern] > [output-path]";

struct Args {
    std::filesystem::path inputPath;
    std::optional<std::string> variableName;
    std::vector<std::string> include;
    bool constexprModifier;
    bool externModifier;
    bool pragmaOnce;
};

std::expected<Args, int> parseArgs(int argc, const char** argv)
{
    std::filesystem::path inputPath;
    std::optional<std::string> variableName;
    std::vector<std::string> include;
    bool constexprModifier = false;
    bool externModifier = false;
    bool pragmaOnce = false;

    if (argc < 2) {
        std::cerr << "error: at least one argument required" << std::endl;
        std::cout << Usage << std::endl;
        return std::unexpected(-1);
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);
        if (arg == "--name") {
            if (i + 1 >= argc) {
                std::cerr << "error: argument after --name expected." << std::endl;
                return std::unexpected(-1);
            }
            variableName = std::string(argv[++i]);
        } else if (arg == "--constexpr") {
            constexprModifier = true;
        } else if (arg == "--extern") {
            externModifier = true;
        } else if (arg == "--pragma-once") {
            pragmaOnce = true;
        } else if (arg == "--include") {
            if (i + 1 >= argc) {
                std::cerr << "error: argument after --include expected." << std::endl;
                return std::unexpected(-1);
            }
            include.push_back(argv[++i]);
        } else {
            inputPath = arg;
        }
    }

    return Args {
        .inputPath = std::move(inputPath),
        .variableName = std::move(variableName),
        .include = std::move(include),
        .constexprModifier = constexprModifier,
        .externModifier = externModifier,
        .pragmaOnce = pragmaOnce,
    };
}

std::string replace(std::string str, const std::string& from, const std::string to)
{
    std::string::size_type n = 0;
    while ((n = str.find(from, n)) != std::string::npos) {
        str.replace(n, from.size(), to);
        n += to.size();
    }

    return str;
}

std::string replaceUnfitCharacters(std::string str)
{
    str = replace(std::move(str), "+", "_plus_");
    str = replace(std::move(str), "-", "_minus_");
    str = replace(std::move(str), "*", "_mul_");
    str = replace(std::move(str), "/", "_div_");
    str = replace(std::move(str), "~", "_tilda_");
    str = replace(std::move(str), "^", "_pow_");
    str = replace(std::move(str), "#", "_hash_");
    str = replace(std::move(str), "@", "_at_");
    str = replace(std::move(str), "!", "_excl_");
    str = replace(std::move(str), "?", "_quest_");
    str = replace(std::move(str), "=", "_eq_");
    str = replace(std::move(str), "\\", "_bslash_");
    str = replace(std::move(str), "&", "_and_");
    str = replace(std::move(str), "%", "_pcent_");
    str = replace(std::move(str), ",", "_coma_");
    str = replace(std::move(str), ":", "_colon_");
    str = replace(std::move(str), ";", "_scolon_");
    str = replace(std::move(str), "'", "_quote_");
    str = replace(std::move(str), "\"", "_dquote_");
    str = replace(std::move(str), "<", "_less_");
    str = replace(std::move(str), ">", "_greater_");
    str = replace(std::move(str), "|", "_wall_");

    return str;
}

std::expected<void, int> writeHCode(
    std::ostream& output,
    std::istream& input,
    std::size_t byteCount,
    const std::string& variableName,
    const std::vector<std::string>& include,
    bool constexprModifier,
    bool externModifier,
    bool pragmaOnce)
{
    output << (pragmaOnce ? "#pragma once\n" : "")
           << "\n"
           << "#include <array>\n"
           << "#include <cstdint>\n";

    for (const auto& i : include) {
        output << "#include <" << i << ">\n";
    }

    output << "\n"
           << (externModifier ? "extern " : "")
           << (constexprModifier ? "constexpr " : "const ")
           << "std::array<std::uint8_t, " << byteCount << "> "
           << replaceUnfitCharacters(variableName)
           << (externModifier ? ";\n" : " = {\n");

    if (!externModifier) {
        for (std::istreambuf_iterator it(input); it != std::istreambuf_iterator<char>(); ++it) {
            output << "\t0x" << std::hex << static_cast<std::uint16_t>(static_cast<std::uint8_t>(*it)) << ",\n";
        }

        output << "};\n";
    }

    output.flush();
    return {};
}

int main(int argc, const char** argv)
{
    const auto args = parseArgs(argc, argv);
    if (!args) {
        return args.error();
    }

    std::ifstream input(args->inputPath, std::ios::in | std::ios::binary);

    if (!input.is_open() || input.bad()) {
        std::cerr << "error: failed to open file `" << args->inputPath << "`: " << std::strerror(errno) << std::endl;
        return -2;
    }

    const auto result = writeHCode(
        std::cout,
        input,
        std::filesystem::file_size(args->inputPath),
        args->variableName.value_or(args->inputPath.string()),
        args->include,
        args->constexprModifier,
        args->externModifier,
        args->pragmaOnce);

    if (!result) {
        return result.error();
    }

    return 0;
}
