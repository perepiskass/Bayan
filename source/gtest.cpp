#include <gtest/gtest.h>
#include "version_lib.h"
#include "options.h"
#include "collector.h"
#include "hashfunction.h"




// Тест кейсы на проверку версий
TEST(version_test_case, version_test_patch)
{
    ASSERT_GE(version_patch(), 1);
}
TEST(version_test_case, version_test_minor)
{
    EXPECT_EQ(version_minor(), 1);
}
TEST(version_test_case, version_test_major)
{
    EXPECT_EQ(version_major(), 1);
}

// Тест кейсы для класса Options
TEST(options_test_case, set_block_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--block");
    argv[2] = const_cast<char*>("128");
    Options opt(argc,argv);

    ASSERT_EQ(opt.block, 128);
}
TEST(options_test_case, set_size_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--size");
    argv[2] = const_cast<char*>("777");
    Options opt(argc,argv);

    ASSERT_EQ(opt.size, 777);
}
TEST(options_test_case, set_depth_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--depth");
    argv[2] = const_cast<char*>("333");
    Options opt(argc,argv);

    ASSERT_EQ(opt.depth, 333);
}
TEST(options_test_case, set_masks_test)
{   
    int argc = 4;
    char* argv[4];
    argv[1] = const_cast<char*>("--masks");
    argv[2] = const_cast<char*>("Hello");
    argv[3] = const_cast<char*>("World");

    Options opt(argc,argv);

    std::string str0 {"hELLO"};
    std::string str1 {"wOrLd"};
    
    ASSERT_EQ(opt.v_regex.size(), 2);
    
    EXPECT_TRUE(std::regex_search(std::begin(str0),std::end(str0),opt.v_regex.at(0)));
    EXPECT_TRUE(std::regex_search(std::begin(str1),std::end(str1),opt.v_regex.at(1)));

    EXPECT_FALSE(std::regex_search(std::begin(str1),std::end(str1),opt.v_regex.at(0)));
    EXPECT_FALSE(std::regex_search(std::begin(str0),std::end(str0),opt.v_regex.at(1)));
}

TEST(options_test_case, set_path_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--path");
    argv[2] = const_cast<char*>("/home");
    Options opt(argc,argv);

    fs::path home{"/home"};

    ASSERT_EQ(opt.paths.size(), 1);
    ASSERT_EQ(opt.paths.at(0),home);
}

TEST(options_test_case, set_exc_path_test)
{   
    int argc = 4;
    char* argv[4];
    argv[1] = const_cast<char*>("--not");
    argv[2] = const_cast<char*>("/var");
    argv[3] = const_cast<char*>("/opt");

    Options opt(argc,argv);

    fs::path var{"/var"};
    fs::path option{"/opt"};

    ASSERT_EQ(opt.exc_paths.size(), 2);
    ASSERT_EQ(opt.exc_paths.at(0),var);
    ASSERT_EQ(opt.exc_paths.at(1),option);
}

TEST(options_test_case, set_hash_crc32_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--hash");
    argv[2] = const_cast<char*>("crc32");

    Options opt(argc,argv);

    const char* exem = "hello world";
    auto hash_exem = opt.hash->getHash(exem,11);

    ASSERT_EQ(hash_exem, "222957957");
}

TEST(options_test_case, set_hash_sha1_test)
{   
    int argc = 3;
    char* argv[3];
    argv[1] = const_cast<char*>("--hash");
    argv[2] = const_cast<char*>("sha1");

    Options opt(argc,argv);

    const char* exem = "hello world";
    auto hash_exem = opt.hash->getHash(exem,11);

    ASSERT_EQ(hash_exem, "71607403733774509323667336631082891497518538989");
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}