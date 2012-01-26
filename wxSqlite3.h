#ifndef __WX_SQLITE3__
#define __WX_SQLITE3__

#include <sqlite3.h>


class wxSqlite3{
 public:
	wxSqlite3();
	wxSqlite3(const char* filename);
	~wxSqlite3();
	
	bool open(const char* filename);
	void close();
	void query();
	void * result();
	
private:
  sqlite3** dbhandle;	
	sqlite3_stmt** dbstmt;

};

#endif //__WX_SQLITE3__