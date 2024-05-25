#pragma once

#include "Model.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

namespace Repository
{
	struct ServiceData {
		ServiceData(size_t data_num = 0, long auto_inc_key = 0, bool ind_is_correct = false);
		void load(std::fstream& file);
		void save(std::fstream& file) const;

		size_t data_num;
		long auto_inc_key;
		bool ind_is_correct;

		static constexpr size_t service_data_size = sizeof(data_num) + sizeof(auto_inc_key) + sizeof(ind_is_correct);
		static constexpr size_t data_num_pos = 0;
		static constexpr size_t auto_inc_key_pos = sizeof(data_num);
		static constexpr size_t ind_is_correct_pos = sizeof(data_num) + sizeof(auto_inc_key);
	};

	class AutorRepository {
		void CreateTable(const fs::path& FileFL);
		void Write(const Model::Autor& data, long pos);

		fs::path DBFolder;
		std::fstream file;
		long auto_inc_key;
		std::unordered_map<long, long> ind;

		friend struct Repository;
	public:
		AutorRepository(const fs::path& DBFolder);
		~AutorRepository();

		Model::Autor Get(long Id);
		void Delete(long Id);
		void Update(const Model::Autor& data);
		void Insert(const Model::Autor& data);
		size_t Calc();
		size_t Calc(long RegionId);
		std::vector<Model::Autor> GetAll();
		std::vector<Model::Autor> GetByRegionId(long RegionId);
	};

	class RegionRepository {
		void CreateTable(const fs::path& FileFL);
		void Write(const Model::Region& data, long pos);

		fs::path DBFolder;
		std::fstream file;
		long auto_inc_key;
		std::unordered_map<long, long> ind;

		friend struct FileRepository;
		AutorRepository* slave;
	public:
		RegionRepository(const fs::path& DBFolder);
		~RegionRepository();

		Model::Region Get(long Id);
		void Delete(long Id);
		void Update(const Model::Region& data);
		void Insert(const Model::Region& data);
		size_t Calc();
		std::vector<Model::Region> GetAll();
	};

	struct FileRepository
	{
		FileRepository(const fs::path& DBFolder = "DataBase");

		RegionRepository Regions;
		AutorRepository Autors;
	};
};


