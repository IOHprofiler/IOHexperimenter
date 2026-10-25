#include "ioh/common/data.hpp"

#include <cstdlib>
#include <mutex>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace ioh::data
{
    namespace
    {
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
        auto path = dir / relative;
        if (!fs::exists(path))
            throw data_error(fmt::format("IOH data file {} is missing from the data directory {} ({})",
                                         relative.string(), dir.string(), origin));
        return path;
    }
} // namespace ioh::data
