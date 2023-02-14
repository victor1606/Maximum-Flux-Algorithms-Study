#include <iostream>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <queue>
#include <list>

#include "maxflow.h"

typedef size_t EdgeNumber;
typedef size_t VertexNumber;

class UnknownEdgeException: public std::out_of_range
{
public:
    explicit UnknownEdgeException(EdgeNumber edgeNumber)
            : std::out_of_range("Edge " + std::to_string(edgeNumber) + " doesn't exist") {}
};

class UnknownVertexException: public std::out_of_range
{
public:
    explicit UnknownVertexException(VertexNumber vertexNumber)
            : std::out_of_range("Vertex " + std::to_string(vertexNumber) + " doesn't exist") {}
};

class FlowMoreThanCapacityException: public std::logic_error
{
public:
    explicit FlowMoreThanCapacityException(EdgeNumber edgeNumber)
            : std::logic_error("Edge " + std::to_string(edgeNumber) + "is overflowed") {}
};

template <typename T>
class Network
{
    struct Edge
    {
        EdgeNumber source;
        EdgeNumber target;

        T capacity;
        T flow;

        Edge(EdgeNumber source, EdgeNumber target, T capacity)
                : source(source), target(target), capacity(capacity), flow(0) {}
        ~Edge() = default;
    };

    VertexNumber flowSource;
    VertexNumber flowTarget;

    std::vector<Edge> edges;
    std::vector<std::vector<EdgeNumber>> graph;
    std::vector<std::vector<EdgeNumber>> graph_reverse;

public:
    Network() = delete;

    explicit Network(size_t numberOfVertices, VertexNumber flowSource, VertexNumber flowTarget)
            : flowSource(flowSource), flowTarget(flowTarget)
    {
        if (!numberOfVertices)
            throw UnknownVertexException(flowSource);
        if (!(flowSource < numberOfVertices))
            throw UnknownVertexException(flowSource);
        if (!(flowTarget < numberOfVertices))
            throw UnknownVertexException(flowTarget);

        graph.resize(numberOfVertices);
        graph_reverse.resize(numberOfVertices);
    }

    ~Network() = default;

    size_t numberOfVertices() const
    {
        return graph.size();
    }

    VertexNumber getFlowSource() const
    {
        return flowSource;
    }

    VertexNumber getFlowTarget() const
    {
        return flowTarget;
    }

    const std::vector<EdgeNumber>& edgesFrom(VertexNumber vertexNumber) const
    {
        if (!(vertexNumber < graph.size()))
            throw UnknownVertexException(vertexNumber);

        return graph[vertexNumber];
    }

    const std::vector<EdgeNumber>& edgesTo(VertexNumber vertexNumber) const
    {
        if (!(vertexNumber < graph.size()))
            throw UnknownVertexException(vertexNumber);

        return graph_reverse[vertexNumber];
    }

    EdgeNumber getBackEdge(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return edgeNumber ^ 1;
    }

    VertexNumber getEdgeSource(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return edges[edgeNumber].source;
    }

    VertexNumber getEdgeTarget(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return edges[edgeNumber].target;
    }

    T getEdgeCapacity(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return edges[edgeNumber].capacity;
    }

    T getEdgeFlow(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return edges[edgeNumber].flow;
    }

    T updateEdgeFlow(EdgeNumber edgeNumber, T delta)
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        edges[edgeNumber].flow += delta;
        edges[edgeNumber ^ 1].flow -= delta;

        if (std::abs(edges[edgeNumber].flow) > std::max(edges[edgeNumber].capacity, edges[edgeNumber ^ 1].capacity))
            throw FlowMoreThanCapacityException(edgeNumber);

        return edges[edgeNumber].flow;
    }

    VertexNumber addVertex()
    {
        graph.resize(graph.size() + 1);
        graph_reverse.resize(graph_reverse.size() + 1);
        return graph.size() - 1;
    }

    void addEdge(VertexNumber source, VertexNumber target, T capacity)
    {
        if (!(source < graph.size()))
            throw UnknownVertexException(source);
        if (!(target < graph.size()))
            throw UnknownVertexException(target);
        assert(capacity >= 0);

        edges.emplace_back(source, target, capacity);
        edges.emplace_back(target, source, 0);
        graph[source].push_back(edges.size() - 2);
        graph[target].push_back(edges.size() - 1);
        graph_reverse[source].push_back(edges.size() - 1);
        graph_reverse[target].push_back(edges.size() - 2);
    }

    bool isBackEdge(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return static_cast<bool>(edgeNumber % 2);
    }

    bool isStraightEdge(EdgeNumber edgeNumber) const
    {
        if (!(edgeNumber < edges.size()))
            throw UnknownEdgeException(edgeNumber);

        return !isBackEdge(edgeNumber);
    }
};

// Relabel-to-front preflow push
// O(V^3)
//
// "Introduction to Algorithm" by Thomas H. Cormen
template <typename T>
class GoldbergSolver
{
    GoldbergSolver() : network(nullptr) {}
    ~GoldbergSolver() = default;

