#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

enum class Texture2DWrapMode{
	CLAMP_EDGE,
	CLAMP_BORDER,
	MIRRORED_REPEAT,
	REPEAT,
	MIRRORED_CLAMP_EDGE
};

struct Texture2DAsset : public Asset {
	Texture2DAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::TEXTURE_2D; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		m_BPP = 0;
		m_Width = 0;
		m_Height = 0;

		m_DataFormat = 0;
		m_InternalFormat = 0;
		m_DataType = GL_UNSIGNED_BYTE;

		m_Data.clear();

		const auto hdr = path.extension() == ".hdr";

		stbi_set_flip_vertically_on_load(flipVertically);
		int width = 0, height = 0, bpp = 0;
		stbi_uc* data = hdr == false ? stbi_load(path.string().c_str(), &width, &height, &bpp, 0)
									 : reinterpret_cast<stbi_uc*>(stbi_loadf(path.string().c_str(), &width, &height, &bpp, 0));
		if (data == nullptr) {
			spdlog::error("[Texture2DAsset::LoadAsset] Couldn't load texture: {}", path);
			return;
		}

		m_Data.assign(data, data + (width * height * bpp * (hdr ? sizeof(float) : sizeof(stbi_uc)) ));
		stbi_image_free(data);

		GLenum internalFormat = 0, dataFormat = 0;
		switch (bpp) {
		case 1:
			dataFormat = GL_RED;
			internalFormat = GL_RED;
			break;
		case 2:
			dataFormat = GL_RG;
			internalFormat = GL_RG8;
			break;
		case 3:
			dataFormat = GL_RGB;
			internalFormat = GL_RGB8;
			break;
		case 4:
			dataFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
		}
		m_DataFormat = dataFormat;
		m_InternalFormat = internalFormat;

		if (hdr) {
			m_DataFormat = GL_RGB;
			m_InternalFormat = GL_RGB16F;
			m_DataType = GL_FLOAT;
		}

		m_BPP = bpp;
		m_Width = width;
		m_Height = height;

		// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // to load data with weird width and heights
		// glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}

	virtual void LoadMeta(cereal::JSONInputArchive& archive) override {
		bool t_FlipVertically{};
		if (cereal::make_optional_nvp(archive, "flip_vertically", t_FlipVertically)) {
			flipVertically = t_FlipVertically;
		}
		std::string wrapModeStr{};
		if (cereal::make_optional_nvp(archive, "wrapMode", wrapModeStr)) {
			const auto wrapModeOpt = magic_enum::enum_cast<Texture2DWrapMode>(wrapModeStr);
			if (wrapModeOpt.has_value()) {
				wrapMode = wrapModeOpt.value();
			}
			else {
				spdlog::error("[Texture2DAsset::LoadMeta] Unknown wrap mode: {}", wrapModeStr);
			}
		}
	};
	virtual void SaveMeta(cereal::JSONOutputArchive& archive) const override {
		archive(cereal::make_nvp("flip_vertically", flipVertically));
		archive(cereal::make_nvp("wrapMode", std::string{ magic_enum::enum_name(wrapMode) }));
	};

	uint32 GetWidth() const { return m_Width; }
	uint32 GetHeight() const { return m_Height; }
	uint32 GetChannels() const { return m_BPP; }

private:
	uint32 m_BPP = 0;
	uint32 m_Width = 0;
	uint32 m_Height = 0;

	uint32 m_DataFormat = 0;
	uint32 m_InternalFormat = 0;
	uint32 m_DataType = GL_UNSIGNED_BYTE;

	std::vector<stbi_uc> m_Data;

public:
	bool flipVertically = true;
	Texture2DWrapMode wrapMode = Texture2DWrapMode::REPEAT;

	friend class Renderer;
};

template <>
inline AssetType AssetTypeFromType<Texture2DAsset>() {
	return AssetType::TEXTURE_2D;
}