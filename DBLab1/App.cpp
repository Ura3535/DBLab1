#include "App.h"
#include "iostream"
#include "execution"

static std::ostream& operator<<(std::ostream& out, Model::Product product) {
	out
		<< product.Id << ' '
		<< product.Name << ' '
		<< product.Price << ' '
		<< product.CategoryId << ' '
		<< product.Description;
	return out;
}

static std::ostream& operator<<(std::ostream& out, Model::Category category) {
	out 
		<< category.Id << ' '
		<< category.Name;
	return out;
}

static void getline(char* str, std::streamsize count) {
	std::string tmp;
	std::cin >> tmp;
	strcpy_s(str, count, tmp.c_str());
}

App::Command App::ToCommand(const std::string& strCommand)
{
	std::string s = strCommand;
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (s == "get-m") return GET_M;
	if (s == "get-s") return GET_S;
	if (s == "del-m") return DEL_M;
	if (s == "del-s") return DEL_S;
	if (s == "update-m") return UPD_M;
	if (s == "update-s") return UPD_S;
	if (s == "insert-m") return INS_M;
	if (s == "insert-s") return INS_S;
	if (s == "calc-m") return CALC_M;
	if (s == "calc-s") return CALC_S;
	if (s == "calc-s2") return CALC_S2;
	if (s == "ut-m") return UT_M;
	if (s == "ut-s") return UT_S;
	if (s == "help") return HELP;
	if (s == "exit") return EXIT;

	return INVALID;
}

void App::Run()
{
	std::string str;
	Command comm;
	Model::Product product;
	Model::Category category;

	while (true) {
		std::cin >> str;
		comm = ToCommand(str);
		try
		{
			switch (comm)
			{
			case GET_M:
				std::cout << "Category Id: ";
				std::cin >> category.Id;
				std::cout << rep.Categories.Get(category.Id) << '\n';
				break;

			case DEL_M:
				std::cout << "Category Id: ";
				std::cin >> category.Id;
				rep.Categories.Delete(category.Id);
				break;

			case UPD_M:
				std::cout << "Category Id: ";
				std::cin >> category.Id;
				std::cout << "Category Name: ";
				getline(category.Name, 50);
				rep.Categories.Update(category);
				break;

			case INS_M:
				std::cout << "Category Name: ";
				getline(category.Name, 50);
				rep.Categories.Insert(category);
				break;

			case CALC_M:
				std::cout << rep.Categories.Calc() << '\n';
				break;

			case UT_M:
				for (const auto& x : rep.Categories.GetAll())
					std::cout << x << '\n';
				break;

			case GET_S:
				std::cout << "Product Id: ";
				std::cin >> product.Id;
				std::cout << rep.Products.Get(product.Id) << '\n';
				break;

			case DEL_S:
				std::cout << "Product Id: ";
				std::cin >> product.Id;
				rep.Products.Delete(product.Id);
				break;

			case UPD_S:
				std::cout << "Product Id: ";
				std::cin >> product.Id;
				std::cout << "Name: ";
				getline(product.Name, 50);
				std::cout << "Price: ";
				std::cin >> product.Price;
				std::cout << "CategoryId: ";
				std::cin >> product.CategoryId;
				std::cout << "Description: ";
				getline(product.Description, 50);
				rep.Products.Update(product);
				break;

			case INS_S:
				std::cout << "Name: ";
				getline(product.Name, 50);
				std::cout << "Price: ";
				std::cin >> product.Price;
				std::cout << "CategoryId: ";
				std::cin >> product.CategoryId;
				std::cout << "Description: ";
				getline(product.Description, 50);
				rep.Products.Insert(product);
				break;

			case CALC_S:
				std::cout << rep.Products.Calc() << '\n';
				break;

			case UT_S:
				for (const auto& x : rep.Products.GetAll())
					std::cout << x << '\n';
				break;

			case CALC_S2:
				std::cout << "CategoryId: ";
				std::cin >> product.CategoryId;
				std::cout << rep.Products.Calc(product.CategoryId) << '\n';
				break;

			case INVALID:
				std::cout << "Неправильна команда\n";
				break;

			case HELP:
				std::cout << "Список команд:\n";

				std::cout << "\t\"get-m\" - оримати Category за його Id\n";
				std::cout << "\t\"del-m\" - видалити Category за його Id\n";
				std::cout << "\t\"update-m\" - оновити Category за його Id\n";
				std::cout << "\t\"insert-m\" - додати новий Category\n";
				std::cout << "\t\"calc-m\" - кількість Category\n";
				std::cout << "\t\"ut-m\" - отримати всі Category\n";

				std::cout << "\t\"get-s\" - оримати Product за його Id\n";
				std::cout << "\t\"del-s\" - видалити Product за його Id\n";
				std::cout << "\t\"update-s\" - оновити Product за його Id\n";
				std::cout << "\t\"insert-s\" - додати нового Product\n";
				std::cout << "\t\"calc-s\" - кількість Product\n";
				std::cout << "\t\"calc-s2\" - кількість Product з заданим CategoryId\n";
				std::cout << "\t\"ut-s\" - отримати всіх Product\n";

				std::cout << "\t\"help\" - всі команди\n";
				std::cout << "\t\"exit\" - завершити виконання\n";
				break;

			case EXIT:
				return;
			default: throw std::exception("Невідома команда");
			}
		}
		catch (const std::exception& exep)
		{
			std::cout << exep.what();
		}
	}
}
