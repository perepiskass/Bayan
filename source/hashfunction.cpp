#include "hashfunction.h"

	HFunction::HFunction()
	{}

	std::string CRC32::getHash(const char* buf, size_t blockSize)
	{
		boost::crc_32_type crc_hash;
		crc_hash.process_bytes(buf, blockSize);
		return std::to_string(crc_hash.checksum());
	}

	std::string SHA1::getHash(const char* buf, size_t blockSize)
	{
		unsigned resHash[5];

		boost::uuids::detail::sha1 Sha1;
		Sha1.process_bytes(buf, blockSize);

		std::string res;

		Sha1.get_digest( resHash );
		
		for (auto d : resHash)	res += std::to_string(d);

		return res;
	}

