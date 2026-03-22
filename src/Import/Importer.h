#pragma once

#include "Data.h"

namespace BadgerEngine::Import {

class Importer {
public:
    Importer() = default;
    virtual ~Importer() = default;

    /**
     * @brief load
     * @param path
     * @return
     */
    virtual Expected<Model> load(
        const TextureLoader&,
        const std::filesystem::path& path) const noexcept
        = 0;

    /**
     * @brief parse
     * @param data
     * @param hint
     * @return
     */
    virtual Expected<Model> parse(
        const TextureLoader&,
        std::span<const std::uint8_t> data,
        const std::map<std::string, std::span<const std::uint8_t>>& additionalData,
        const std::string& hint) const noexcept
        = 0;
};

}
