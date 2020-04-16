#pragma once
#include <boost/crc.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include <string>

/**
 * @brief Интерфейс для реализации функций хэширования.
 */
class HFunction
{
	public:
	HFunction();
	virtual std::string getHash(const char* buf, size_t blockSize)=0;
    virtual ~HFunction()=default;
	
};

class CRC32:public HFunction
{
	public:
	virtual std::string getHash(const char* buf, size_t blockSize) override;
	~CRC32()=default;
};

class SHA1:public HFunction
{
	public:
	virtual std::string getHash(const char* buf, size_t blockSize) override;
	~SHA1()=default;
};