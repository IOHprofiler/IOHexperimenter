#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#if __cplusplus == 202002L
#include <chrono>
#else
#include <ctime>
#endif

#include <cxxopts.hpp>
#include <clutchlog/clutchlog.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <ioh.hpp>

//! Error codes returned on exit.
enum class Error : unsigned char {
    No_Error = 0,
    No_File = 2, // ENOENT
    Invalid_Argument = 22, // EINVAL
    Not_FIFO = 60, // ENOSTR
    Unreadable = 77, // EBADFD
    Missing_Argument = 132,
    Payload_Parse,
    Payload_Type,
    Payload_Incomplete,
    Dimension_Mismatch,
    Not_Supported,
    Unknown = 255
};

//! Macro to hide the necessary verbose casting.
#ifdef WITH_CLUTCHLOG
    #define EXIT_ON_ERROR(IOH_server_err_code, IOH_server_msg) { \
        if(static_cast<unsigned char>(Error::IOH_server_err_code) > 0) { \
            CLUTCHLOG(critical, IOH_server_msg); \
        } \
        exit(static_cast<unsigned char>(Error::IOH_server_err_code)); \
    }
#else
    #define EXIT_ON_ERROR(IOH_server_err_code, IOH_server_msg) { \
        if(static_cast<unsigned char>(Error::IOH_server_err_code) > 0) { \
            std::cerr << "ERROR: " << IOH_server_msg << std::endl; \
        } \
        exit(static_cast<unsigned char>(Error::IOH_server_err_code)); \
    }
#endif

#define EXIT(IOH_server_err_code) exit(static_cast<unsigned char>(Error::IOH_server_err_code))

//! Strip spaces around the input string.
std::string strip(std::string s)
{
    s.erase(std::find_if( s.rbegin(), s.rend(),
            [](int ch) { return !std::isspace(ch); }
        ).base(), s.end());
    return s;
}

//! Split a string in a vector.
template<class T>
std::vector<T> split(const std::string& str, char delim = ',') {
    std::vector<T> vec;
    size_t start;
    size_t end = 0;
    while( (start = str.find_first_not_of(delim, end)) != std::string::npos ) {
        end = str.find(delim, start);
        std::string sval = strip(str.substr(start, end - start));
        T val = static_cast<T>(std::stol(sval));
        vec.push_back(val);
    }
    return vec;
}

//! Write a message in a file.
void send(const std::string& filename, const std::string& msg)
{
    CLUTCHLOG(debug, "Send response...");
    std::ofstream ofs( filename );
    if(ofs.fail()) {
        EXIT_ON_ERROR(Unreadable, "Output fifo named pipe cannot be read.");
    }
    ofs << msg << std::endl;
    ofs.close();
    CLUTCHLOG(xdebug, "Sent: `" << msg << "`.");
}

//! Read a message in a file.
std::string read(const std::string& filename)
{
    CLUTCHLOG(debug, "Wait for query... ");
    std::ifstream ifs( filename );
    if(ifs.fail()) {
        EXIT_ON_ERROR(Unreadable, "Input fifo named pipe cannot be written.");
    }
    std::stringstream datas;
    datas << ifs.rdbuf();
    ifs.close();
    std::string data = strip(datas.str());
    CLUTCHLOG(xdebug, "Received: `" << data << "`");
    return data;
}

std::string timestamp()
{
#if __cplusplus == 202002L
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%FT%TZ}", now);
#else
    time_t now;
    time(&now);
    char buf[sizeof "2021-12-21T22:32:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    // If a  compiler doesn't support %F or %T, we can fallback to:
    //strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
#endif
}

json forge_default()
{
    json j = {
        {"remarks", "ioh-server v0.1.0"},
        {"timestamp", timestamp()}
    };
    return j;
}

json forge_ack()
{
    json jreply = forge_default();
    jreply["reply_type"] = "ack";
    return jreply;
}

json forge_error(const std::string msg, const Error code = Error::Unknown)
{
    json jreply = forge_default();
    jreply["reply_type"] = "error";
    jreply["message"] = msg;
    jreply["code"] = code;
    return jreply;
}

template<class T>
json forge_value(const double val, const std::vector<T> sol)
{
    json jreply = forge_default();
    jreply["reply_type"] = "value";
    jreply["value"] = val;
    jreply["solution"] = sol;
    return jreply;

}

//! Send back an error message to the client and log it.
void error(const std::string& filename, const std::string& msg, const Error code = Error::Unknown)
{
    CLUTCHLOG(error, msg);
    json jquery = forge_error(msg, code);
    send(filename, jquery.dump());
}

