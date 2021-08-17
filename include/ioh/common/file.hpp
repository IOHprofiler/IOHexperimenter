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


namespace ioh::common::file
{

    class FileSystemItem
    {
    protected:
        fs::path root_;
        std::string name_;
        fs::path path_;

    public:
        FileSystemItem() {}

        FileSystemItem(fs::path root, const std::string &name) :
            root_(std::move(root)), name_(name), path_(root_ / name)
        {
        }

        void path(const fs::path &path)
        {
            root_ = path.parent_path();
            name_ = (path.filename() != "" ? path.filename() : path.stem()).generic_string();
            path_ = path;
        }

        [[nodiscard]] fs::path path() const { return path_; }

        [[nodiscard]] fs::path root() const { return root_; }

        [[nodiscard]] std::string name() const { return name_; }

        void remove() const { remove_all(path_); }

        fs::path operator/(const std::string &p) const { return path() / p; }

        fs::path operator/(const fs::path &p) const { return path() / p; }
    };

    class UniqueFolder : public FileSystemItem
    {
    public:
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

        explicit UniqueFolder(const std::string &name) : UniqueFolder(fs::current_path(), name) {}

        explicit UniqueFolder() {}
    };
} // namespace ioh::common::file
