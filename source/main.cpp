#include "collector.h"

int main(int argc, char *argv[]) 
{
  try
  {
    Options opt(argc,argv);
    Collector duplicates(&opt);

    auto result = duplicates.collect();

    for(const auto& dir : result)
    {
      for(const auto& file_path : dir.second)
      {
        std::cout << fs::absolute(file_path).string() << std::endl;
      }
      std::cout << std::endl;
    }

  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}
