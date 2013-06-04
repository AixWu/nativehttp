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
#include "protocol.h"

namespace ws
{
    void init()
    {
        ws::enabled = cfg->get_int("ws_enable");
        if(ws::enabled)
        {
            if(cfg->get_int("ws_nuri_max")>0)
            {
                ws::units = new data::vector<ws_ent>(cfg->get_int("ws_nuri_max"));
            }
            else
            {
                ws::enabled = false;
                return;
            }

            ws::mbufsize = cfg->get_int("ws_in_msg_buf");

            ws::rbuf = new unsigned char*[http::maxConnections];
            ws::rcv_msg_size = new uint64_t[http::maxConnections];
            ws::rcv_frame_left = new uint64_t[http::maxConnections];
            for(int i=0;i<http::maxConnections;i++)
            {
                ws::rbuf[i] = NULL;
                ws::rcv_msg_size[i] = 0;
                ws::rcv_frame_left[i] = 0;
            }
        }
    }
}
