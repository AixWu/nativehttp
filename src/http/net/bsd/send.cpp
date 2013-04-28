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
#include "../net.h"
#include "http/stat.h"

int nhSend(SOCKET sock, const void *data, int len)
{
    const char* dp=(const char*)data;
    int sent, left;

	left = len;
	sent = 0;
	errno = 0;
	do {
		len = send(sock, (const char *)dp, left, http::asyncsnd?MSG_DONTWAIT:0);
		if ( len > 0 ) {
			sent += len;
			left -= len;
			dp += len;
		}
	} while ( (left > 0) && ((len > 0) || (errno == EINTR)) );

	return(sent);
}

namespace http
{
namespace bsd
{
void* sender(void* unused)
{
    int ts=0;
    while(1)
    {
        if(http::tosend.empty())
        {
            SDL_Delay(1);
            continue;
        }

        SDL_mutexP(http::mtx_snd);

        outdata proc=http::tosend.front(ts);
        if(ts==1)
        {
            SDL_mutexV(http::mtx_snd);
            continue;
        }
        http::tosend.pop();
        SDL_mutexV(http::mtx_snd);

        if(http::connected[proc.uid])
        {

            nhSend(http::connected[proc.uid],proc.data,proc.size);
            http::statdata::onsend(proc.size);
        }


        if(proc.fas)
        {
            delete[] proc.data;
        }
    }
    return NULL;
}

void sendNow(int uid, unsigned long datasize, char* data, bool free)
{
    nhSend(http::connected[uid],data,datasize);
    if(free)
    {
        delete[] data;
    }
}
}
}