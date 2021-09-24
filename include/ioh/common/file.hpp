#pragma once

#include <string>
#include <utility>
#include <fmt/format.h>

#ifdef FSEXPERIMENTAL
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

//! File namespace
namespace ioh::common::file
{

    /**
     * @brief Item on the filesystem
     * 
     */
    class FileSystemItem
    {
    protected:
        //! the directory containing the item
        fs::path root_;

        //! name of the item
        std::string name_;

        //! The full path of the file
        fs::path path_;

    public:
        FileSystemItem() {}

        /**
         * @brief Construct a new File System Item object
         * 
         * @param root the directory containing the item
         * @param name name of the item
         */
        FileSystemItem(fs::path root, const std::string &name) :
            root_(std::move(root)), name_(name), path_(root_ / name)
        {
        }

        //! Set the path of the item
        void path(const fs::path &path)
        {
            root_ = path.parent_path();
            name_ = (path.filename() != "" ? path.filename() : path.stem()).generic_string();
            path_ = path;
        }

        //! Accessor for `path_`
        [[nodiscard]] fs::path path() const { return path_; }

        //! Accessor for `root_`
        [[nodiscard]] fs::path root() const { return root_; }

        //! Accessor for `name_`
        [[nodiscard]] std::string name() const { return name_; }

        //! Remove the item on the filesystem
        void remove() const { remove_all(path_); }

        //! Join the path
        fs::path operator/(const std::string &p) const { return path() / p; }
        
        //! Join the path
        fs::path operator/(const fs::path &p) const { return path() / p; }
    };
    
    /**
     * @brief Creates a new folder with a unique name
     * 
     */
    class UniqueFolder : public FileSystemItem
    {
    public:
        /**
         * @brief Construct a new Unique Folder object.
         * 
         * @param root the directory containing the item
         * @param name name of the folder
         */
        explicit UniqueFolder(fs::path root, const std::string &name) : FileSystemItem(std::move(root), name)
        {
            if (!exists(root_))
                create_directories(root_);

            path_ = root_ / name;
            auto index = 0;
            while (exists(path_))
            {
                ++index;
                name_ = fmt::format("{}-{}", name, index);
                path_ = root_ / name_;
            }
            create_directories(path_);
        }
        //! Construct a new Unique Folder object.
        explicit UniqueFolder(const std::string &name) : UniqueFolder(fs::current_path(), name) {}

        //! Construct a new Unique Folder object.
        explicit UniqueFolder() {}
    };
} // namespace ioh::common::file
