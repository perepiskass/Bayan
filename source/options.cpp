#include "options.h"

Options::Options(int argc, char *argv[])
{
    po::options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help screen")
      ("version,v", "Program version")
      ("path", po::value<std::vector<std::string>>()->multitoken()->
        zero_tokens()->composing()->default_value(strings {"."}, "."), "Path to scanning directory")
      ("not", po::value<std::vector<std::string>>()->multitoken()->
        zero_tokens()->composing()->default_value(strings {"/"}, ""), "Path to exclude scanning")
      ("depth", po::value<size_t>()->default_value(0), "Scanning depth")
      ("size", po::value<size_t>()->default_value(1), "Minimum file size (byte)")
      ("masks", po::value<std::vector<std::string>>()->multitoken()->
        zero_tokens()->composing()->default_value(strings {"\n"}, ""), "File name masks to scan")
      ("block", po::value<size_t>()->default_value(256), "Block size for reading a file")
      ("hash", po::value<std::string>()->default_value("crc32"), "Hash algorithm for searching");

    po::positional_options_description pos_desc;
    pos_desc.add("path", -1);

    po::command_line_parser parser{argc, argv};
    parser.options(desc).positional(pos_desc).allow_unregistered();
    po::parsed_options parsed_options = parser.run();

    po::variables_map vm;
    store(parsed_options, vm);
    notify(vm);

    if (vm.count("help"))
    std::cout << desc << '\n';
    if (vm.count("version"))
    std::cout << "version: " << version_major()<< '.'<< version_minor() << '.' << version_patch() << std::endl << std::endl;

    this->size = vm["size"].as<size_t>();
    this->block = vm["block"].as<size_t>();
    this->depth = vm["depth"].as<size_t>();

    this->setPath(vm["path"].as<std::vector<std::string>>());
    this->setExceptPath(vm["not"].as<std::vector<std::string>>());
    this->setMasks(vm["masks"].as<std::vector<std::string>>());
    this->setHash(vm["hash"].as<std::string>());

}

Options::~Options()
{
    delete hash;
}

void Options::setMasks(const std::vector<std::string>& masks)
{    
    if(!masks.empty() && masks[0] != "\n")
    {
        for(const auto& str : masks)
        {
            v_regex.emplace_back(makeRegex(str));
        }
    }
    else v_regex.clear();
}

void Options::setHash(const std::string hash_name)
{
    if(hash_name == "crc32") this->hash = new CRC32();
    else if(hash_name == "sha1") this->hash = new SHA1();
    else
    {
        std::cout <<"This HASH - " << hash_name << " do not supported, sorry" << std::endl;
        exit(0);
    }
}

std::regex Options::makeRegex(const std::string& mask)
{
    if(!mask.empty())
    {
        return std::regex{mask,std::regex::icase};
    }
    return std::regex{};
}

bool Options::checkPath(const std::string& dir)
{
    if(!fs::exists(fs::absolute(fs::path(dir),fs::current_path())))
    {
        std::cout << "Path " << dir << " does not exist." << std::endl;
        exit(0);
    }
    else return true;
}

void Options::setPath(const std::vector<std::string>& paths)
{
    for(const auto& dir : paths)
    {
        if(checkPath(dir))
        {
            if(dir == ".") this->paths.emplace_back(fs::current_path());
            else this->paths.emplace_back(fs::path(dir));
        }
    }
}

void Options::setExceptPath(const std::vector<std::string>& exc_paths)
{
    for(const auto& dir : exc_paths)
    {
        if(checkPath(dir))
        {
            this->exc_paths.emplace_back(fs::path(dir));
        }
    }
}

