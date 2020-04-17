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
    bool compare = false;
    for(auto& name_vhash : all_path)
    {
        if(compareSizeAndDir(name_vhash.first,file))
        {
            bool iqual = false;
            size_t count_block = fs::file_size(name_vhash.first)/opt->block;
            if(fs::file_size(name_vhash.first)%opt->block) ++count_block;
            
            // open file
            std::fstream is_one (file.c_str(),std::ios_base::in | std::ios_base::binary);
            if (is_one.is_open()) 
            {   
                size_t i = 0;
                for(;i<name_vhash.second.size();)
                {
                    all_path[file].emplace_back(std::move(hashFromBlock(is_one,i,count_block)));
                    if(name_vhash.second.at(i) == all_path[file].at(i)) 
                    {
                        iqual = true;
                        ++i;
                    }
                    else 
                    {
                        iqual = false;
                        break;
                    }
                }

                if (iqual == true || name_vhash.second.empty())
                {
                    std::fstream is_two (name_vhash.first.c_str(),std::ios_base::in | std::ios_base::binary);
                    if(is_two.is_open())
                    {
                        for(;i < count_block;)
                        {
                            all_path[file].emplace_back(std::move(hashFromBlock(is_one,i,count_block)));
                            name_vhash.second.emplace_back(std::move(hashFromBlock(is_two,i,count_block)));
                            if(name_vhash.second.at(i) == all_path[file].at(i)) 
                            {
                                iqual = true;
                                ++i;
                            }
                            else 
                            {
                                iqual = false;
                                break;
                            }

                        }
                    }
                    else throw errno;
                    is_two.close();
                }
                compare = true;
            }
            else throw errno;
            is_one.close();
            //-----------------------------------------------------------------------------------------------
                
            if(iqual)
            {
                auto str = all_path.at(file).front() + all_path.at(file).back();
                result[str].emplace_back(file);
                auto it = std::find(std::begin(result[str]),std::end(result[str]),name_vhash.first);
                if (it == std::end(result[str])) result[str].emplace_back(name_vhash.first);
                break;
            }
        }
    }
    if(!compare) all_path[file]; 
}

//-----Функция получения хэш из блока ----------------------------------------------------
std::string Collector::hashFromBlock(std::fstream& is, size_t i, size_t count_block)
{
    char* buffer = new char [opt->block];
		if(i < count_block-1)
		{
			is.seekg (i*opt->block, is.beg);
			is.read (buffer,opt->block);
		}
		else
		{   
            is.seekg (0,is.end);
            auto length_all = is.tellg();

			is.seekg (i*opt->block, is.beg);
			int length =length_all - is.tellg();
			is.read (buffer,length);
			for(uint i = length; i < opt->block; ++i)
			{
				buffer[i] = '\0';
			}
		}
		
		std::string hash_result = opt->hash->getHash(buffer,opt->block);
		delete [] buffer;
		return hash_result;
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

