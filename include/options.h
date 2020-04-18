#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <algorithm>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "hashfunction.h"
#include "version_lib.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;
using strings = std::vector<std::string>;

/**
 * @brief Класс, для сбора, фильтрации и хранения заданных опций.
 */
class Options
{
    public:
        size_t size;        ///< Миниальный размер блока файла для сканирования, задается по умолчанию в конструкторе (1 байт)
        size_t block;       ///< Размер блока для чтения с файла и построению по нему хэш суммы для сравнения, задается по умолчанию в конструкторе (256 байт)
        size_t depth;       ///< Глубина сканирования в папках, задается по умолчанию в конструкторе (0 - без вложенных папок)
        std::vector<fs::path> paths;        ///< Пути для сканирования файлов
        std::vector<fs::path> exc_paths;    ///< Пути для исключения из сканирования (вложенные так же не будут сканироваться, надо указывать явно в path)    
        std::vector<std::regex> v_regex;    ///< Хранит список регулярных выражений для применеия фильтра маски файла
        HFunction* hash = nullptr;

        Options(int argc, char *argv[]);
        ~Options();
    private:
        void setMasks(const std::vector<std::string>& masks);
        void setPath(const std::vector<std::string>& paths);
        void setExceptPath(const std::vector<std::string>& exc_paths);
        void setHash(const std::string hash_name);
        std::regex makeRegex(const std::string& mask);
        bool checkPath(const std::string& path);

};