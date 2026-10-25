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
            int fixed_dimension;
            int fixed_instance;
        };

#include "data_manifest.inc"

        std::mutex root_mutex;
        fs::path explicit_root;

        //! The data directory and where it came from
        std::pair<fs::path, std::string> find_root()
        {
            {
                const std::lock_guard<std::mutex> lock(root_mutex);
                if (!explicit_root.empty())
                {
                    if (fs::is_directory(explicit_root))
                        return {explicit_root, "set with ioh::data::set_root"};
                    throw data_error(fmt::format("The IOH data directory {} set with ioh::data::set_root does not exist",
                                                 explicit_root.string()));
                }
            }

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) // getenv
#endif
            const char *resources = std::getenv("IOH_RESOURCES");
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
            if (resources != nullptr && *resources != '\0')
            {
                if (fs::is_directory(resources))
                    return {fs::path(resources), "from the IOH_RESOURCES environment variable"};
                throw data_error(fmt::format(
                    "The IOH data directory {} from the IOH_RESOURCES environment variable does not exist", resources));
            }

            std::vector<std::string> defaults;
#ifdef IOH_SOURCE_DATA_DIR
            defaults.emplace_back(IOH_SOURCE_DATA_DIR);
#endif
#ifdef IOH_INSTALL_DATA_DIR
            defaults.emplace_back(IOH_INSTALL_DATA_DIR);
#endif
            std::string looked;
            for (const auto &dir : defaults)
            {
                if (fs::is_directory(dir))
                    return {fs::path(dir), "the default of the build"};
                looked += (looked.empty() ? "" : ", ") + dir;
            }

            throw data_error(fmt::format(
                "IOH could not find its data directory (the static folder of IOHexperimenter): neither "
                "ioh::data::set_root nor the IOH_RESOURCES environment variable is set{}. Set one of them to the data "
                "directory.",
                looked.empty() ? ", and this build has no default"
                               : ", and the defaults of the build (" + looked + ") do not exist"));
        }

        //! Checks, once per data directory, that its manifest.json has the data version of the library
        void check_version(const fs::path &dir, const std::string &origin)
        {
            static std::mutex mutex;
            static std::set<std::string> checked;
            const std::lock_guard<std::mutex> lock(mutex);
            if (checked.count(dir.string()) != 0)
                return;

            std::string version;
            if (std::ifstream in(dir / "manifest.json"); in)
            {
                const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                const std::string key = "\"data_version\": \"";
                if (const auto start = text.find(key); start != std::string::npos)
                    version = text.substr(start + key.size(), text.find('"', start + key.size()) - start - key.size());
            }
            if (version != generated_data_version)
                throw data_error(fmt::format(
                    "The IOH data directory {} ({}) {}, but this build of IOH needs data version {}: use the data "
                    "directory of this version (the static folder of IOHexperimenter)",
                    dir.string(), origin,
                    version.empty() ? "has no manifest.json with a data version" : "has data version " + version,
                    generated_data_version));
            checked.insert(dir.string());
        }
    } // namespace

    void set_root(const fs::path &path)
    {
        const std::lock_guard<std::mutex> lock(root_mutex);
        explicit_root = path;
    }

    fs::path root() { return find_root().first; }

    fs::path file(const fs::path &relative)
    {
        const auto [dir, origin] = find_root();
        check_version(dir, origin);
        auto path = dir / relative;
        if (!fs::exists(path))
            throw data_error(fmt::format("IOH data file {} is missing from the data directory {} ({})",
                                         relative.string(), dir.string(), origin));
        return path;
    }

    std::vector<ManifestEntry> manifest_entries(const std::string &family)
    {
        std::vector<ManifestEntry> entries;
        for (const auto &e : generated_entries)
            if (family == e.family)
                entries.push_back(
                    {e.id, e.name, e.meta_name, e.problem_class, e.definition, e.fixed_dimension, e.fixed_instance});
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
