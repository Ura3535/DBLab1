#include "Repository.h"

using namespace Repository;
using namespace Model;

static long get_min_pos(std::multiset<long>& trash) {
	auto min_it = trash.begin();
	long res = *min_it;
	trash.erase(min_it);
	return res;
}

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

Model::Autor AutorRepository::Read(long pos)
{
	Autor obj;
	file.seekg(ServiceData::service_data_size + pos * Autor::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&obj.Id), sizeof(obj.Id));
	file.read(reinterpret_cast<char*>(&obj.RegionId), sizeof(obj.RegionId));
	file.read(obj.AutorName, sizeof(obj.AutorName));
	file.read(obj.Pseudonym, sizeof(obj.Pseudonym));

	return obj;
}

void AutorRepository::Defragment()
{
	while (!trash.empty())
	{
		long hole = get_min_pos(trash);
		auto max_pair = std::max_element(
			ind.begin(), ind.end(),
			[](const std::pair<const long, long>& p1, const std::pair<const long, long>& p2) {
				return p1.second < p2.second;
			});
		if (max_pair->second <= hole) break;
		Write(Get(max_pair->first), hole);
		ind[max_pair->first] = hole;
	}
}

AutorRepository::AutorRepository(const fs::path& DBFolder)
	: DBFolder(DBFolder)
{
	fs::path FileFL = DBFolder / "Autors.fl";
	fs::path FileIND = DBFolder / "Autors.ind";

	if (!fs::exists(FileFL))
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
	Defragment();

	fs::path FileFL = DBFolder / "Autors.fl";
	fs::path FileIND = DBFolder / "Autors.ind";

	std::ofstream index_table(FileIND, std::ios::out | std::ios::trunc);

	for (const auto& x : ind)
		index_table << x.first << ' ' << x.second << '\n';

	ServiceData serv_data{ ind.size(), auto_inc_key, true };
	serv_data.save(file);

	fs::resize_file(FileFL, ServiceData::service_data_size + ind.size() * Autor::size);
}

Autor AutorRepository::Get(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Autor Id");

	return Read(ind[Id]);
}

void AutorRepository::Delete(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Autor Id");

	trash.insert(ind[Id]);
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

	long pos = trash.empty() ? (long)ind.size() : get_min_pos(trash);
	Write(data_with_Id, pos);
	ind[data_with_Id.Id] = pos;

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
	Autor tmp;
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
	for (const auto& x : ind) {
		vector.push_back(Get(x.first));
	}

	return vector;
}

std::vector<Autor> AutorRepository::GetByRegionId(long RegionId)
{
	std::vector<Autor> vector;
	Autor tmp;
	for (const auto& x : ind) {
		tmp = Get(x.first);
		if (tmp.RegionId == RegionId)
			vector.push_back(tmp);
	}

	return vector;
}
