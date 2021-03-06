/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include "nativehttp.h"
#include "queue.h"
#include <string.h>
#include <cstdarg>

void nativehttp::base::SQLite::open(const char *file, bool fast)
{
	emsg = NULL;
	sqlite3_open(file, &db);
	if (fast)
	{
		sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &emsg);
		sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &emsg);
	}
}

const char *nativehttp::base::SQLite::getLastError()
{
	return emsg;
}

nativehttp::base::SQLiteResult nativehttp::base::SQLite::exec(const char *q)
{
	sqlite3_stmt *statement;
	unsigned int cols = 0;
	data::queue<char **> dat;


	if (sqlite3_prepare_v2(db, q, -1, &statement, 0) == SQLITE_OK)
	{
		cols = sqlite3_column_count(statement);
		int result = 0;
		while (true)
		{
			result = sqlite3_step(statement);

			if (result == SQLITE_ROW)
			{
				char **row = new char*[cols];
				for (unsigned int col = 0; col < cols; col++)
				{
					char *s = (char*)sqlite3_column_text(statement, col);
					if (s)
					{
						row[col] = new char[strlen(s) + 1];
						strcpy(row[col], s);
					}
					else
					{
						row[col] = NULL;
					}
				}
				dat.push(row);
			}
			else
			{
				break;
			}
		}

		sqlite3_finalize(statement);
	}

	char *** rtd = new char **[dat.size()];
	unsigned int ds = dat.size();
	for (unsigned int i = 0; i < ds; i++)
	{
		rtd[i] = dat.front();
		dat.pop();
	}

	nativehttp::base::SQLiteResult rt;
	rt.__set(cols, ds, rtd, this);
	return rt;

}

nativehttp::base::SQLiteResult::SQLiteResult()
{
	dt = NULL;
	cols = 0;
	rows = 0;
	from = NULL;
}

void nativehttp::base::SQLite::transaction_start()
{
	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &emsg);
}

void nativehttp::base::SQLite::transaction_done()
{
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &emsg);
}

void nativehttp::base::SQLiteResult::__set(unsigned int c, unsigned int r, char *** d, nativehttp::base::SQLite *clr)
{
	cols = c;
	rows = r;
	dt = d;
	from = clr;
}

void nativehttp::base::SQLiteResult::free()
{
	for (unsigned int i = 0; i < rows && dt; i++)
	{
		for (unsigned int j = 0; j < cols && dt[i]; j++)
		{
			if (dt[i][j])delete[] dt[i][j];
			dt[i][j] = NULL;
		}
		if (dt[i])delete[] dt[i];
		dt[i] = NULL;
	}
	if (dt)delete[] dt;
	dt = NULL;
}

char **nativehttp::base::SQLiteResult::operator[](int i)
{
	if (dt)return dt[i];
	return NULL;
}

unsigned int nativehttp::base::SQLiteResult::numRows()
{
	return rows;
}

bool nativehttp::base::SQLite::isTable(const char *name)
{
	bool rt = false;
	string qry = "SELECT name FROM sqlite_master WHERE type='table' AND name='";
	qry += name;
	qry += "';";

	nativehttp::base::SQLiteResult tr = this->exec(qry.c_str());

	rt = (tr.numRows() > 0);
	tr.free();

	return rt;
}

void nativehttp::base::SQLite::create_table(const char *name, unsigned int cols, ...)
{
	va_list cll;
	va_start(cll, cols);
	string req = "CREATE TABLE IF NOT EXISTS '";
	req += name;
	req += "' (";
	for (unsigned int i = 0; i < cols; i++)
	{
		nativehttp::base::SQLiteCol tc = va_arg(cll, nativehttp::base::SQLiteCol);
		req += "'";
		req += tc.name;
		req += "' ";

		switch (tc.type)
		{
			case nativehttp::base::SLC_NULL:
				req += "NULL";
				break;
			case nativehttp::base::SLC_INTEGER:
				req += "INTEGER";
				break;
			case nativehttp::base::SLC_REAL:
				req += "REAL";
				break;
			case nativehttp::base::SLC_TEXT:
				req += "TEXT";
				break;
			case nativehttp::base::SLC_BLOB:
				req += "BLOB";
				break;
			default:
				return;
		}

		if (i + 1 < cols)req += ", ";
	}
	req += ");";
	va_end(cll);

	this->exec(req.c_str()).free();

}

void nativehttp::base::SQLite::create_table(const char *name, unsigned int cols, SQLiteCol *cl)
{
	string req = "CREATE TABLE IF NOT EXISTS '";
	req += name;
	req += "' (";
	for (unsigned int i = 0; i < cols; i++)
	{
		nativehttp::base::SQLiteCol tc = cl[i];
		req += "'";
		req += tc.name;
		req += "' ";

		switch (tc.type)
		{
			case nativehttp::base::SLC_NULL:
				req += "NULL";
				break;
			case nativehttp::base::SLC_INTEGER:
				req += "INTEGER";
				break;
			case nativehttp::base::SLC_REAL:
				req += "REAL";
				break;
			case nativehttp::base::SLC_TEXT:
				req += "TEXT";
				break;
			case nativehttp::base::SLC_BLOB:
				req += "BLOB";
				break;
			default:
				return;
		}

		if (i + 1 < cols)req += ", ";
	}
	req += ");";

	this->exec(req.c_str()).free();
}

nativehttp::base::SQLiteCol::SQLiteCol()
{
	name = NULL;
	type = nativehttp::base::SLC_NULL;
}

nativehttp::base::SQLiteCol::SQLiteCol(const char *n, SQLite_ctype t)
{
	name = n;
	type = t;
}

struct slblt
{
	string f;
	nativehttp::base::SQLite *p;
};
vector<slblt>slopls;

extern "C" nativehttp::base::SQLite *sqlite_open(string file, bool fast)
{
	for (unsigned int i = 0; i < slopls.size(); i++)
	{
		if (slopls[i].f == file)
		{
			return slopls[i].p;
		}
	}
	slblt ttm = {file, new nativehttp::base::SQLite};
	ttm.p->open(file.c_str(), fast);
	if (ttm.p)
	{
		slopls.push_back(ttm);
	}
	return ttm.p;
}


