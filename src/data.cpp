#include "ioh/common/data.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <mutex>
#include <set>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace ioh::data
{
    namespace
    {
        //! A row of the manifest compiled into the library
        struct GeneratedEntry
        {
            const char *family;
            int id;
            const char *name;
            const char *meta_name;
            const char *problem_class;
            const char *definition;
        };

#include "data_manifest.inc"

        std::mutex root_mutex;

        //! The directory given to set_root (a function-local static: set_root may be called while other static
        //! variables are initialized)
        fs::path &explicit_root()
        {
            static fs::path root;
            return root;
        }

        //! The data version in the manifest.json of a directory, or "" when it has none
        std::string read_version(const fs::path &dir)
        {
            std::ifstream in(dir / "manifest.json");
            if (!in)
                return {};
            const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            const std::string key = "\"data_version\": \"";
            const auto start = text.find(key);
            if (start == std::string::npos)
                return {};
            const auto begin = start + key.size();
            return text.substr(begin, text.find('"', begin) - begin);
        }

        //! The environment variable IOH_RESOURCES, empty when it is not set (on Windows read as wide characters,
        //! so that any path works)
        fs::path resources_variable()
        {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) // getenv
#endif
#ifdef _WIN32
            const wchar_t *value = _wgetenv(L"IOH_RESOURCES");
#else
            const char *value = std::getenv("IOH_RESOURCES");
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
            return value == nullptr ? fs::path() : fs::path(value);
        }

        //! The data directory and where it came from
        std::pair<fs::path, std::string> find_root()
        {
            {
                const std::lock_guard<std::mutex> lock(root_mutex);
                const auto &root = explicit_root();
                if (!root.empty())
                {
                    if (fs::is_directory(root))
                        return {root, "set with ioh::data::set_root"};
                    throw data_error(fmt::format("The IOH data directory {} set with ioh::data::set_root does not exist",
                                                 root.u8string()));
                }
            }

            if (const auto resources = resources_variable(); !resources.empty())
            {
                if (fs::is_directory(resources))
                    return {resources, "from the IOH_RESOURCES environment variable"};
                throw data_error(
                    fmt::format("The IOH data directory {} from the IOH_RESOURCES environment variable does not exist",
                                resources.u8string()));
            }

            // the defaults of the build: the first with the library's data version, so that e.g. a source tree with
            // newer data does not hide the installed copy
            std::vector<fs::path> defaults;
#ifdef IOH_SOURCE_DATA_DIR
            defaults.push_back(fs::u8path(IOH_SOURCE_DATA_DIR));
#endif
#ifdef IOH_INSTALL_DATA_DIR
            defaults.push_back(fs::u8path(IOH_INSTALL_DATA_DIR));
#endif
            std::string looked;
            for (const auto &dir : defaults)
            {
                std::string state;
                if (!fs::is_directory(dir))
                    state = "does not exist";
                else if (const auto version = read_version(dir); version != generated_data_version)
                    state = version.empty() ? "has no data version" : "has data version " + version;
                else
                    return {dir, "the default of the build"};
                looked += fmt::format("{}{} ({})", looked.empty() ? "" : ", ", dir.u8string(), state);
            }

            throw data_error(fmt::format(
                "IOH could not find its data directory (the static folder of IOHexperimenter, data version {}): "
                "neither ioh::data::set_root nor the IOH_RESOURCES environment variable is set{}. Set one of them to "
                "the data directory.",
                generated_data_version,
                looked.empty() ? ", and this build has no default" : ", and no default of the build fits: " + looked));
        }

        //! Checks, once per data directory, that its manifest.json has the data version of the library
        void check_version(const fs::path &dir, const std::string &origin)
        {
            static std::mutex mutex;
            static std::set<std::string> checked;
            const std::lock_guard<std::mutex> lock(mutex);
            if (checked.count(dir.u8string()) != 0)
                return;

            if (const auto version = read_version(dir); version != generated_data_version)
                throw data_error(fmt::format(
                    "The IOH data directory {} ({}) {}, but this build of IOH needs data version {}: use the data "
                    "directory of this version (the static folder of IOHexperimenter)",
                    dir.u8string(), origin,
                    version.empty() ? "has no manifest.json with a data version" : "has data version " + version,
                    generated_data_version));
            checked.insert(dir.u8string());
        }
    } // namespace

    void set_root(const fs::path &path)
    {
        const std::lock_guard<std::mutex> lock(root_mutex);
        explicit_root() = path;
    }

    fs::path root() { return find_root().first; }

    fs::path file(const fs::path &relative)
    {
        const auto [dir, origin] = find_root();
        check_version(dir, origin);
        auto path = dir / relative;
        if (!fs::exists(path))
            throw data_error(fmt::format("IOH data file {} is missing from the data directory {} ({})",
                                         relative.u8string(), dir.u8string(), origin));
        return path;
    }

    std::vector<ManifestEntry> manifest_entries(const std::string &family)
    {
        std::vector<ManifestEntry> entries;
        for (const auto &e : generated_entries)
            if (family == e.family)
                entries.push_back({e.id, e.name, e.meta_name, e.problem_class, e.definition});
        return entries;
    }

    bool in_manifest(const fs::path &relative)
    {
        const auto path = relative.generic_string();
        return std::any_of(std::begin(generated_files), std::end(generated_files),
                           [&path](const char *file) { return path == file; });
    }

    std::string data_version() { return generated_data_version; }
} // namespace ioh::data
