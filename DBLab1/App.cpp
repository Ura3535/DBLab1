#include "App.h"
#include "iostream"
#include "execution"

static std::ostream& operator<<(std::ostream& out, Model::Autor autor) {
	out << autor.Id << ' ' << autor.RegionId << ' ' << autor.AutorName << ' ' << autor.Pseudonym;
	return out;
}

static std::ostream& operator<<(std::ostream& out, Model::Region region) {
	out << region.Id << ' ' << region.RegionName;
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

App::App()
	: rep(new Repository::FileRepository)
{}

App::~App()
{
	delete rep;
}

void App::Run()
{
	std::string str;
	Command comm;
	Model::Autor autor;
	Model::Region region;

	while (true) {
		std::cin >> str;
		comm = ToCommand(str);
		try
		{
			switch (comm)
			{
			case GET_M:
				std::cout << "Region Id: ";
				std::cin >> region.Id;
				std::cout << rep->Regions.Get(region.Id) << '\n';
				break;

			case DEL_M:
				std::cout << "Region Id: ";
				std::cin >> region.Id;
				rep->Regions.Delete(region.Id);
				break;

			case UPD_M:
				std::cout << "Region Id: ";
				std::cin >> region.Id;
				std::cout << "Region Name: ";
				getline(region.RegionName, 50);
				rep->Regions.Update(region);
				break;

			case INS_M:
				std::cout << "Region Name: ";
				getline(region.RegionName, 50);
				rep->Regions.Insert(region);
				break;

			case CALC_M:
				std::cout << rep->Regions.GetAll().size() << '\n';
				break;

			case UT_M:
				for (const auto& x : rep->Regions.GetAll())
					std::cout << x << '\n';
				break;

			case GET_S:
				std::cout << "Autor Id: ";
				std::cin >> autor.Id;
				std::cout << rep->Autors.Get(autor.Id) << '\n';
				break;

			case DEL_S:
				std::cout << "Autor Id: ";
				std::cin >> autor.Id;
				rep->Autors.Delete(autor.Id);
				break;

			case UPD_S:
				std::cout << "Autor Id: ";
				std::cin >> autor.Id;
				std::cout << "RegionId: ";
				std::cin >> autor.RegionId;
				std::cout << "AutorName: ";
				getline(autor.AutorName, 50);
				std::cout << "Pseudonym: ";
				getline(autor.Pseudonym, 50);
				rep->Autors.Insert(autor);
				break;

			case INS_S:
				std::cout << "RegionId: ";
				std::cin >> autor.RegionId;
				std::cout << "AutorName: ";
				getline(autor.AutorName, 50);
				std::cout << "Pseudonym: ";
				getline(autor.Pseudonym, 50);
				rep->Autors.Insert(autor);
				break;

			case CALC_S:
				std::cout << rep->Autors.GetAll().size() << '\n';
				break;

			case UT_S:
				for (const auto& x : rep->Autors.GetAll())
					std::cout << x << '\n';
				break;

			case CALC_S2:
				std::cout << "RegionId: ";
				std::cin >> autor.RegionId;
				for (const auto& x : rep->Autors.GetByRegionId(autor.RegionId))
					std::cout << x << '\n';
				break;

			case INVALID:
				std::cout << "����������� �������\n";
				break;

			case HELP:
				std::cout << "������ ������:\n";

				std::cout << "\t\"get-m\" - ������� Region �� ���� Id\n";
				std::cout << "\t\"del-m\" - �������� Region �� ���� Id\n";
				std::cout << "\t\"update-m\" - ������� Region �� ���� Id\n";
				std::cout << "\t\"insert-m\" - ������ ����� Region\n";
				std::cout << "\t\"calc-m\" - ������� Region-��\n";
				std::cout << "\t\"ut-m\" - �������� �� Region-�\n";

				std::cout << "\t\"get-s\" - ������� Autor �� ���� Id\n";
				std::cout << "\t\"del-s\" - �������� Autor �� ���� Id\n";
				std::cout << "\t\"update-s\" - ������� Autor �� ���� Id\n";
				std::cout << "\t\"insert-s\" - ������ ������ Autor-�\n";
				std::cout << "\t\"calc-s\" - ������� Autor-��\n";
				std::cout << "\t\"calc-s2\" - ������� Autor-�� � ������� RegionId\n";
				std::cout << "\t\"ut-s\" - �������� ��� Autor-��\n";
				break;

			case EXIT:
				return;
			default: throw std::exception("������� �������");
			}
		}
		catch (const std::exception& exep)
		{
			std::cout << exep.what();
		}
	}
}
