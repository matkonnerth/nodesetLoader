#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <iostream>

namespace nodesetLoader
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

    bool IsForward() const
    {
        return m_isForward;
    }

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
    const UA_NodeId& Id() const { return m_id;}

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
            });

            if (it != m_nonHierachicalRefs.end())
        {
            return it->Target();
        }
    }

    const std::vector<Reference>& HierachicalRefs() const
    {
        return m_hierachicalRefs;
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

using OptionalNodeRef = std::optional<std::reference_wrapper<const Node>>;

class Namespace
{
  public:
    void addNode(const NL_Node *node) { m_nodes.emplace_back(node); }

    const OptionalNodeRef getNode(const UA_NodeId& id) const
    {
        auto it =
            std::find_if(m_nodes.begin(),
                         m_nodes.end(), [&id](const Node &n) {
                             return UA_NodeId_equal(&id,
                                                    &n.Id());
                         });

        if (it != m_nodes.end())
        {
            return *it;
        }
        return std::nullopt;
    }

  private:
    std::vector<Node> m_nodes{};
};

class HierachicalVisitor
{
public:
    HierachicalVisitor(const Namespace& ns): m_ns{ns}{}

    void visit(const UA_NodeId& id)
    {
        auto node = m_ns.getNode(id);
        if(node)
        {
            std::cout << node->get().BrowseName() << "\n";
            for(const auto& ref : node->get().HierachicalRefs())
            {
                if(ref.IsForward())
                {
                    visit(ref.Target());
                }
            }
        }
    }

private:
    const Namespace& m_ns;
};

} // namespace nodesetLoader