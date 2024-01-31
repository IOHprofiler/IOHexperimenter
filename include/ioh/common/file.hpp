
#pragma once
#include <string>
#include <utility>
#include "ioh/common/format.hpp"
#include "ioh/common/log.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <streambuf>
#include <thread>


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

#ifdef HAS_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif
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
            //! Data container
            std::string data;

        public:
            //! Stream operator
            friend std::istream &operator>>(std::istream &is, Line &l)
            {
                while (std::getline(is, l.data) && l.data.empty())
                    ;
                return is;
            }
            //! String convertor
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
    inline void merge_dat_file(const std::string tmp_dat_file_path, const std::string tar_dat_file_path)
    {
        std::ofstream tar_dat_file(tar_dat_file_path, std::ios_base::app);
        std::ifstream tmp_dat_file(tmp_dat_file_path);
        std::string tmp_line;
        if (tar_dat_file.is_open() && tmp_dat_file.is_open())
        {
            while (getline(tmp_dat_file, tmp_line))
            {
                tar_dat_file << tmp_line << std::endl;
            }
        }
        tar_dat_file.close();
        tmp_dat_file.close();
    }

#ifdef HAS_JSON
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
        for (auto const &dir_entry : fs::directory_iterator{tmp_directory_path})
        {
            if (!fs::is_directory(dir_entry))
            {
                std::string tmp_file_string = dir_entry.path().filename().string();
                if (tmp_file_string.substr(tmp_file_string.length() - 5, tmp_file_string.length() - 1) == ".json")
                {
                    std::ifstream tmp(dir_entry.path().string(), std::ifstream::binary);
                    json tmp_data = json::parse(tmp);

                    if (!fs::exists(target_folder + tmp_file_string))
                    {
                        fs::copy(tmp_folder + tmp_file_string, target_folder + tmp_file_string);
                        std::ifstream tar(target_folder + tmp_file_string);
                        json tar_data = json::parse(tar);
                        auto tmp_scenarios = tmp_data["scenarios"];
                        for (auto tmp_dim_data : tmp_scenarios)
                        {
                            merge_dat_file(tmp_folder + to_string(tmp_dim_data["path"]),
                                           target_folder + to_string(tmp_dim_data["path"]));
                        }
                        tar.close();
                    }
                    else
                    {
                        std::ifstream tar(target_folder + tmp_file_string);
                        json tar_data = json::parse(tar);
                        auto tmp_scenarios = tmp_data["scenarios"];
                        for (auto tmp_dim_data : tmp_scenarios)
                        {
                            auto dim = tmp_dim_data["dimension"];
                            auto tmp_run_data = tmp_dim_data["runs"];
                            for (size_t i = 0; i != tar_data["scenarios"].size(); ++i)
                            {
                                for (auto tmp_ins_data : tmp_run_data)
                                {
                                    if (tar_data["scenarios"][i]["dimension"] == dim)
                                    {
                                        tar_data["scenarios"][i]["runs"] += tmp_ins_data;
                                    }
                                }
                            }
                            auto path = to_string(tmp_dim_data["path"]);
                            path.erase(remove(path.begin(), path.end(), '"'), path.end());
                            merge_dat_file(tmp_folder + path, target_folder + path);
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
#endif // DEBUG

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
     * @endcode
     * @tparam T the type of the output vector, should be instantiatable from a std::string
     * @param path the path of the file
     * @return std::vector<T> the contents of the file
     */
    template <typename T = std::string,
              typename std::enable_if<std::is_constructible<T, std::string>::value, T>::type * = nullptr>
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
    //                                            std::is_constructible<typename T::element_type,
    //                                            std::string>::value>::value>
    // The above fails because of substitutions with types that don't have T::element_type
    template <typename T,
              typename std::enable_if<utils::is_shared_ptr_string_constructible<T>::value, T>::type * = nullptr>
    std::vector<T> as_text_vector(const fs::path &path)
    {
        std::istringstream in(as_string(path));
        std::vector<T> result;

        std::transform(std::istream_iterator<utils::Line>(in), std::istream_iterator<utils::Line>(),
                       std::back_inserter(result), [](const utils::Line &line) {
                           return std::make_shared<typename T::element_type>(static_cast<std::string>(line));
                       });
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


    inline FILE *open_file(const fs::path &path)
    {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
        return fopen(path.generic_string().c_str(), "ab");
#pragma warning(pop)
#else
        return fopen(path.generic_string().c_str(), "ab");
#endif
    }

    struct Writer
    {
        virtual void open(const fs::path &) = 0;
        virtual void close() = 0;
        virtual bool is_open() = 0;
        virtual void write(const std::string &) = 0;
    };

    struct NoWriter: Writer{
        bool is_open_;
        size_t chars_written;
        NoWriter(): is_open_(false), chars_written(0) {}

        void open(const fs::path &) override {
            is_open_ = true;
        }
        void close() override {
            is_open_ = false;
        }
        bool is_open() override {
            return is_open_;
        }
        void write(const std::string &s) override {
            chars_written += s.size();
        }
    };


    struct OFStream : Writer
    {
        std::ofstream out_;
        fs::path path_;
        OFStream() : out_{}, path_{} {}

        virtual ~OFStream() { close(); }

        void open(const fs::path &new_path) override
        {
            close();
            path_ = new_path;
            out_.open(new_path);
        };
        void close() override
        {
            if (is_open())
            {
                out_.close();
            }
        };
        bool is_open() override { return out_.is_open(); };
        void write(const std::string &s) override { out_ << s; };
    };

    struct FWriter : Writer
    {
        fs::path path;
        FILE *file_ptr;

        size_t buffer_size;
        int buffer_mode;

        FWriter(size_t buffer_size = 0, int buffer_mode = _IOFBF) :
            path{}, file_ptr{nullptr}, buffer_size{buffer_size}, buffer_mode{buffer_mode}
        {
        }

        void open(const fs::path &new_path) override
        {
            close();
            path = new_path;
            file_ptr = open_file(path);

            if (file_ptr == NULL)
            {
                std::perror("creating cachedfile");
                return;
            }
            if (buffer_size != 0){
                if (std::setvbuf(file_ptr, nullptr, buffer_mode, buffer_size) != 0)
                    std::perror("setvbuf failed");
            }
#ifndef _MSC_VER
            else
            {
                struct stat stats;
                if (fstat(fileno(file_ptr), &stats) != -1) 
                    if (std::setvbuf(file_ptr, nullptr, _IOFBF, stats.st_blksize) != 0)
                        std::perror("setvbuf failed"); 
            }
#endif
        }

        void close() override
        {
            if (is_open())
            {
                close_file();
                path.clear();
            }
        }

        virtual void close_file()
        {
            fclose(file_ptr);
            file_ptr = nullptr;
        }

        bool is_open() override { return file_ptr != nullptr; }

        virtual ~FWriter() { close(); }


        virtual void write(const char *data, const size_t data_size)
        {
            const auto ret = fwrite(data, sizeof(char), data_size, file_ptr);
            if (ret != data_size)
                std::perror("writing fwrite");
        }

        void write(const std::string &s) override { write(s.data(), s.size()); }
    };

    struct CachedFWriter : FWriter
    {
        std::vector<char> buffer_;
        size_t buffered_elements_;
        size_t buffer_size_;
        char* start;
        CachedFWriter(const size_t buffer_size = 4096) : 
            FWriter{}, buffer_(buffer_size), buffered_elements_{0}, buffer_size_{buffer_size}, start(buffer_.data())
        {
        }

        void close() override
        {
            if (is_open())
            {
                write_chunk();
                close_file();
                buffered_elements_ = 0;
                path.clear();
            }
        }
        void open(const fs::path &new_path) override
        {
            close();
            path = new_path;
            file_ptr = open_file(path);

            if (file_ptr == NULL)
            {
                std::perror("creating cachedfile");
                return;
            }
            std::setvbuf(file_ptr, nullptr, _IONBF, 0);
        }

        virtual ~CachedFWriter() { close(); }

        virtual void write_chunk()
        {
            const auto ret = fwrite(start, sizeof(char), buffered_elements_, file_ptr);
            if (ret != buffered_elements_)
                std::perror("writing cached file");
            buffered_elements_ = 0;
        }

        void write(const char *data, const size_t data_size) override
        {
            size_t bytes_written = 0;
            while (bytes_written < data_size)
            {
                const size_t space = buffer_size_ - buffered_elements_;
                const size_t required = data_size - bytes_written;
                const size_t to_write = std::min(space, required);

                std::copy(data + bytes_written, data + bytes_written + to_write, start + buffered_elements_);
                bytes_written += to_write;
                buffered_elements_ += to_write;
                if (buffered_elements_ == buffer_size_)
                    write_chunk();
            }
        }
    };

#if !(defined(_MSC_VER) || defined(__APPLE__))
    struct DirectIOWriter: CachedFWriter {
        int fileno;

        DirectIOWriter(const size_t buffer_size = 4096): 
            CachedFWriter(buffer_size),
            fileno(-1)
        {
            size_t offset = 512 - ((size_t)start % 512);
            buffer_.resize(buffer_.size() + offset);
            start += offset;
        }

        void open(const fs::path &new_path) override
        {
            close();
            path = new_path;
            fileno = ::open(path.generic_string().c_str(), 
                O_CREAT | O_WRONLY | O_APPEND | O_DIRECT, 0600
            );

            if (fileno == -1)
            {
                std::perror("creating cachedfile");
                return;
            }
        }
        bool is_open() override { return fileno != -1; }

        virtual ~DirectIOWriter() { close(); }

        void close() override
        {
            if (is_open())
            {
                close_file();
                buffered_elements_ = 0;
                fileno = -1;
                path.clear();
            }
        }

        void close_file() override {
            ::close(fileno);
            
            file_ptr = open_file(path);
            CachedFWriter::write_chunk();
            fclose(file_ptr);   
            file_ptr = nullptr;       
        }

        void write_chunk() override 
        {
            const size_t ret = ::write(fileno, start, buffered_elements_);
            if (ret != buffered_elements_)
                std::perror("writing cached file");
            buffered_elements_ = 0;
        }
    };
#else
    using DirectIOWriter = CachedFWriter;
#endif


    
    // https://stackoverflow.com/questions/21126950/asynchronously-writing-to-a-file-in-c-unix
    struct async_buf : std::streambuf
    {
        std::ofstream out;
        std::mutex mutex;
        std::condition_variable condition;
        std::queue<std::vector<char>> queue;
        std::vector<char> buffer;
        bool done;
        size_t buffer_size_;
        std::thread thread;

        void worker()
        {
            bool local_done(false);
            std::vector<char> buf;
            while (!local_done)
            {
                {
                    std::unique_lock<std::mutex> guard(this->mutex);
                    this->condition.wait(guard, [this]() { return !this->queue.empty() || this->done; });
                    if (!this->queue.empty())
                    {
                        buf.swap(queue.front());
                        queue.pop();
                    }
                    local_done = this->queue.empty() && this->done;
                }
                if (!buf.empty())
                {
                    out.write(buf.data(), std::streamsize(buf.size()));
                    buf.clear();
                }
            }
            out.flush();
        } 

    public:
        async_buf(std::string const &name, const size_t buffer_size, const bool disable_local_buffering) :
            out(name), buffer(buffer_size), done(false), buffer_size_(buffer_size),
            thread(&async_buf::worker, this)
        {
            if (disable_local_buffering)
                out.rdbuf()->pubsetbuf(nullptr, 0);

            this->setp(this->buffer.data(), this->buffer.data() + this->buffer.size() - 1);

        }
        ~async_buf()
        {
            #ifdef _MSC_VER
            #pragma warning(push)
            #pragma warning(disable : 4834)
            std::unique_lock<std::mutex>(this->mutex), (this->done = true);
            #endif 
            this->condition.notify_one();
            this->thread.join();
        }
        int overflow(int c)
        {
            if (c != std::char_traits<char>::eof())
            {
                *this->pptr() = std::char_traits<char>::to_char_type(c);
                this->pbump(1);
            }
            return this->sync() != -1 ? std::char_traits<char>::not_eof(c) : std::char_traits<char>::eof();
        }
        int sync()
        {
            if (this->pbase() != this->pptr())
            {
                this->buffer.resize(std::size_t(this->pptr() - this->pbase()));
                {
                    std::unique_lock<std::mutex> guard(this->mutex);
                    this->queue.push(std::move(this->buffer));
                }
                this->condition.notify_one();
                this->buffer = std::vector<char>(buffer_size_);
                this->setp(this->buffer.data(), this->buffer.data() + this->buffer.size() - 1);
            }
            return 0;
        }
    };

    struct AsyncWriter : Writer
    {
        async_buf* buffer_;    
        std::ostream* out_;
        fs::path path_;
        size_t buffer_size_;
        bool disable_local_buffering_;

        AsyncWriter(const size_t buffer_size = 128, const bool disable_local_buffering = false) :
            buffer_(nullptr), out_(nullptr), path_{}, buffer_size_(buffer_size),
            disable_local_buffering_(disable_local_buffering)
        {
        }

        virtual ~AsyncWriter() { close(); }

        void open(const fs::path &new_path) override
        {
            close();
            buffer_ = new async_buf(new_path.generic_string(), buffer_size_, disable_local_buffering_);
            out_ = new std::ostream(buffer_);
            path_ = new_path;
        };
        void close() override
        {
            if (is_open())
            {
                out_->flush();
                delete out_;
                out_ = nullptr;
                delete buffer_;
                buffer_ = nullptr;
            }
        };
        bool is_open() override { return out_ != nullptr; };
        void write(const std::string &s) override { (*out_) << s; };
    };



} // namespace ioh::common::file
