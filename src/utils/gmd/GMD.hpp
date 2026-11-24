#pragma once

#include <optional>
#include <Geode/Result.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <asp/fs/Path.hpp>

#define GMDAPI_DLL

namespace gmd {
    class ImportGmdFile;
    class ExportGmdFile;

    enum class GmdFileType {
        Lvl,
        Gmd,
        Gmd2,
    };

    constexpr auto DEFAULT_GMD_TYPE = GmdFileType::Gmd;
    constexpr auto GMD2_VERSION = 1;

    constexpr const char* gmdTypeToString(GmdFileType type) {
        switch (type) {
            case GmdFileType::Lvl:  return "lvl";
            case GmdFileType::Gmd:  return "gmd";
            case GmdFileType::Gmd2: return "gmd2";
            default:                return nullptr;
        }
    }
    constexpr std::optional<GmdFileType> gmdTypeFromString(const char* type) {
        using geode::utils::hash;
        switch (hash(type)) {
            case hash("lvl"):  return GmdFileType::Lvl;
            case hash("gmd"):  return GmdFileType::Gmd;
            case hash("gmd2"): return GmdFileType::Gmd2;
            default:           return std::nullopt;
        }
    }

    template<class T>
    class IGmdFile {
    protected:
        std::optional<GmdFileType> m_type;

    public:
        T& setType(GmdFileType type) {
            m_type = type;
            return *static_cast<T*>(this);
        }
    };

    class ImportGmdFile : public IGmdFile<ImportGmdFile> {
    protected:
        asp::fs::Path m_path;
        bool m_importSong = false;

        ImportGmdFile(asp::fs::Path const& path);

        geode::Result<std::string> getLevelData() const;

    public:
        static ImportGmdFile from(asp::fs::Path const& path);
        bool tryInferType();
        ImportGmdFile& inferType();
        ImportGmdFile& setImportSong(bool song);
        geode::Result<GJGameLevel*> intoLevel() const;
    };

    class ExportGmdFile : public IGmdFile<ExportGmdFile> {
    protected:
        GJGameLevel* m_level;
        bool m_includeSong = false;

        ExportGmdFile(GJGameLevel* level);

        geode::Result<std::string> getLevelData() const;

    public:
        static ExportGmdFile from(GJGameLevel* level);
        ExportGmdFile& setIncludeSong(bool song);
        geode::Result<geode::ByteVector> intoBytes() const;
        geode::Result<> intoFile(asp::fs::Path const& path) const;
    };

    geode::Result<> exportLevelAsGmd(
        GJGameLevel* level,
        asp::fs::Path const& to,
        GmdFileType type = DEFAULT_GMD_TYPE
    );

    geode::Result<GJGameLevel*> importGmdAsLevel(
        asp::fs::Path const& from
    );
}
