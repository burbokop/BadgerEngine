#pragma once

#include "Error.h"
#include <cstdint>
#include <filesystem>
#include <vector>

namespace BadgerEngine {

using Bytes = std::vector<std::uint8_t>;

namespace Fs {

Expected<Bytes> readBinary(const std::filesystem::path& path) noexcept;

}

}
