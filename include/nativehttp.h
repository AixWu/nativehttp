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
#ifndef NATIVEHTTP_H_INCLUDED
#define NATIVEHTTP_H_INCLUDED

#define NATIVEHTTP_API_VERSION 14
/*
NOTE, that API version macro MUST be written with following style:
#define[space - ASCII 32]NATIVEHTTP_API_VERSION[space - ASCII 32][Version - ASCII number(That Cpp compiller will see as normal integer)][newline]
*/

#include <string>
#include <vector>
#include <deque>
#include <stdio.h>
#include <sqlite3.h>

#define initated() return -(NATIVEHTTP_API_VERSION)
#define initerr(_rcode) return ((_rcode>0)?(_rcode):(-(_rcode)))+2

#define NH_SUCCES 0
#define NH_ERROR 1

using namespace std;

namespace nativehttp
{

	namespace data
	{

		struct token
		{
			int id;
			string s;
			token(string st, int i);
		};

		class superstring
		{
			private:

				vector<token>tokens;
				bool rae;
				bool lck;
				unsigned int lpos;

			public:

				unsigned int pos;/** Position */
				string str;/** Container string, DO NOT OVERUSE THIS, the type MAY CHANGE in future API versions */


				/** Core functions*/
				~superstring();
				superstring();

				superstring& lock();
				superstring& lock(unsigned int lp);
				superstring& unlock();

				bool atbegin();
				bool atend();

				size_t length();
				size_t size();

				void clear();
				const char* c_str();

				superstring& go_begin();
				superstring& go_end();
				superstring& go_begin(int p);
				superstring& go_end(int p);
				superstring& go_pos(int p);

				char& operator[](unsigned int i);


				/** C++ string functions*/
				superstring(string s);
				void set(string s);
				void operator()(string s);

                string skip(string stb);
                string tochar(string fend);
                bool contain(string stb);
                bool contain_not(string stb);
                bool contain_only(string stb);
				string to(string fend);
				string back_to(string fend);
				string from(string start);
				unsigned int find(string str);

				int check(string sch);
				string get(unsigned int n);
				string get_begin(unsigned int n);
				string get_end(unsigned int n);

				superstring& change(string from, string to);
				superstring& remove(string from, string to);
				superstring& remove(string s);

                static int int_from_str(string in);

				/** SuperString functions*/
				void set(superstring s);
				void operator()(superstring s);
				superstring operator+=(superstring in);

                superstring skip(superstring stb);
                superstring tochar(superstring fend);
                bool contain(superstring stb);
                bool contain_not(superstring stb);
                bool contain_only(superstring stb);
				superstring to(superstring fend);
				superstring back_to(superstring fend);
				superstring from(superstring start);

				/** Integer functions*/
				void set(int s);
				void operator()(int s);
				superstring operator*=(unsigned int rc);

				static string str_from_int(int in);
				static superstring sst_from_int(int in);

				string from(unsigned int sp);
				string to(unsigned int ep);

				/** 64bit integer functions */
                static string str_from_int64(int64_t in);
				static superstring sst_from_int64(int64_t in);
                static int64_t int64_from_sst(superstring in);
                static int64_t int64_from_str(string in);

				/** Size functions */
				static string str_from_size(size_t in);
				static superstring sst_from_size(size_t in);

				/** Double precission functions */
				static string str_from_double(double in);
				static superstring sst_from_double(double in);

				/** Token functions*/
				superstring& add_token(token t);
				void clear_tokens();
				token tok();
				token tok(string &opt);
				size_t num_tokens();


				/** Character functions*/
				int count(const char c);


				/** File functions*/
				static string file(string fn);
				void set_file(string fn);
				void append_file(string fn);

                static int file(string fn, string content);
                static int str_to_file(string fn, string content);
                static int sst_to_file(string fn, superstring content);
                static int append_str_file(string fn, string append);
                static int append_sst_file(string fn, superstring append);

		};

		typedef superstring ss;
		typedef int clientid;

		struct cfgfil
		{

			string name;
			string cont;

		};

		class Ccfg
		{
			private:

				string flie;
				vector<cfgfil>fileds;

			public:

				void parse_file(const char *f);
				string get_var(string name);
				int get_int(string name);
				Ccfg(string f)
				{
					parse_file(f.c_str());
				}
				~Ccfg()
				{
					fileds.clear();
				}

		};

		struct pagedata
		{
			pagedata(string s);
			void operator()(string s);
			pagedata();
			char *data;
			size_t size;
		};

		struct cfil
		{
			string name;
			string value;
		};

		class cookiedata
		{
			private:
				vector<cfil>data;
				string sets;
			public:
				cookiedata(string strcookie);
				string get(string name);
				void set(string name, string value);
				void set(string name, string value, string attributes);
				string gethead();
		};

		class postgetdata
		{
			private:
				vector<cfil>data;
			public:
				postgetdata(string dstr);
				string get(string name);
		};

		class session
		{
			private:
				unsigned int ssid;
				bool valid;
			public:
				void __init(cookiedata *cd, nativehttp::data::clientid uid);
				void __mkinv(nativehttp::data::clientid uid);
				string get(string name);
				void set(string name, string value);
				bool is_valid();
				static session* by_uid(nativehttp::data::clientid uid);
		};

	}//data namespace

