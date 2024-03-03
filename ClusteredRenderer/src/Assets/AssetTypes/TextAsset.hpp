#pragma once

#include <Core.hpp>
#include "AssetType.hpp"

struct TextAsset : public Asset {
	TextAsset() = default;
	virtual constexpr AssetType GetType() const override { return AssetType::TEXT; };

	virtual void LoadAsset(const std::filesystem::path& path) override {
		std::ifstream input(path);
		const auto size = std::filesystem::file_size(path);
		text.resize(size);
		input.read(text.data(), size);
	}
	virtual void SaveAsset(const std::filesystem::path& path) const override {
		std::ofstream output(path);
		output << text;
	}

	virtual void LoadMeta(cereal::JSONInputArchive& archive) override{
		std::optional<int> text_size_opt{};
		cereal::make_optional_nvp(archive, "text_size", text_size_opt);

		const auto text_size = text_size_opt.value_or(0);
		spdlog::debug("Reading test text_size: {}", text_size);
	};
	virtual void SaveMeta(cereal::JSONOutputArchive& archive) const override{
		archive(cereal::make_nvp("text_size", text.length()));
	};

	std::string text;
};

template <>
inline AssetType AssetTypeFromType<TextAsset>() {
	return AssetType::TEXT;
}