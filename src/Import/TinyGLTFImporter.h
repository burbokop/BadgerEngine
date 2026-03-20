#pragma once

#include "Importer.h"

namespace BadgerEngine::Import {

class TinyGLTFImporter : public Importer {
public:
    TinyGLTFImporter() = default;

    // Importer interface
public:
    Expected<Model> load(const TextureLoader&, const std::filesystem::path& path) const noexcept override;
    Expected<Model> parse(const TextureLoader&, std::span<std::uint8_t> data, const std::string& hint) const noexcept override;
};

}