template<class T>
std::shared_ptr<T> create( const cxxopts::ParseResult& asked)
{
    const auto& factory = ioh::problem::ProblemRegistry<T>::instance();

    /**** Now that we have a factory, we can list the available problems. *****/
    if(asked["help-problems"].as<bool>()) {
        std::cout << "Available problems:" << std::endl;
        for(const auto& name : factory.names()) {
            std::cout << "\t" << name << std::endl;
        }
        EXIT(No_Error);
    }
    std::shared_ptr<T> problem = factory.create( asked["problem"].as<std::string>(), asked["instance"].as<int>(), asked["dimension"].as<int>() );
    return problem;
}

template<class T>
void call(std::shared_ptr<T> problem, const cxxopts::ParseResult& asked, const json& jquery, const std::string& freply)
{
    std::vector<typename T::Type> sol = jquery["solution"];

    /***** Sanity checks. *****/
    if(static_cast<int>(sol.size()) != asked["dimension"].as<int>()) {
        std::ostringstream msg;
        msg << "Received a solution of size " << sol.size() << ", which does not match the problem's dimension of " << asked["dimension"].as<int>();
        error(freply, msg.str(), Error::Dimension_Mismatch);
        return;
    }
    // TODO check domain constraints.

    CLUTCHLOG(debug, "Call the objective function...");
    double fval = (*problem)(sol);
    CLUTCHLOG(xdebug, "Result: `" << fval << "`.");
                
    json jreply = forge_value<typename T::Type>(fval, sol);
    send(freply, jreply.dump());
}


