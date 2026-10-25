#pragma once

#include <stdexcept>
#include <string>

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
     * @brief The path of a data file. Throws data_error when it does not exist (or see root).
     *
     * @param relative the path of the file relative to the data directory
     */
    fs::path file(const fs::path &relative);
} // namespace ioh::data
