#include "../Utils/Error.h"
#include "Texture.h"
#include <filesystem>
#include <map>

namespace BadgerEngine {

class TextureLoader {
public:
    /// Internal texture path
    /// Must match with the path inside 3d model file
    using VirtualTexturePath = std::string;

    TextureLoader& operator=(TextureLoader&&) = delete;
    TextureLoader& operator=(const TextureLoader&) = delete;
    TextureLoader(TextureLoader&&) = delete;
    TextureLoader(const TextureLoader&) = delete;
    TextureLoader() = default;

    [[nodiscard]] Expected<void> load(const VirtualTexturePath& vpath, const std::filesystem::path& path) noexcept;
    [[nodiscard]] Expected<void> load(std::map<VirtualTexturePath, std::filesystem::path> recipes) noexcept;
    [[nodiscard]] Expected<void> parse(const VirtualTexturePath& vpath, std::span<const std::uint8_t> bytes) noexcept;
    [[nodiscard]] Expected<void> parse(std::map<VirtualTexturePath, std::span<const std::uint8_t>> recipes) noexcept;

    std::optional<SharedTexture> texture(const VirtualTexturePath& path) const noexcept
    {
        const auto it = m_textures.find(path);
        if (it != m_textures.end()) {
            return it->second;
        } else {
            return std::nullopt;
        }
    }

private:
    std::map<VirtualTexturePath, SharedTexture> m_textures;
};

}
