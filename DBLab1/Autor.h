#pragma once
struct Autor
{
	long Id;
	long RegionId;
	char AutorName[50];
	char Pseudonym[50];

	static constexpr size_t size = sizeof(Id) + sizeof(RegionId) + sizeof(AutorName) + sizeof(Pseudonym);
};

