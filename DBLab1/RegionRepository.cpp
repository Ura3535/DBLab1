#include "Repository.h"

using namespace Model;
using namespace Repository;

void RegionRepository::CreateTable(const fs::path& FileFL)
{
	std::fstream new_table(FileFL, std::ios::out | std::ios::binary);
	ServiceData default_serv_data;
	default_serv_data.save(new_table);
}

void RegionRepository::Write(const Region& data, long pos)
{
	file.seekp(ServiceData::service_data_size + pos * Region::size, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data.Id), sizeof(data.Id));
	file.write(data.RegionName, sizeof(data.RegionName));
}

RegionRepository::RegionRepository(const fs::path& DBFolder)
	: DBFolder(DBFolder)
	, slave(nullptr)
{
	fs::path FileFL = DBFolder / "Regions.fl";
	fs::path FileIND = DBFolder / "Regions.ind";

	if (!std::filesystem::exists(FileFL))
		CreateTable(FileFL);
	file.open(FileFL, std::ios::in | std::ios::out | std::ios::binary);

	ServiceData serv_data;
	serv_data.load(file);
	auto_inc_key = serv_data.auto_inc_key;

	if (serv_data.ind_is_correct) {
		std::ifstream index(FileIND);
		long key, val;

		for (int i = 0; i != serv_data.data_num; ++i) {
			index >> key >> val;
			ind[key] = val;
		}
	}
	else {
		long Id;
		for (int i = 0; i != serv_data.data_num; ++i) {
			file.seekg(ServiceData::service_data_size + i * Region::size, std::ios::beg);
			file.read(reinterpret_cast<char*>(&Id), sizeof(Id));
			ind[Id] = i;
		}
	}

	bool make_ind_bad = false;
	file.seekp(ServiceData::ind_is_correct_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&make_ind_bad), sizeof(make_ind_bad));
}

RegionRepository::~RegionRepository()
{
	std::ofstream index(DBFolder / "Regions.ind", std::ios::out | std::ios::trunc);
	for (const auto& x : ind)
		index << x.first << ' ' << x.second << '\n';

	ServiceData serv_data{ ind.size(), auto_inc_key, true };
	serv_data.save(file);

	//TODO: eraze end of file
}

Region RegionRepository::Get(long Id)
{
	Region obj;
	file.seekg(ServiceData::service_data_size + ind[Id] * Region::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&obj.Id), sizeof(obj.Id));
	file.read(obj.RegionName, sizeof(obj.RegionName));

	return obj;
}

void RegionRepository::Delete(long Id)
{
	for (const auto& x : slave->GetByRegionId(Id))
		slave->Delete(x.Id);

	long Id_of_last;
	file.seekg(ServiceData::service_data_size + (ind.size() - 1) * Region::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&Id_of_last), sizeof(Id_of_last));
	Region tmp = Get(Id_of_last);

	Write(tmp, ind[Id]);

	ind[tmp.Id] = ind[Id];
	ind.erase(Id);

	size_t data_num = ind.size();
	file.seekp(ServiceData::data_num_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
}

void RegionRepository::Update(const Region& data)
{
	Write(data, ind[data.Id]);
}

void RegionRepository::Insert(const Region& data)
{
	Region data_with_Id(data);
	data_with_Id.Id = auto_inc_key++;

	Write(data_with_Id, (long)ind.size());

	ind[data_with_Id.Id] = (long)ind.size();


	size_t data_num = ind.size();
	file.seekp(ServiceData::data_num_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
	file.seekp(ServiceData::auto_inc_key_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&auto_inc_key), sizeof(auto_inc_key));
}

std::vector<Region> RegionRepository::GetAll()
{
	std::vector<Region> vector(ind.size());
	file.seekg(ServiceData::service_data_size, std::ios::beg);
	int i = 0;
	for (const auto& x : ind) {
		vector[i++] = Get(x.first);
	}

	return vector;
}