    Network<T>* network;

    std::vector<VertexNumber>   height;
    std::vector<T>              overflow;
    std::vector<size_t>         edgePosition;
    std::list<VertexNumber>     list;

    void initializePreflow()
    {
        height.resize(network->numberOfVertices());
        overflow.resize(network->numberOfVertices());
        edgePosition.resize(network->numberOfVertices());

        height.assign(height.size(), 0);
        height[network->getFlowSource()] = network->numberOfVertices();

        overflow.assign(overflow.size(), 0);
        for (VertexNumber vertex = 0; vertex < network->numberOfVertices(); ++vertex)
        {
            for (EdgeNumber edge: network->edgesFrom(vertex))
            {
                network->updateEdgeFlow(edge, -network->getEdgeFlow(edge));
            }
        }
        for (EdgeNumber edge: network->edgesFrom(network->getFlowSource()))
        {
            network->updateEdgeFlow(edge, network->getEdgeCapacity(edge));
            overflow[network->getEdgeTarget(edge)] += network->getEdgeCapacity(edge);
            overflow[network->getFlowSource()] -= network->getEdgeCapacity(edge);
        }

        edgePosition.assign(edgePosition.size(), 0);

        list.clear();
        for (VertexNumber vertex = 0; vertex < network->numberOfVertices(); ++vertex)
        {
            if (vertex != network->getFlowSource() && vertex != network->getFlowTarget())
            {
                list.push_back(vertex);
            }
        }
    }

    void push(EdgeNumber edge)
    {
        VertexNumber from = network->getEdgeSource(edge);
        VertexNumber to = network->getEdgeTarget(edge);

        assert(overflow[from] > 0);
        assert(network->getEdgeCapacity(edge) - network->getEdgeFlow(edge));
        assert(height[from] == 1 + height[to]);

        T delta = std::min(network->getEdgeCapacity(edge) - network->getEdgeFlow(edge), overflow[from]);
        network->updateEdgeFlow(edge, delta);
        overflow[from] -= delta;
        overflow[to] += delta;
    }

    void relabel(VertexNumber vertex)
    {
        VertexNumber minimumHeight = height[vertex];

        for (EdgeNumber edge: network->edgesFrom(vertex))
        {
            if (!(network->getEdgeCapacity(edge) - network->getEdgeFlow(edge)))
            {
                continue;
            }
            VertexNumber to = network->getEdgeTarget(edge);
            assert(height[to] >= height[vertex]);
            minimumHeight = std::min(minimumHeight, height[to]);
        }

        height[vertex] = 1 + minimumHeight;
    }

    void discharge(VertexNumber vertex)
    {
        while (overflow[vertex] > 0)
        {
            if (edgePosition[vertex] >= network->edgesFrom(vertex).size())
            {
                relabel(vertex);
                edgePosition[vertex] = 0;
                continue;
            }

            EdgeNumber edge = network->edgesFrom(vertex)[edgePosition[vertex]];
            VertexNumber to = network->getEdgeTarget(edge);
            if (network->getEdgeCapacity(edge) - network->getEdgeFlow(edge)
                && height[vertex] == 1 + height[to])
            {
                push(edge);
            }
            else
            {
                ++edgePosition[vertex];
            }
        }
    }

public:
    GoldbergSolver(GoldbergSolver const&) = delete;
    GoldbergSolver(GoldbergSolver&&) = delete;
    GoldbergSolver& operator =(GoldbergSolver const&) = delete;
    GoldbergSolver& operator =(GoldbergSolver&&) = delete;

    static GoldbergSolver& getInstance()
    {
        static GoldbergSolver instance;
        return instance;
    }

    T findMaximumFlow(Network<T>* network)
    {
        this->network = network;

        initializePreflow();

        auto listIterator = list.begin();
        while (listIterator != list.end())
        {
            VertexNumber vertex = *listIterator;
            VertexNumber previousHeight = height[vertex];

            discharge(vertex);

            if (height[vertex] > previousHeight)
            {
                list.erase(listIterator);
                list.push_front(vertex);
                listIterator = list.begin();
            }
            else
            {
                ++listIterator;
            }
        }

        return -overflow[network->getFlowSource()];
    }

};

int maxflow(ifstream& infile) {
    size_t numberOfVertices;
    size_t numberOfEdges;
    VertexNumber start = 0;
    VertexNumber finish;

    infile >> numberOfVertices >> numberOfEdges;

    finish = numberOfVertices - 1;
    
    Network<int> network(numberOfVertices, start, finish);

    for (size_t i = 0; i < numberOfEdges; ++i)
    {
        VertexNumber from; // 0 ... numberOfVertices - 1
        VertexNumber to; // 0 ... numberOfVertices - 1
        int capacity;

        infile >> from >> to >> capacity;
        network.addEdge(from, to, capacity);
    }

    auto& solver2 = GoldbergSolver<int>::getInstance();

    int maximumFlow2 = solver2.findMaximumFlow(&network);

    return maximumFlow2;
}
