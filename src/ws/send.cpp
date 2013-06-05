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
#include "ws.h"
#include "http/net/net.h"

namespace ws
{
    void chunked_send(int uid, const void* data, uint64_t dsize)
    {
        for(uint64_t i=0;i<dsize;i+=ws::max_sender_chunk)
        {
            uint64_t sna = dsize - i;
            if(sna > ws::max_sender_chunk)sna = ws::max_sender_chunk;
            unsigned char* stmp = new unsigned char[sna];
            memcpy(stmp, data, sna);
            http::send(uid, sna, (const char*)stmp, true);
        }
    }
}


namespace nativehttp
{
    namespace websock
    {
        int sendBin(nativehttp::data::clientid uid, const void* data, uint64_t size)
        {
            if(ws::enabled)
            {
                uint64_t dsize = size;
                if(dsize > ws::max_sendable)
                {
                    return NH_ERROR;
                }

                unsigned int head_size = 2;
                if(dsize > 125)head_size+=2;
                if(dsize > 65535)head_size+=6;//2+6 = 8

                unsigned char* sbuf = new unsigned char[head_size + dsize];
                sbuf[0] = 0b10000001;

                if(dsize < 126)
                {
                    sbuf[1] = (uint8_t)dsize;
                }
                else if(dsize < 65536)
                {
                    sbuf[1] = uint8_t(126);

                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        *((uint16_t*)(sbuf+2)) = (uint16_t)dsize;
                    }
                    else
                    {
                        *((uint8_t*)(sbuf+2)) = ((uint8_t*)(&dsize))[1];
                        *((uint8_t*)(sbuf+3)) = ((uint8_t*)(&dsize))[0];
                    }
                }
                else
                {
                    sbuf[1] = uint8_t(127);

                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        *((uint64_t*)(sbuf+2)) = dsize;
                    }
                    else
                    {
                        *((uint8_t*)(sbuf+2)) = ((uint8_t*)(&dsize))[7];
                        *((uint8_t*)(sbuf+3)) = ((uint8_t*)(&dsize))[6];
                        *((uint8_t*)(sbuf+4)) = ((uint8_t*)(&dsize))[5];
                        *((uint8_t*)(sbuf+5)) = ((uint8_t*)(&dsize))[4];
                        *((uint8_t*)(sbuf+6)) = ((uint8_t*)(&dsize))[3];
                        *((uint8_t*)(sbuf+7)) = ((uint8_t*)(&dsize))[2];
                        *((uint8_t*)(sbuf+8)) = ((uint8_t*)(&dsize))[1];
                        *((uint8_t*)(sbuf+9)) = ((uint8_t*)(&dsize))[0];
                    }
                }

                memcpy(sbuf + head_size, data, dsize);

                ws::chunked_send(uid, sbuf, head_size + dsize);
                delete[] sbuf;

            }
            else
            {
                return NH_ERROR;
            }
            return NH_SUCCES;
        }

        int sendTxt(nativehttp::data::clientid uid, const char* data)
        {
            if(ws::enabled)
            {
                uint64_t dsize = strlen(data);
                if(dsize > ws::max_sendable)
                {
                    return NH_ERROR;
                }

                unsigned int head_size = 2;
                if(dsize > 125)head_size+=2;
                if(dsize > 65535)head_size+=6;//2+6 = 8

                unsigned char* sbuf = new unsigned char[head_size + dsize];
                sbuf[0] = 0b10000001;

                if(dsize < 126)
                {
                    sbuf[1] = (uint8_t)dsize;
                }
                else if(dsize < 65536)
                {
                    sbuf[1] = uint8_t(126);

                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        *((uint16_t*)(sbuf+2)) = (uint16_t)dsize;
                    }
                    else
                    {
                        *((uint8_t*)(sbuf+2)) = ((uint8_t*)(&dsize))[1];
                        *((uint8_t*)(sbuf+3)) = ((uint8_t*)(&dsize))[0];
                    }
                }
                else
                {
                    sbuf[1] = uint8_t(127);

                    if(IS_MACHINE_NETWORK_BYTE_ORDERED)
                    {
                        *((uint64_t*)(sbuf+2)) = dsize;
                    }
                    else
                    {
                        *((uint8_t*)(sbuf+2)) = ((uint8_t*)(&dsize))[7];
                        *((uint8_t*)(sbuf+3)) = ((uint8_t*)(&dsize))[6];
                        *((uint8_t*)(sbuf+4)) = ((uint8_t*)(&dsize))[5];
                        *((uint8_t*)(sbuf+5)) = ((uint8_t*)(&dsize))[4];
                        *((uint8_t*)(sbuf+6)) = ((uint8_t*)(&dsize))[3];
                        *((uint8_t*)(sbuf+7)) = ((uint8_t*)(&dsize))[2];
                        *((uint8_t*)(sbuf+8)) = ((uint8_t*)(&dsize))[1];
                        *((uint8_t*)(sbuf+9)) = ((uint8_t*)(&dsize))[0];
                    }
                }

                memcpy(sbuf + head_size, data, dsize);

                ws::chunked_send(uid, sbuf, head_size + dsize);
                delete[] sbuf;

            }
            else
            {
                return NH_ERROR;
            }
            return NH_SUCCES;
        }
    }
}
