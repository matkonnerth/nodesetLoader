/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *    Copyright 2019 (c) Matthias Konnerth
 */

#include "backend.h"
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include "Node.h"

char nodeidDump[256];
static char *printId(const UA_NodeId *id)
{
    UA_String myStr = {0};
    UA_NodeId_print(id, &myStr);
    if (myStr.length >= 256)
        myStr.length = 255;
    memcpy(nodeidDump, myStr.data, myStr.length);
    nodeidDump[myStr.length] = 0;
    UA_String_clear(&myStr);
    return nodeidDump;
}

unsigned short addNamespace(void *userContext, const char *uri) { return 1; }

nodesetLoader::Namespace myNamespace{};

void dumpNode(void *userContext, const NL_Node *node)
{
    myNamespace.addNode(node);
}
