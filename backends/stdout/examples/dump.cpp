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

unsigned short addNamespace(void *userContext, const char *uri) { return 1; }

void dumpNode(void *userContext, const NL_Node *node)
{
    auto myNamespace = static_cast<nodesetLoader::Namespace*>(userContext);
    myNamespace->addNode(node);
}
