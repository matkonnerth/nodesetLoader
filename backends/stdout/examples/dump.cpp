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

namespace mk
{

class Reference
{
  public:
    Reference(const NL_Reference *ref) : m_isForward{ref->isForward}
    {
        UA_NodeId_copy(&ref->target, &m_target);
        UA_NodeId_copy(&ref->refType, &m_refType);
    }

    const UA_NodeId &ReferenceType() const { return m_refType; }

    const UA_NodeId &Target() const { return m_target; }

  private:
    bool m_isForward;
    UA_NodeId m_target;
    UA_NodeId m_refType;
};

class Node final
{
  public:
    Node(const NL_Node *node)
        : m_nodeClass{node->nodeClass}, m_browseName{node->browseName.name}
    {
        UA_NodeId_copy(&node->id, &m_id);
        addHierachicalRefs(node);
        addNonHierachicalRefs(node);
    }

    bool operator=(const Node &other)
    {
        return UA_NodeId_equal(&m_id, &other.m_id);
    }

    const std::string &BrowseName() const { return m_browseName; }

    std::optional<UA_NodeId> TypeDefinitionId() const
    {
        if (m_nodeClass != NODECLASS_OBJECT)
        {
            return std::nullopt;
        }
        auto it = std::find_if(
            m_nonHierachicalRefs.begin(), m_nonHierachicalRefs.end(),
            [](const Reference &ref) {
                auto hasTypeDefinitionId =
                    UA_NODEID_NUMERIC(0, UA_NS0ID_HASTYPEDEFINITION);

                return UA_NodeId_equal(&ref.ReferenceType(),
                                       &hasTypeDefinitionId);
            })

            if (it != m_nonHierachicalRefs.end())
        {
            return it->Target();
        }
    }

  private:
    void addHierachicalRefs(const NL_Node *node)
    {

        addRefs(node->hierachicalRefs, m_hierachicalRefs);
    }

    void addNonHierachicalRefs(const NL_Node *node)
    {

        addRefs(node->nonHierachicalRefs, m_nonHierachicalRefs);
    }

    void addRefs(const NL_Reference *ref, std::vector<Reference> &refs)
    {
        while (ref)
        {
            refs.push_back(ref);
            ref = ref->next;
        }
    }
    NL_NodeClass m_nodeClass;
    std::string m_browseName;
    UA_NodeId m_id;
    std::vector<Reference> m_hierachicalRefs{};
    std::vector<Reference> m_nonHierachicalRefs{};
};

class MyNamespace
{
  public:
    void addNode(const NL_Node *node) { m_nodes.emplace_back(node); }

  private:
    std::vector<Node> m_nodes{};
};

} // namespace mk

void dumpNode(void *userContext, const NL_Node *node)
{
    printf("NodeId: %s BrowseName: %s DisplayName: %s\n", printId(&node->id),
           node->browseName.name, node->displayName.text);

    switch (node->nodeClass)
    {
    case NODECLASS_OBJECT:
        printf("\tparentNodeId: %s\n",
               printId(&((const NL_ObjectNode *)node)->parentNodeId));
        printf("\teventNotifier: %s\n",
               ((const NL_ObjectNode *)node)->eventNotifier);
        break;
    case NODECLASS_VARIABLE:
        printf("\tparentNodeId: %s\n",
               printId(&((const NL_VariableNode *)node)->parentNodeId));
        printf("\tdatatype: %s\n",
               printId(&((const NL_VariableNode *)node)->datatype));
        printf("\tvalueRank: %s\n", ((const NL_VariableNode *)node)->valueRank);
        printf("\tarrayDimensions: %s\n",
               ((const NL_VariableNode *)node)->valueRank);
        break;
    case NODECLASS_OBJECTTYPE:
        break;
    case NODECLASS_DATATYPE:
        break;
    case NODECLASS_METHOD:
        break;
    case NODECLASS_REFERENCETYPE:
    case NODECLASS_VARIABLETYPE:
        break;
    }
    NL_Reference *hierachicalRef = node->hierachicalRefs;
    while (hierachicalRef)
    {
        printf("\treftype: %s", printId(&hierachicalRef->refType));
        printf(" target: %s\n", printId(&hierachicalRef->target));
        hierachicalRef = hierachicalRef->next;
    }

    NL_Reference *nonHierRef = node->nonHierachicalRefs;
    while (nonHierRef)
    {
        printf("\treftype: %s", printId(&nonHierRef->refType));
        printf(" target: %s\n", printId(&nonHierRef->target));
        nonHierRef = nonHierRef->next;
    }
}