int main(int argc, char** argv)
{
    /***********************************************************************
     * Command line arguments
     **********************************************************************/

    cxxopts::Options args("ioh-service",
        "Server of IOHexperimenter problems' objective functions.");

    args.add_options()
        /* Main */
        ("input", "Input fifo named pipe.", cxxopts::value<std::string>() )
        ("output", "Output fifo named pipe.", cxxopts::value<std::string>() )
        ("h,help", "Print help")
        /* Problem */
        ("t,type", "Problem type (`integer` or `real`)", cxxopts::value<std::string>()/*->default_value("integer")*/ )
        ("p,problem",  "Problem name",  cxxopts::value<std::string>()->default_value("OneMax") )
        ("i,instance", "Instance identifier", cxxopts::value<int>()->default_value("0") )
        ("d,dimension", "Problem dimension", cxxopts::value<int>()->default_value("1") )
        /* Logging */
        ("f,folder", "Folder in which to store the run data.", cxxopts::value<std::string>()->default_value("ioh_experiment") )
        ("s,solver", "Name of the solver that will do the queries", cxxopts::value<std::string>()->default_value("my_solver") )
        ("m,metadata", "Additional information to store in the data files", cxxopts::value<std::string>()->default_value("") )
        /* Debug */
        ("dbg-level", "Debugging log level (Critical, Error, Warning, Progress, Note, Info, Debug or XDebug)", cxxopts::value<std::string>()->default_value("Warning") )
#if CLUTCHLOG_HAVE_UNIX_SYSINFO == 1
        ("dbg-depth", "Max call stack depth until which to enable logging", cxxopts::value<size_t>()->default_value("999") )
#endif
        ("dbg-file", "Regexp matching the source code file(s) in which to enable debugging log", cxxopts::value<std::string>()->default_value(".*") )
        ("dbg-func", "Regexp matching the source code function(s) in which to enable debugging log", cxxopts::value<std::string>()->default_value(".*") )
        ("help-problems", "List the available problems", cxxopts::value<bool>() )
    ;

    args.parse_positional({"input", "output"});

    const cxxopts::ParseResult asked = args.parse(argc, argv);

    if(asked.count("help")) {
        std::cout << args.help() << std::endl;
        EXIT(No_Error);
    }

    if(not asked.count("type")) {
        EXIT_ON_ERROR(Missing_Argument, "Missing required argument `type` (must be set to `integer` or `real`).");
    }

    /***********************************************************************
     * Logging configuration
     **********************************************************************/

    auto& log = clutchlog::logger();

    log.out(std::clog);
    log.threshold( log.level_of(asked["dbg-level"].as<std::string>()) );

#if CLUTCHLOG_HAVE_UNIX_SYSINFO == 1
    log.depth( asked["dbg-depth"].as<size_t>() );
#endif 

    log.file( asked["dbg-file"].as<std::string>() );
    log.func( asked["dbg-func"].as<std::string>() );

#ifndef NDEBUG
    log.format("[{name}] {level}:{depth_marks} {msg}\t\t\t\t\t{func} @ {file}:{line}\n");
#else
    log.format("[{name}] {level_letter}({depth}): {msg}\n");
#endif

    /***********************************************************************
     * IOH instantiations
     **********************************************************************/

    std::string type = asked["type"].as<std::string>();

    if(not (type == "integer" or type == "real")) {
        EXIT_ON_ERROR(Invalid_Argument, "Unknown problem type: `" << type << "`");
    }

    // FIXME check if IOH can have the necessary abstract interfaces here.
    std::shared_ptr<ioh::problem::Integer> problem_int  = nullptr;
    std::shared_ptr<ioh::problem::Real>    problem_real = nullptr;

    if(type == "integer") {
        problem_int = create<ioh::problem::Integer>(asked);
    } else if(type == "real") {
        problem_real = create<ioh::problem::Real>(asked);
    }

    // TODO parametrize W-model?


    // Continue with IOH instantiations.
    auto logger = ioh::logger::Analyzer(
            {ioh::trigger::on_improvement},
            {}, // additional properties // TODO FIXME find a way to handle (undeclared?) properties from the client.
            fs::current_path(), // Not sure it would be useful to let this be a parameter, as we already have folder and the service is single-expe by design.
            asked["folder"].as<std::string>(),
            asked["solver"].as<std::string>(),
            asked["metadata"].as<std::string>()
        );


    if(type == "integer") {
        problem_int->attach_logger(logger);
    } else if(type == "real") {
        problem_real->attach_logger(logger);
    }

    /***********************************************************************
     * Sanity checks
     **********************************************************************/

    if(not asked.count("input")) {
        EXIT_ON_ERROR(Missing_Argument, "Missing required argument: input fifo named pipes.");
    }
    if(not asked.count("output")) {
        EXIT_ON_ERROR(Missing_Argument, "Missing required argument: output fifo named pipes.");
    }

    std::string finput = asked["input"].as<std::string>();
    std::string freply = asked["output"].as<std::string>();

    if(not std::filesystem::exists(finput)) {
        EXIT_ON_ERROR(No_File, "Input fifo named pipe does not exists.");
    }
    if(not std::filesystem::exists(freply)) {
        EXIT_ON_ERROR(No_File, "Output fifo named pipe does not exists.");
    }

    if(not std::filesystem::is_fifo(finput)) {
        EXIT_ON_ERROR(Not_FIFO, "Input is not a fifo named pipe.");
    }
    if(not std::filesystem::is_fifo(freply)) {
        EXIT_ON_ERROR(Not_FIFO, "Ouptut is not a fifo named pipe.");
    }

    if(asked["dimension"].as<int>() < 1) {
        EXIT_ON_ERROR(Invalid_Argument, "Problem dimension cannot be < 1.");
    }
    
    /***********************************************************************
     * Main loop
     **********************************************************************/

    CLUTCHLOG(progress, "Start the server.");

    while(true) {

        // Blocking call on named pipes.
        std::string data = read(finput);

        json jquery;
        try {
            jquery = json::parse(data);
        } catch (json::parse_error& ex) {
            std::ostringstream msg;
            msg << "JSON payload parse error at byte " << ex.byte << ".";
            error(freply, msg.str(), Error::Payload_Parse);
            continue;
        }

        if(not jquery.contains("query_type")) {
            error(freply, "JSON payload does not contain a `query_type` field.", Error::Payload_Type);
            continue;
        }

        std::string query_type = jquery["query_type"];

        /**********************************************************************
         * stop
         *********************************************************************/
        if(query_type == "stop") {
            CLUTCHLOG(progress, "Been asked to exit.");
            json jreply = forge_ack();
            send(freply, jreply.dump());
            break;

        /**********************************************************************
         * new_run
         *********************************************************************/
        } else if(query_type == "new_run") {
            CLUTCHLOG(note, "Reset the logger for a new run.");
            logger.reset();
            json jreply = forge_ack();
            send(freply, jreply.dump());
            continue;

        /**********************************************************************
         * call
         *********************************************************************/
        } else if(query_type == "call") {

            if(not jquery.contains("solution")) {
                error(freply, "Query `call` should contain a `solution` field.", Error::Payload_Incomplete);
                continue;
            }

            if(type == "integer") {
                call<ioh::problem::Integer>(problem_int, asked, jquery, freply);
            } else if(type == "real") {
                call<ioh::problem::Real>(problem_real, asked, jquery, freply);
            }

        /**********************************************************************
         * else
         *********************************************************************/
        } else {
            std::ostringstream msg;
            msg << "query_type `" << query_type << "` not supported.";
            error(freply, msg.str(), Error::Not_Supported);
            continue;
        }
    } // while true

    CLUTCHLOG(progress, "Stop the server.");
    EXIT(No_Error);
}
