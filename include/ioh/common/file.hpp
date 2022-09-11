
#pragma once

#include <fmt/format.h>



#include <string>
#include <utility>
#include "ioh/common/log.hpp"

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

#include <nlohmann/json.hpp>
using json = nlohmann::json;

//! File namespace
namespace ioh::common::file
{

    namespace utils
    {
        /**
         * @brief Convert a string to a integral type
         *
         * @tparam T the type to cast to
         * @param s the input string
         * @return std::enable_if<std::is_integral<T>::value, T>::type
         */
        template <typename T>
        inline typename std::enable_if<std::is_integral<T>::value, T>::type from_string(const std::string &s)
        {
            return {std::stoi(s)};
        }

        /**
         * @brief Convert a string to a floating point type
         *
         * @tparam T the type to cast to
         * @param s the input string
         * @return std::enable_if<std::is_integral<T>::value, T>::type
         */
        template <typename T>
        inline typename std::enable_if<std::is_floating_point<T>::value, T>::type from_string(const std::string &s)
        {
            return static_cast<T>(std::stod(s));
        }


        /**
         * @brief Helper class to extract lines with istream_iterator
         *
         */
        class Line
        {
            std::string data;

        public:
            friend std::istream &operator>>(std::istream &is, Line &l)
            {
                while (std::getline(is, l.data) && l.data.empty())
                    ;
                return is;
            }
            operator std::string() const { return data; }
        };

        /**
         * @brief Get the absolute path of IOHexperimenter/static
         *
         * @return fs::path the absolute path of IOHexperimenter/static
         */
        inline fs::path get_static_root()
        {
            const auto static_root = fs::path("IOHexperimenter") / fs::path("static");
            fs::path root;
            for (const auto &e : fs::current_path())
            {
                root /= e;
                if (exists(root / static_root))
                {
                    root = root / static_root;
                    return root;
                }
            }
            IOH_DBG(warning, "could static root");
            return {};
        }


        /**
         * @brief Finds a file located in the static folder of this repository
         *
         * @param filename the name of the file
         * @return fs::path the absolute path of the file
         */
        inline fs::path find_static_file(const std::string &filename)
        {
            auto file = get_static_root() / filename;

            if (!exists(file))
            {
                IOH_DBG(warning, "could not find file: " << filename);
                return {};
            }
            return file;
        }


        //! Helper type for detecting shared ptr of which the element type can be constructed from a string
        template <typename T>
        struct is_shared_ptr_string_constructible : std::false_type
        {
        };
        template <typename T>
        struct is_shared_ptr_string_constructible<std::shared_ptr<T>> : std::is_constructible<T, std::string>
        {
        };

        

    } // namespace utils


    /**
     * @brief Merge the tmp_dat_file_path to tar_dat_file_path
     * 
     * @param tmp_dat_file_path 
     * @param tar_dat_file_path 
     */
    inline void merge_dat_file(const std::string tmp_dat_file_path, const std::string tar_dat_file_path) {
        std::ofstream tar_dat_file(tar_dat_file_path,std::ios_base::app);
        std::ifstream tmp_dat_file(tmp_dat_file_path);
        std::string tmp_line;
        if (tar_dat_file.is_open() && tmp_dat_file.is_open()) {
            while (getline(tmp_dat_file,tmp_line)) {
                tar_dat_file << tmp_line << std::endl;
            }
        }
        tar_dat_file.close();
        tmp_dat_file.close();
    }

