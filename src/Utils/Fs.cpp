#include "Fs.h"

#include "NumericCast.h"

#include <cstring>
#include <fstream>

namespace BadgerEngine::Fs {

namespace {

std::vector<std::uint8_t> charToUInt8(std::vector<char> d)
{
    return std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(d.data()), reinterpret_cast<std::uint8_t*>(d.data()) + d.size());
}

}

Expected<Bytes> readBinary(const std::filesystem::path& path) noexcept
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return unexpected("Failed to open file \"" + path.string() + "\": " + std::strerror(errno));
    }

    const std::size_t fileSize = numericCast<std::size_t>(static_cast<std::streamoff>(file.tellg())).value();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), numericCast<std::streamsize>(fileSize).value());

    file.close();

    return charToUInt8(buffer);
}

}
