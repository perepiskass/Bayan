#include "collector.h"

Collector::Collector(Options* options): opt(options)
{
}

std::map<std::string,std::vector<fs::path> > Collector::collect()
{
    for(const auto& dir : opt->paths)
    {
        iterateOverFiles(opt->depth, dir);
    }
    return result;
}

//-----Функция сравнения на исключаемые директории------------------------------------------
bool Collector::pathCompare(const fs::path& dir)
{
    for(const auto& exc_dir : opt->exc_paths)
    {
        if (fs::absolute(dir) == fs::absolute(exc_dir)) return false;
    }
    return true;
}
//-----Функция сравнения на соответствеие маски файла ---------------------------------------
bool Collector::maskCompare(const fs::path& dir)
{
    if(opt->v_regex.empty()) return true;
    std::string filename{dir.filename().string()};
    bool res = false;
    for(const auto& reg : opt->v_regex)
    {
        if(std::regex_search(std::begin(filename),std::end(filename),reg)) res = true;
    }
    return res;
}

//-----Функция сравнения размера файла и его пути----------------------------------------
bool Collector::compareSizeAndDir(const fs::path& file1, const fs::path& file2)
{   
    if (fs::file_size(file1) == fs::file_size(file2) && file1!=file2) return true;
    else return false;
}
//-----Функция получения и сравнения хэш для блоков---------------------------------------
void Collector::hashCompare(const fs::path& file)
{
    for(const auto& name_vhash : all_path)
    {
        if(compareSizeAndDir(name_vhash.first,file))
        {
            size_t count_block = fs::file_size(name_vhash.first)/opt->block;
            if(fs::file_size(name_vhash.first)%opt->block) ++count_block;
            
            bool iqual = false;
            for(size_t i = 0 ; i < count_block; ++i )
            {
                if(all_path[name_vhash.first].size() > i && !(all_path[file].size() > i))
                {
                    all_path[file].emplace_back(hashFromFile(i,file));
                    if(name_vhash.second[i] == all_path[file][i]) iqual = true;
                    else {iqual = false; break;}
                }
                else if(!(all_path[name_vhash.first].size() > i) && !(all_path[file].size() > i))
                {
                    all_path[name_vhash.first].emplace_back(hashFromFile(i,name_vhash.first));
                    all_path[file].emplace_back(hashFromFile(i,file));
                    if(all_path[name_vhash.first][i] == all_path[file][i]) iqual = true;
                    else {iqual = false; break;}
                }
            }
            if(iqual)
            {
                auto str = all_path.at(file).front() + all_path.at(file).back();
                result[str].emplace_back(file);
                auto it = std::find(std::begin(result[str]),std::end(result[str]),name_vhash.first);
                if (it == std::end(result[str])) result[str].emplace_back(name_vhash.first);
            }
        }
        else all_path[file];

    }
}

//-----Функция просмотра файлов и фильтрации папок и фалов по заданным параметрам---------------------
void Collector::iterateOverFiles(size_t depth, fs::path dir)
{
        if(pathCompare(dir))
        {
            for(const auto& entry : fs::directory_iterator(dir))
            {   
                const fs::path file{entry.path()};
                if(fs::is_directory(file))
                {
                    if(depth > 0) iterateOverFiles(depth - 1, file);   
                }
                else if(maskCompare(file) && fs::file_size(file) >= opt->size)
                {
                    if(all_path.empty()) all_path[file];
                    else
                    {
                        hashCompare(file);
                    }
                }
            }
        }
}

//-----Получение хэша от размера блока заданного файла------------------------------------------
std::string Collector::hashFromFile(size_t i,const fs::path& file)
{
	std::fstream is (file.c_str(),std::ios_base::in | std::ios_base::binary);
  if (is.is_open()) 
  {
		// get length of file: Можно заменить на получение размера файла из boost::filesystem
		is.seekg (0, is.end);
		int length_all = is.tellg();
		// get bloc count
		size_t count_block = length_all/opt->block;
		if(length_all%opt->block) ++count_block;

		char* buffer = new char [opt->block];
		if(i < count_block-1)
		{
			is.seekg (i*opt->block, is.beg);
			is.read (buffer,opt->block);
		}
		else
		{
			is.seekg (i*opt->block, is.beg);
			int length =length_all - is.tellg();
			is.read (buffer,length);
			for(uint i = length; i < opt->block; ++i)
			{
				buffer[i] = '\0';
			}
		}
		is.close();
		std::string hash_result;
		hash_result = opt->hash->getHash(buffer,opt->block);
		delete [] buffer;
		return hash_result;
  	}
	else throw errno;
}