    /**
     * @brief Merge the files in tmp_folder into the target folder
     * 
     * @param tmp_folder 
     * @param target_folder 
     */
    inline void merge_output_to_single_folder(std::string tmp_folder, std::string target_folder) 
    {
        const fs::path tmp_directory_path{tmp_folder};
        const fs::path target_directory_path{target_folder};

        // std::vector<fs::path> tmp_sub_directories;
        // std::vector<fs::path> tmp_sub_info;
        
        // ! Getting the file name directory names
        for (auto const& dir_entry : fs::directory_iterator{tmp_directory_path}) 
        {
            if(!fs::is_directory(dir_entry)) {
                std::string tmp_file_string = dir_entry.path().filename().string();
                if(tmp_file_string.substr(tmp_file_string.length()-5,tmp_file_string.length()-1) == ".json")
                {
                    std::ifstream tmp(dir_entry.path().string(), std::ifstream::binary);
                    json tmp_data = json::parse(tmp);

                    if(!fs::exists(target_folder + tmp_file_string)) {
                        fs::copy(tmp_folder+tmp_file_string, target_folder+tmp_file_string);
                        std::ifstream tar(target_folder + tmp_file_string);
                        json tar_data = json::parse(tar);
                        auto tmp_scenarios = tmp_data["scenarios"];
                        for (auto tmp_dim_data : tmp_scenarios) {
                            merge_dat_file(tmp_folder+to_string(tmp_dim_data["path"]),target_folder+to_string(tmp_dim_data["path"]));
                        }
                        tar.close();
                    } else {
                        std::ifstream tar(target_folder + tmp_file_string);
                        json tar_data = json::parse(tar);
                        auto tmp_scenarios = tmp_data["scenarios"];
                        for (auto tmp_dim_data : tmp_scenarios) {
                            auto dim = tmp_dim_data["dimension"];
                            auto tmp_run_data = tmp_dim_data["runs"];
                            // std::cout << tmp_ins_data << std::endl;
                            for (size_t i = 0; i != tar_data["scenarios"].size(); ++i) {
                                for (auto tmp_ins_data : tmp_run_data) {
                                    if(tar_data["scenarios"].at(i)["dimension"] == dim) {
                                    tar_data["scenarios"][i]["runs"] += tmp_ins_data;
                                    }
                                }
                            }
                            auto path = to_string(tmp_dim_data["path"]);
                            path.erase(remove(path.begin(), path.end(), '"'), path.end()); 
                            merge_dat_file(tmp_folder+path,target_folder+path);
                        }
                        tar.close();

                        std::ofstream tar_json(target_folder + dir_entry.path().filename().string());
                        tar_json << tar_data;
                        tar_json.close();
                    }
                }
            }   
        }
    }
    /**
     * @brief Get the file contents as string object
     *
     * @param path the path of the file
     * @return std::string the contents of the file
     */
    inline std::string as_string(const fs::path &path)
    {
        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    }


    /**
     * @brief Extracts all numbers from a file and converts them to a given numeric type T
     *
     * @tparam T the numeric type
     * @tparam std::enable_if<std::is_arithmetic<T>::value, T>::type
     * @param path The path of the
     * @return std::vector<T> all the numbers in the file given by path
     */
    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    inline std::vector<T> as_numeric_vector(const fs::path &path)
    {
        std::vector<T> result;
        std::ifstream t(path);
        std::transform(std::istream_iterator<std::string>(t), std::istream_iterator<std::string>(),
                       std::back_inserter(result), utils::from_string<T>);
        return result;
    }


    /**
     * @brief Get the file contents as a string vector per line
     * example usage:
     * @code
     * struct Person{
     *      std::string name;
     *      Person(const std::string& name): name(name) {}
     * };
     * std::vector<Person> persons = as_text_vector<Person>(file_with_names);
     *
     * @tparam T the type of the output vector, should be instantiatable from a std::string
     * @param path the path of the file
     * @return std::vector<T> the contents of the file
     */
    template <typename T = std::string,
              typename std::enable_if<std::is_constructible<T, std::string>::value, T>::type* = nullptr>
    std::vector<T> as_text_vector(const fs::path &path)
    {
        std::istringstream in(as_string(path));
        std::vector<T> result;

        std::transform(std::istream_iterator<utils::Line>(in), std::istream_iterator<utils::Line>(),
                       std::back_inserter(result),
                       [](const utils::Line &line) { return static_cast<std::string>(line); });
        return result;
    }

    // TODO: make use templates for this function
    // example SFINAI https://stackoverflow.com/questions/62264951/handle-logical-or-for-predicates-in-stdenable-if
    // Something like: 
    // template <
    //         typename T,
    //         typename = typename std::enable_if<is_shared_ptr<T>::value &&
    //                                            std::is_constructible<typename T::element_type, std::string>::value>::value>
    // The above fails because of substitutions with types that don't have T::element_type
    template <typename T,
              typename std::enable_if<utils::is_shared_ptr_string_constructible<T>::value, T>::type* = nullptr>
    std::vector<T> as_text_vector(const fs::path &path)
    {
        std::istringstream in(as_string(path));
        std::vector<T> result;

        std::transform(std::istream_iterator<utils::Line>(in), std::istream_iterator<utils::Line>(),
                       std::back_inserter(result),
                       [](const utils::Line &line) { return std::make_shared<typename T::element_type>(static_cast<std::string>(line)); });
        return result;
    }

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
