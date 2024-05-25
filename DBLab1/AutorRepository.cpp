#include "Repository.h"

using namespace Repository;
using namespace Model;

void AutorRepository::CreateTable(const fs::path& FileFL)
{
	std::fstream new_table(FileFL, std::ios::out | std::ios::binary);
	ServiceData default_serv_data;
	default_serv_data.save(new_table);
	
}

void AutorRepository::Write(const Autor& data, long pos)
{
	file.seekp(ServiceData::service_data_size + pos * Autor::size, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data.Id), sizeof(data.Id));
	file.write(reinterpret_cast<const char*>(&data.RegionId), sizeof(data.RegionId));
	file.write(data.AutorName, sizeof(data.AutorName));
	file.write(data.Pseudonym, sizeof(data.Pseudonym));
}

AutorRepository::AutorRepository(const fs::path& DBFolder)
	: DBFolder(DBFolder)
{
	fs::path FileFL = DBFolder / "Autors.fl";
	fs::path FileIND = DBFolder / "Autors.ind";

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
			file.seekg(ServiceData::service_data_size + i * Autor::size, std::ios::beg);
			file.read(reinterpret_cast<char*>(&Id), sizeof(Id));
			ind[Id] = i;
		}
	}

	bool make_ind_bad = false;
	file.seekp(ServiceData::ind_is_correct_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&make_ind_bad), sizeof(make_ind_bad));
}

AutorRepository::~AutorRepository()
{
	std::ofstream index_table(DBFolder / "Autors.ind", std::ios::out | std::ios::trunc);

	for (const auto& x : ind)
		index_table << x.first << ' ' << x.second << '\n';

	ServiceData serv_data{ ind.size(), auto_inc_key, true };
	serv_data.save(file);

	//TODO: eraze end of file
}

Autor AutorRepository::Get(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Autor Id");
	Autor obj;
	file.seekg(ServiceData::service_data_size + ind[Id] * Autor::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&obj.Id), sizeof(obj.Id));
	file.read(reinterpret_cast<char*>(&obj.RegionId), sizeof(obj.RegionId));
	file.read(obj.AutorName, sizeof(obj.AutorName));
	file.read(obj.Pseudonym, sizeof(obj.Pseudonym));

	return obj;
}

void AutorRepository::Delete(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Autor Id");
	long Id_of_last;
	file.seekg(ServiceData::service_data_size + (ind.size() - 1) * Autor::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&Id_of_last), sizeof(Id_of_last));
	Autor tmp = Get(Id_of_last);

	Write(tmp, ind[Id]);

	ind[tmp.Id] = ind[Id];
	ind.erase(Id);


	size_t data_num = ind.size();
	file.seekp(ServiceData::data_num_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
}

void AutorRepository::Update(const Autor& data)
{
	if (!ind.contains(data.Id))
		throw std::exception("Немає такого Autor Id");
	Write(data, ind[data.Id]);
}

void AutorRepository::Insert(const Autor& data)
{
	Autor data_with_Id(data);
	data_with_Id.Id = auto_inc_key++;

	Write(data_with_Id, (long)ind.size());

	ind[data_with_Id.Id] = (long)ind.size();

	size_t data_num = ind.size();
	file.seekp(ServiceData::data_num_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
	file.seekp(ServiceData::auto_inc_key_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&auto_inc_key), sizeof(auto_inc_key));
}

size_t AutorRepository::Calc()
{
	return ind.size();
}

size_t AutorRepository::Calc(long RegionId)
{
	std::vector<Autor> vector;
	Autor tmp;
	file.seekg(ServiceData::service_data_size, std::ios::beg);
	size_t size = 0;
	for (const auto& x : ind) {
		tmp = Get(x.first);
		if (tmp.RegionId == RegionId)
			++size;
	}

	return size;
}

std::vector<Autor> AutorRepository::GetAll()
{
	std::vector<Autor> vector;
	file.seekg(ServiceData::service_data_size, std::ios::beg);
	for (const auto& x : ind) {
		vector.push_back(Get(x.first));
	}

	return vector;
}

std::vector<Autor> AutorRepository::GetByRegionId(long RegionId)
{
	std::vector<Autor> vector;
	Autor tmp;
	file.seekg(ServiceData::service_data_size, std::ios::beg);
	for (const auto& x : ind) {
		tmp = Get(x.first);
		if (tmp.RegionId == RegionId)
			vector.push_back(tmp);
	}

	return vector;
}
