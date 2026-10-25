#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef FSEXPERIMENTAL
#define JSON_HAS_EXPERIMENTAL_FILESYSTEM 1
#undef JSON_HAS_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#undef JSON_HAS_EXPERIMENTAL_FILESYSTEM
#define JSON_HAS_FILESYSTEM 1
#include <filesystem>
namespace fs = std::filesystem;
#endif

//! Where the library finds its data files: the static folder of IOHexperimenter
namespace ioh::data
{
    //! The data directory or a data file is missing
    struct data_error : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Sets the data directory, which comes before the IOH_RESOURCES environment variable and the defaults of
     * the build (see root). An empty path unsets it.
     *
     * @param path the data directory
     */
    void set_root(const fs::path &path);

    /**
     * @brief The data directory: the one given to set_root, otherwise the IOH_RESOURCES environment variable,
     * otherwise the first existing of the build's defaults: the static folder of the source tree it was built from,
     * and share/ioh of its installation (the Python package sets its own). Throws data_error, saying where it looked
     * and how to set it, when the directory given by set_root or IOH_RESOURCES does not exist, or none is set and no
     * default exists.
     */
    fs::path root();

    /**
     * @brief The path of a data file. Throws data_error when it does not exist, when the data directory does not have
     * the data version of the library (see data_version), or see root.
     *
     * @param relative the path of the file relative to the data directory
     */
    fs::path file(const fs::path &relative);

    //! A registry entry defined by data, from the manifest (static/manifest.json) the library is built with
    struct ManifestEntry
    {
        //! The id in the registry
        int id;

        //! The name in the registry
        std::string name;

        //! The name the problem reports (for the CEC2013 problems it differs from the registry name)
        std::string meta_name;

        //! The class of a graph problem: MaxCut, MaxCoverage, MaxInfluence or PackWhileTravel
        std::string problem_class;

        //! The definition of a graph problem's graph (its files)
        std::string definition;

        //! The dimension the problem always uses
        int fixed_dimension;

        //! The instance the problem always uses
        int fixed_instance;
    };

    //! The entries of a problem family in the manifest: GraphProblem or CEC2013
    std::vector<ManifestEntry> manifest_entries(const std::string &family);

    //! Whether the manifest lists a data file
    bool in_manifest(const fs::path &relative);

    //! The data version the library is built with, which the manifest.json of the data directory must have
    std::string data_version();

    namespace detail
    {
        //! The values that cached keeps for one type
        template <typename T>
        struct Cache
        {
            //! A value, and the lock under which it is loaded
            struct Slot
            {
                std::mutex mutex;
                std::shared_ptr<const T> value;
            };

            static inline std::mutex mutex;
            static inline std::map<std::string, std::shared_ptr<Slot>> slots;
        };
    } // namespace detail

    /**
     * @brief The value that loader returns for key, loaded once and shared: the first call with a key calls loader
     * (once, also when several threads ask at the same time), later calls get the same object. If loader throws,
     * the next call tries again. The values of each type T are kept until the program ends.
     *
     * @param key the key, e.g. the path of a data file (see file)
     * @param loader the function that loads the value
     */
    template <typename T, typename Loader>
    std::shared_ptr<const T> cached(const std::string &key, Loader &&loader)
    {
        using Cache = detail::Cache<T>;
        std::shared_ptr<typename Cache::Slot> slot;
        {
            const std::lock_guard<std::mutex> lock(Cache::mutex);
            auto &s = Cache::slots[key];
            if (!s)
                s = std::make_shared<typename Cache::Slot>();
            slot = s;
        }
        const std::lock_guard<std::mutex> lock(slot->mutex);
        if (!slot->value)
            slot->value = std::make_shared<const T>(loader());
        return slot->value;
    }
} // namespace ioh::data
