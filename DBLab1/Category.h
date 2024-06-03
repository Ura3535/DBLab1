#pragma once
struct Category
{
	long Id;
	char Name[50];

	static constexpr size_t size
		= sizeof(Id)
		+ sizeof(Name);
};

