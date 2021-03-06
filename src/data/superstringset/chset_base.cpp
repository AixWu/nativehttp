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
#include <stdio.h>
#include "data/queue.h"

#define SSLOCK if(lck)pos=lpos

namespace nativehttp
{
    namespace data
    {
        superstringset& superstringset::changeset_addRule(string from, string to)
        {
            _chsetrule trl;
            trl._from = from;
            trl._to = to;
            chset_rules.push_back(trl);
            return *this;
        }

        superstringset& superstringset::changeset_clean()
        {
            chset_rules.clear();
            return *this;
        }

        superstringset& superstringset::changeset_removeRule(string from, string to)
        {
            data::queue<int> to_remove;

            for(unsigned int i=0;i<chset_rules.size();i++)
            {
                if(chset_rules[i]._from == from&&chset_rules[i]._to == to)
                {
                    to_remove.push(i);
                }
            }
            while(!to_remove.empty())
            {
                chset_rules.erase(chset_rules.begin() + to_remove.front());
                to_remove.pop();
            }
            return *this;
        }

        int superstringset::changeset_numRules()
        {
            return chset_rules.size();
        }
    }
}
