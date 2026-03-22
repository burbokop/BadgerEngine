#pragma once

#include "Importer.h"

namespace BadgerEngine::Import {

class AssimpImporter : public Importer {
public:
    AssimpImporter() = default;

    // Importer interface
public:
    Expected<Model> load(const TextureLoader&, const std::filesystem::path& path) const noexcept override;
    Expected<Model> parse(
        const TextureLoader&,
        std::span<const std::uint8_t> data,
        const std::map<std::string, std::span<const std::uint8_t>>& additionalData,
        const std::string& hint) const noexcept override;
};

}
