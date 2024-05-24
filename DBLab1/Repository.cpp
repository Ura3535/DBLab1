#include "Repository.h"

using namespace Model;
using namespace Repository;

ServiceData::ServiceData(size_t data_num, long auto_inc_key, bool ind_is_correct)
	: data_num(data_num)
	, auto_inc_key(auto_inc_key)
	, ind_is_correct(ind_is_correct)
{}

void ServiceData::load(std::fstream& file)
{
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(&data_num), sizeof(data_num));
	file.read(reinterpret_cast<char*>(&auto_inc_key), sizeof(auto_inc_key));
	file.read(reinterpret_cast<char*>(&ind_is_correct), sizeof(ind_is_correct));
}

void ServiceData::save(std::fstream& file) const
{
	file.seekp(0, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
	file.write(reinterpret_cast<const char*>(&auto_inc_key), sizeof(auto_inc_key));
	file.write(reinterpret_cast<const char*>(&ind_is_correct), sizeof(ind_is_correct));
}

FileRepository::FileRepository(const fs::path& DBFolder)
	: Regions(DBFolder)
	, Autors(DBFolder)
{
	Regions.slave = &Autors;
}
