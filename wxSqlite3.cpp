#include <cstdio>
#include "wxSqlite3.h"

wxSqlite3::wxSqlite3()
{
	dbhandle = nullptr;		
		
}

wxSqlite3::wxSqlite3(const char* filename)
{
	dbhandle = nullptr;
	this->open(filename);
}

wxSqlite3::~wxSqlite3()
{
	sqlite3_close(*dbhandle);
	
}

bool wxSqlite3::open(const char* filename)
{
	int isAlreadyOpened;
	if(dbhandle != nullptr)
	{
		isAlreadyOpened = sqlite3_close(*dbhandle);
		if (isAlreadyOpened == SQLITE_BUSY)
			return false;
	}
	int isOpened=sqlite3_open(filename, dbhandle);
	if(isOpened==SQLITE_OK)
		return true;
	else
	{
		const char* errmsg = sqlite3_errmsg(*dbhandle);
		printf("%s\n", errmsg);
		return false;
	}
		return false;		
	
}