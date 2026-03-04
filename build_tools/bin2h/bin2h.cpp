#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

const auto Usage = "Usage: bin2h [input-path] --name [variable-name] > [output-path]";

struct Args {
    std::filesystem::path inputPath;
    std::optional<std::string> variableName;
};

std::expected<Args, int> parseArgs(int argc, const char** argv)
{
    std::filesystem::path inputPath;
    std::optional<std::string> variableName;

    if(argc < 2) {
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
        } else {
            inputPath = arg;
        }
    }

    return Args {
        .inputPath = std::move(inputPath),
        .variableName = std::move(variableName),
    };
}

std::expected<void, int> writeHCode(std::ostream& output, std::istream& input, std::size_t byteCount, const std::string& variableName)
{    
    output << "#pragma once\n"
           << "\n"
           << "#include <array>\n"
           << "#include <cstdint>\n"
           << "\n"
           << "constexpr std::array<std::uint8_t, " << byteCount << "> " << variableName << " = {\n";

    for (std::istreambuf_iterator it(input); it != std::istreambuf_iterator<char>(); ++it) {
        output << "\t0x" << std::hex << static_cast<std::uint16_t>(static_cast<std::uint8_t>(*it))  << ",\n";
    }

    output << "};\n";
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

    const auto result = writeHCode(std::cout, input, std::filesystem::file_size(args->inputPath), args->variableName.value_or(args->inputPath));
    if (!result) {
        return result.error();
    }

    return 0;
}
