#pragma once

#include "Repository.h"

class App
{
	enum Command
	{
		GET_M,	DEL_M,	UPD_M,	INS_M,	CALC_M,	UT_M,
		GET_S,	DEL_S,	UPD_S,	INS_S,	CALC_S,	UT_S,
		CALC_S2, INVALID, HELP, EXIT
	};

	Command ToCommand(const std::string& strCommand);

	Repository::FileRepository rep;
public:

	void Run();
};

