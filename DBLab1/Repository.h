#pragma once

#include "Model.h"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
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

	class ProductRepository {
		void CreateTable(const fs::path& FileFL);
		void Write(const Model::Product& data, long pos);
		Model::Product Read(long pos);
		void Defragment();

		fs::path DBFolder;
		std::fstream file;
		long auto_inc_key;
		std::unordered_map<long, long> ind;
		std::multiset<long> trash;

		friend struct Repository;
	public:
		ProductRepository(const fs::path& DBFolder);
		~ProductRepository();

		Model::Product Get(long Id);
		void Delete(long Id);
		void Update(const Model::Product& data);
		void Insert(const Model::Product& data);
		size_t Calc();
		size_t Calc(long CategoryId);
		std::vector<Model::Product> GetAll();
		std::vector<Model::Product> GetByCategoryId(long CategoryId);
	};

	class CategoryRepository {
		void CreateTable(const fs::path& FileFL);
		void Write(const Model::Category& data, long pos);
		Model::Category Read(long pos);
		void Defragment();

		fs::path DBFolder;
		std::fstream file;
		long auto_inc_key;
		std::unordered_map<long, long> ind;
		std::multiset<long> trash;

		friend struct FileRepository;
		ProductRepository* slave;
	public:
		CategoryRepository(const fs::path& DBFolder);
		~CategoryRepository();

		Model::Category Get(long Id);
		void Delete(long Id);
		void Update(const Model::Category& data);
		void Insert(const Model::Category& data);
		size_t Calc();
		std::vector<Model::Category> GetAll();
	};

	struct FileRepository
	{
		FileRepository(const fs::path& DBFolder = "Files");

		CategoryRepository Categories;
		ProductRepository Products;
	};
};