	struct rdata
	{
		nativehttp::data::cookiedata *cookie;
		nativehttp::data::postgetdata *get;
		nativehttp::data::postgetdata *post;
		nativehttp::data::session *session;
		string response;
		string userAgent;
		string referer;
		string host;
		string uri;
		unsigned int remoteIP;
		string ctype;
	};

#define NB_VERSION 1

#define NB_ERROR 0
#define NB_SUCCES 1

	namespace base
	{

		typedef bool (*nbfilter)(string*, void*);

		class nbrow
		{
			public:
				string *data;
				~nbrow();
		};

		class nbtable
		{
			public:
				int cn;
				string *cols;
				vector<nbrow*>data;
				string name;
				nbtable(string n, string *cl, int cnb);
				~nbtable();
				int ins(string *in);
		};

		class nbresult
		{
			public:
				vector<string*>res;
				int count();
				int filter(nbfilter flt, void *fltdata);

				string *operator[](int rowid);
		};

		class nbase
		{
			private:
				vector<nbtable*>data;
				string bfn;
			public:
				nbase(string file);
				int createtable(string name, string *columns, int cnum);
				int insert(string tabname, string *rowdata);
				int del(string tabname, nbfilter flt, void *fltdata);
				int save();
				int istable(string name);
				nbresult operator()(string table);
				nbresult operator()(string table, nbfilter flt, void *fltdata);
		};

		extern "C" nbase *nbase_open(string file);

		typedef class SQLite SQLite;

		class SQLiteResult
		{
			private:
				unsigned int cols;
				unsigned int rows;
				char *** dt;
				nativehttp::base::SQLite *from;
			public:

				SQLiteResult();
				void __set(unsigned int c, unsigned int r, char *** d, nativehttp::base::SQLite *clr);

				void free();

				unsigned int numRows();
				char **operator[](int);

		};

		enum SQLite_ctype
		{
		    SLC_NULL,
		    SLC_INTEGER,
		    SLC_REAL,
		    SLC_TEXT,
		    SLC_BLOB
		};

		struct SQLiteCol
		{
			SQLiteCol();
			SQLiteCol(const char *n, SQLite_ctype t);
			const char *name;
			SQLite_ctype type;
		};

		class SQLite
		{
			private:

				sqlite3 *db;
				char *emsg;

			public:

				const char *getLastError();
				void open(const char *file, bool fast);
				SQLiteResult exec(const char *q);

				void transaction_start();
				void transaction_done();

				void create_table(const char *name, unsigned int cols, ...);
				void create_table(const char *name, unsigned int cols, SQLiteCol *cl);

				bool isTable(const char *name);

		};

		extern "C" SQLite *sqlite_open(string file, bool fast);

	}//base namespace

	namespace init
	{
		extern "C" void attach_uri(string uri, bool top);
	}

	namespace server
	{
		extern "C" string version();
		extern "C" void log(string lname, string value);
		extern "C" void logid(int id, string lname, string value);

		extern "C" data::clientid maxClients();

		extern "C" bool is_ssl();

		namespace stat
		{
			extern "C" bool stats();
			extern "C" bool transfer_stats();
			extern "C" bool hit_stats();
			extern "C" bool method_stats();

			extern "C" unsigned long long hits();
			extern "C" unsigned long long connections();
			extern "C" unsigned long long uploaded();
			extern "C" unsigned long long downloaded();

			extern "C" unsigned long long get_requests();
			extern "C" unsigned long long post_requests();

			extern "C" unsigned long long hourly_length();

			extern "C" unsigned long long hour_hits(unsigned long hid);
			extern "C" unsigned long long hour_connections(unsigned long hid);
			extern "C" unsigned long long hour_upload(unsigned long hid);
			extern "C" unsigned long long hour_download(unsigned long hid);

		}
	}

	namespace utils
	{
		extern "C" string decode_poststring(string str);
	}

	namespace websock
	{
        typedef int (*cb_onConnect)(nativehttp::data::clientid);
        typedef int (*cb_onDisconnect)(nativehttp::data::clientid);
        typedef int (*cb_onBinMsg)(nativehttp::data::clientid, const void*, uint64_t);
        typedef int (*cb_onTxtMsg)(nativehttp::data::clientid, const char*);

        int add(const char* uri, const char* protocol_name, cb_onConnect onConnect, cb_onDisconnect onDisconnect, cb_onTxtMsg onTMsg, cb_onBinMsg onBMsg);

        int disconnect(nativehttp::data::clientid uid);

        int sendTxt(nativehttp::data::clientid uid, const char* data);
        int sendBin(nativehttp::data::clientid uid, const void* data, uint64_t size);

        int streamTxt(nativehttp::data::clientid uid, const char* data, bool finalize);
        int streamBin(nativehttp::data::clientid uid, const void* data, uint64_t size, bool finalize);


	}

}//nativehttp namespace

namespace nh = nativehttp;
namespace nd = nativehttp::data;
namespace nb = nativehttp::base;
namespace ns = nativehttp::server;
namespace nw = nativehttp::websock;

#endif // NATIVEHTTP_H_INCLUDED
