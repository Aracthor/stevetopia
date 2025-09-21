#include "Pathfinding.hpp"

#include <algorithm>
#include <set>
#include <unordered_map>

#include "hatcher/assert.hpp"

namespace
{
struct NodeValueSorter
{
    using NodeCosts = std::unordered_map<const Pathfinding::Node*, float>;

    const glm::vec2 endPos;
    NodeCosts& costs;

    NodeValueSorter(glm::vec2 endPos, NodeCosts& costs)
        : endPos(endPos)
        , costs(costs)
    {
    }

    int GetNodeValue(const Pathfinding::Node* node) const
    {
        HATCHER_ASSERT(costs.find(node) != costs.end());
        return costs.at(node) + glm::length(node->pos - endPos);
    }

    bool operator()(const Pathfinding::Node* nodeA, const Pathfinding::Node* nodeB) const
    {
        const int nodeAValue = GetNodeValue(nodeA);
        const int nodeBValue = GetNodeValue(nodeB);
        return nodeAValue < nodeBValue;
    };
};
using NodeSet = std::multiset<const Pathfinding::Node*, NodeValueSorter>;
} // namespace

bool Pathfinding::ContainsNode(glm::vec2 position) const
{
    return FindNodeByPosition(position) != nullptr;
}

void Pathfinding::CreateNode(glm::vec2 position)
{
    m_nodes.emplace_back(new Node({.pos = position}));
}

void Pathfinding::LinkNodes(glm::vec2 positionA, glm::vec2 positionB)
{
    Node* nodeA = FindNodeByPosition(positionA);
    Node* nodeB = FindNodeByPosition(positionB);
    HATCHER_ASSERT(nodeA);
    HATCHER_ASSERT(nodeB);
    HATCHER_ASSERT(std::find(nodeA->links.begin(), nodeA->links.end(), nodeB) == nodeA->links.end());
    nodeA->links.emplace_back(nodeB);
}

void Pathfinding::DeleteNode(glm::vec2 position)
{
    Node* node = FindNodeByPosition(position);
    HATCHER_ASSERT(node);
    for (auto& neighbour : node->links)
    {
        neighbour->links.erase(std::find(neighbour->links.begin(), neighbour->links.end(), node));
    }

    auto NodeIsAtPosition = [position](const unique_ptr<Node>& node) { return node->pos == position; };
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), NodeIsAtPosition);
    m_nodes.erase(it);
}

std::vector<glm::vec2> Pathfinding::GetPath(glm::vec2 startPos, glm::vec2 endPos, float distance) const
{
    const Node* startNode = FindNodeByPosition(startPos);
    if (!startNode)
        return {};

    std::unordered_map<const Pathfinding::Node*, const Pathfinding::Node*> previous;
    NodeValueSorter::NodeCosts nodeCosts;
    NodeValueSorter sorter = NodeValueSorter(endPos, nodeCosts);
    NodeSet toVisit(sorter);
    nodeCosts[startNode] = 0.f;
    previous[startNode] = nullptr;
    toVisit.insert(startNode);

    while (!toVisit.empty() && glm::distance((*toVisit.begin())->pos, endPos) > distance)
    {
        const Node* node = *toVisit.begin();
        toVisit.erase(toVisit.begin());
        for (auto& neighbour : node->links)
        {
            if (previous.find(neighbour) == previous.end())
            {
                previous[neighbour] = node;
                nodeCosts[neighbour] = sorter.costs[node] + glm::length(node->pos - neighbour->pos);
                toVisit.insert(neighbour);
            }
        }
    }

    if (!toVisit.empty() && glm::distance((*toVisit.begin())->pos, endPos) <= distance)
    {
        std::vector<glm::vec2> result;
        const Node* node = *toVisit.begin();
        while (node != startNode)
        {
            result.push_back(node->pos);
            node = previous[node];
        }
        return result;
    }

    return {};
}

Pathfinding::Node* Pathfinding::FindNodeByPosition(glm::vec2 position)
{
    auto NodeIsAtPosition = [position](const unique_ptr<Node>& node) { return node->pos == position; };
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), NodeIsAtPosition);
    return (it == m_nodes.end()) ? nullptr : it->get();
}

const Pathfinding::Node* Pathfinding::FindNodeByPosition(glm::vec2 position) const
{
    auto NodeIsAtPosition = [position](const unique_ptr<Node>& node) { return node->pos == position; };
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), NodeIsAtPosition);
    return (it == m_nodes.end()) ? nullptr : it->get();
}
