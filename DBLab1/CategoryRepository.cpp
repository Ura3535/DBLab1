#include "Repository.h"
//#include <execution>

using namespace Repository;

static long get_min_pos(std::multiset<long>& trash) {
	auto min_it = trash.begin();
	long res = *min_it;
	trash.erase(min_it);
	return res;
}

void CategoryRepository::CreateTable(const fs::path& FileFL)
{
	std::fstream new_table(FileFL, std::ios::out | std::ios::binary);
	ServiceData default_serv_data;
	default_serv_data.save(new_table);
}

void CategoryRepository::Write(const Model::Category& data, long pos)
{
	file.seekp(ServiceData::service_data_size + pos * Model::Category::size, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data.Id), sizeof(data.Id));
	file.write(data.Name, sizeof(data.Name));
}

Model::Category CategoryRepository::Read(long pos)
{
	Model::Category obj;
	file.seekg(ServiceData::service_data_size + pos * Model::Category::size, std::ios::beg);
	file.read(reinterpret_cast<char*>(&obj.Id), sizeof(obj.Id));
	file.read(obj.Name, sizeof(obj.Name));

	return obj;
}

void CategoryRepository::Defragment()
{
	if (ind.empty()) return;
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

CategoryRepository::CategoryRepository(const fs::path& DBFolder)
	: DBFolder(DBFolder)
	, slave(nullptr)
{
	fs::path FileFL = DBFolder / "Categories.fl";
	fs::path FileIND = DBFolder / "Categories.ind";

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
			file.seekg(ServiceData::service_data_size + i * Model::Category::size, std::ios::beg);
			file.read(reinterpret_cast<char*>(&Id), sizeof(Id));
			ind[Id] = i;
		}
	}

	bool make_ind_bad = false;
	file.seekp(ServiceData::ind_is_correct_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&make_ind_bad), sizeof(make_ind_bad));
}

CategoryRepository::~CategoryRepository()
{
	Defragment();

	fs::path FileFL = DBFolder / "Categories.fl";
	fs::path FileIND = DBFolder / "Categories.ind";

	std::ofstream index(FileIND, std::ios::out | std::ios::trunc);
	for (const auto& x : ind)
		index << x.first << ' ' << x.second << '\n';

	ServiceData serv_data{ ind.size(), auto_inc_key, true };
	serv_data.save(file);

	fs::resize_file(FileFL, ServiceData::service_data_size + ind.size() * Model::Category::size);
}

Model::Category CategoryRepository::Get(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Category Id");
	return Read(ind[Id]);
}

void CategoryRepository::Delete(long Id)
{
	if (!ind.contains(Id))
		throw std::exception("Немає такого Category Id");

	for (const auto& x : slave->GetByCategoryId(Id))
		slave->Delete(x.Id);

	trash.insert(ind[Id]);
	ind.erase(Id);

	size_t data_num = ind.size();
	file.seekp(ServiceData::data_num_pos, std::ios::beg);
	file.write(reinterpret_cast<const char*>(&data_num), sizeof(data_num));
}

void CategoryRepository::Update(const Model::Category& data)
{
	if (!ind.contains(data.Id))
		throw std::exception("Немає такого Category Id");
	Write(data, ind[data.Id]);
}

void CategoryRepository::Insert(const Model::Category& data)
{
	Model::Category data_with_Id(data);
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

size_t CategoryRepository::Calc()
{
	return ind.size();
}

std::vector<Model::Category> CategoryRepository::GetAll()
{
	std::vector<Model::Category> vector;
	for (const auto& x : ind) {
		vector.push_back(Get(x.first));
	}

	return vector;
}