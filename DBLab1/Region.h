#pragma once
struct Region
{
	long Id;
	char RegionName[50];

	static constexpr size_t size = sizeof(Id) + sizeof(RegionName);
};

