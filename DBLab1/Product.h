#pragma once
struct Product
{
	long Id;
	char Name[50];
	long Price;
	long CategoryId;
	char Description[50];

	static constexpr size_t size
		= sizeof(Id)
		+ sizeof(Name)
		+ sizeof(Price)
		+ sizeof(CategoryId)
		+ sizeof(Description);
};

