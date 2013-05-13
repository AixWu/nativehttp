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
#include "protocol.h"
#include "../executor.h"
#include "../data.h"
#include "../stat.h"
#include "../net/net.h"

#ifdef NHDBG
#include "utils/memory.h"
#endif

namespace http
{

	void *executor(void *eid)
	{
		http::Sexecutor *exc = (http::Sexecutor*)eid;
		exc->state = -1;
		nativehttp::rdata rd;
		http::rproc::lrqd ld;
		int ts = 0;

		prctl(PR_SET_NAME, ("nh-exec-" + nativehttp::data::superstring::str_from_int(exc->id)).c_str(), 0, 0, 0);

		while (true)
		{
			SDL_mutexP(http::mtx_exec);
			if (http::toexec.size() <= 0)
			{
				SDL_mutexV(http::mtx_exec);
				SDL_Delay(1);
				continue;
			}
			if (http::toexec.front(ts)->taken > 0)
			{
				SDL_mutexV(http::mtx_exec);
				SDL_Delay(1);
				continue;
			}
			if (ts == 1)
			{
				SDL_mutexV(http::mtx_exec);
				SDL_Delay(1);
				continue;
			}
			http::toexec.front()->taken = exc->id;
			http::request *process = http::toexec.front(ts);
			if (ts == 1)
			{
				SDL_mutexV(http::mtx_exec);
				SDL_Delay(1);
				continue;
			}


			http::toexec.pop();
			SDL_mutexV(http::mtx_exec);

			http::statdata::onhit();

			rd.get = NULL;
			rd.post = NULL;
			rd.cookie = NULL;
			rd.session = NULL;

			ld.clen = 0;
			ld.d501 = 0;

			http::rproc::line0(process, rd, ld);

			switch (process->method)
			{
				case 1:
					http::statdata::onget();
					break;
				case 2:
					http::statdata::onpost();
					break;
			}

			if (process->method == 0)
			{

				http::send(process->uid, http::error::e400.size, http::error::e400.data, false);
				http::unlockclient(process->uid);
				delete[] process->request;
				continue;
			}
			if (process->method == 3)
			{
				http::send(process->uid, http::error::e501.size, http::error::e501.data, false);
				http::unlockclient(process->uid);
				delete[] process->request;
				continue;
			}
			if (!process->http11)
			{
				http::send(process->uid, http::error::e505.size, http::error::e505.data, false);
				http::unlockclient(process->uid);
				delete[] process->request;
				delete process;
				continue;
			}

			http::rproc::header(process, rd, ld);

            if(ld.d501)
            {
                if (rd.cookie)
                {
                    delete rd.cookie;
                }
                if (rd.get)
                {
                    delete rd.get;
                }
                if (rd.post)
                {
                    delete rd.post;
                }

                http::send(process->uid, http::error::e501.size, http::error::e501.data, false);
				http::unlockclient(process->uid);
				delete process;
				process = NULL;
				continue;
            }

			if (!rd.cookie)
			{
				rd.cookie = new nativehttp::data::cookiedata("");
			}

			if (ld.clen > 0 && process->method == 2)
			{
				if (ld.clen > http::maxPost)
				{
					if (rd.cookie)
					{
						delete rd.cookie;
					}
					if (rd.get)
					{
						delete rd.get;
					}
					if (rd.post)
					{
						delete rd.post;
					}
					exc->fd1 = NULL;
					exc->fd2 = NULL;
					exc->state = time(0);
					exc->in = 3;
					http::bsd::sendNow(process->uid, http::error::e403.size, http::error::e403.data, false);
					exc->state = -1;
					exc->in = 0;
					http::kickclient(process->uid);
					delete[] process->request;
					delete process;
					continue;
				}
				exc->fd1 = rd.cookie;
				exc->fd2 = rd.get;
				exc->state = time(0);
				exc->in = 1;
				http::rproc::post(rd, process, ld);
				exc->state = -1;
				exc->in = 0;
				if (!rd.post)
				{
					delete[] process->request;
					delete process;
					process->request = NULL;
					if (rd.cookie)
					{
						delete rd.cookie;
					}
					if (rd.get)
					{
						delete rd.get;
					}
					if (rd.post)
					{
						delete rd.post;
					}
					http::unlockclient(process->uid);
					continue;//will be disconnected
				}
			}
			delete[] process->request;
			process->request = NULL;

			rd.remoteIP = http::client_ips[process->uid];

			if (rd.cookie && http::usesessions)
			{
				rd.session = new nativehttp::data::session;
				rd.session->__init(rd.cookie);
			}

			nativehttp::data::pagedata result;
			exc->fd1 = rd.cookie;
			exc->fd2 = rd.get;
			exc->in = 2;
			exc->state = time(0);
			if (http::rproc::ex(result, &rd))
			{
				exc->state = -1;
				exc->in = 0;
				if (rd.cookie)
				{
					delete rd.cookie;
				}
				if (rd.get)
				{
					delete rd.get;
				}
				if (rd.post)
				{
					delete rd.post;
				}
				http::send(process->uid, http::error::e404.size, http::error::e404.data, false);
				http::unlockclient(process->uid);
				delete process;
				process = NULL;
				continue;
			}

			exc->state = -1;
			exc->in = 0;

			if (rd.cookie)
			{
				delete rd.cookie;
			}
			if (rd.get)
			{
				delete rd.get;
			}
			if (rd.post)
			{
				delete rd.post;
			}

			if (result.data)
			{
				http::send(process->uid, result.size, result.data, true);
			}

			http::unlockclient(process->uid);
			delete process;
			process = NULL;
		}
		return NULL;
	}
}