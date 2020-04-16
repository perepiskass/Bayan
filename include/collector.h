#pragma once
#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <algorithm>
#include <boost/program_options.hpp>


#pragma once
#include "hashfunction.h"
#include "options.h"


/**
 * @brief Класс, организует сбор дубликатов файлов и хранение промежуточной информации.
 */
class Collector
{
    private:
        Options* opt;
        std::map<fs::path,std::vector<std::string>> all_path;       ///< Хранит хэш блоков файла и путь к нему(только для файлов которые проходят фильтры по опциям)
        std::map<std::string,std::vector<fs::path>> result;         ///< Хранит результат, хэш одинаковых файлов и пути к ним
    public:
        Collector(Options* opt);
        std::map<std::string,std::vector<fs::path> > collect();
    private:
        void iterateOverFiles(size_t depth, fs::path dir);
        bool pathCompare(const fs::path& dir);
        bool maskCompare(const fs::path& dir);
        void hashCompare(const fs::path& file);
        bool compareSizeAndDir(const fs::path& file1, const fs::path& file2);
        std::string hashFromFile(size_t i,const fs::path& file);

};




