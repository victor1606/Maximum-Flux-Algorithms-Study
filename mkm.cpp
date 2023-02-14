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

// Malhotra-Kumar-Maheshwari
// O(V^3)
//
// http://eprints.utas.edu.au/160/1/iplFlow.pdf
// http://faculty.cs.tamu.edu/chen/courses/cpsc669/2014/notes/ch2.pdf
template <typename T>
class MKMSolver
{
    MKMSolver() : network(nullptr) {}
    ~MKMSolver() = default;

    Network<T>* network;

    const size_t DISTANCE_INFINITY = SIZE_MAX - 10;
    std::vector<size_t> distance;

    std::vector<T> potentialIn;
    std::vector<T> potentialOut;

    void initializePotentials()
    {
        potentialIn.resize(network->numberOfVertices());
        potentialOut.resize(network->numberOfVertices());

        potentialIn.assign(potentialIn.size(), 0);
        potentialOut.assign(potentialOut.size(), 0);

        for (VertexNumber vertex = 0; vertex < network->numberOfVertices(); ++vertex)
        {
            for (EdgeNumber edge: network->edgesFrom(vertex))
            {
                if (distance[network->getEdgeTarget(edge)] == distance[network->getEdgeSource(edge)] + 1)
                {
                    T potential = network->getEdgeCapacity(edge) - network->getEdgeFlow(edge);
                    potentialIn[network->getEdgeTarget(edge)] += potential;
                    potentialOut[network->getEdgeSource(edge)] += potential;
                }
            }
        }
    }

    T getPotential(VertexNumber vertexNumber) const
    {
        assert(vertexNumber < network->numberOfVertices());

        if (vertexNumber == network->getFlowSource())
        {
            return potentialOut[vertexNumber];
        }

        if (vertexNumber == network->getFlowTarget())
        {
            return potentialIn[vertexNumber];
        }

        return std::min(potentialIn[vertexNumber], potentialOut[vertexNumber]);
    }

    bool bfs()
    {
        static std::queue<VertexNumber> queue;
        assert(queue.empty());

        assert(distance.size() == network->numberOfVertices());

        std::fill(distance.begin(), distance.end(), DISTANCE_INFINITY);

        distance[network->getFlowSource()] = 0;
        queue.push(network->getFlowSource());

        while (!queue.empty())
        {
            VertexNumber currentVertex = queue.front();
            queue.pop();

            for (EdgeNumber edgeNumber: network->edgesFrom(currentVertex))
            {
                assert(network->getEdgeSource(edgeNumber) == currentVertex);

                VertexNumber edgeTarget = network->getEdgeTarget(edgeNumber);
                if (network->getEdgeFlow(edgeNumber) < network->getEdgeCapacity(edgeNumber)
                    && distance[edgeTarget] == DISTANCE_INFINITY)
                {
                    distance[edgeTarget] = distance[currentVertex] + 1;
                    queue.push(edgeTarget);
                }
            }
        }

        return distance[network->getFlowTarget()] != DISTANCE_INFINITY;
    }

    void push(VertexNumber startVertex, T value)
    {
        static std::queue<VertexNumber> queue;
        assert(queue.empty());

        static std::vector<T> toBePushed;
        toBePushed.resize(network->numberOfVertices());
        toBePushed.assign(toBePushed.size(), 0);

        toBePushed[startVertex] += value;
        queue.push(startVertex);

        while (!queue.empty())
        {
            VertexNumber vertex = queue.front();
            queue.pop();

            for (EdgeNumber edge: network->edgesFrom(vertex))
            {
                if (!toBePushed[vertex])
                {
                    break;
                }
                if (distance[network->getEdgeTarget(edge)] != distance[vertex] + 1)
                {
                    continue;
                }

                T willBePushed = std::min(network->getEdgeCapacity(edge) - network->getEdgeFlow(edge), toBePushed[vertex]);
                if (!willBePushed)
                {
                    continue;
                }

                if (!toBePushed[network->getEdgeTarget(edge)] && network->getEdgeTarget(edge) != network->getFlowTarget())
                {
                    queue.push(network->getEdgeTarget(edge));
                }

                network->updateEdgeFlow(edge, willBePushed);

                potentialIn[network->getEdgeTarget(edge)] -= willBePushed;
                potentialOut[vertex] -= willBePushed;

                toBePushed[vertex] -= willBePushed;
                toBePushed[network->getEdgeTarget(edge)] += willBePushed;
            }
        }
    }

    void pull(VertexNumber startVertex, T value)
    {
        static std::queue<VertexNumber> queue;
        assert(queue.empty());

        static std::vector<T> toBePushed;
        toBePushed.resize(network->numberOfVertices());
        toBePushed.assign(toBePushed.size(), 0);

        toBePushed[startVertex] += value;
        queue.push(startVertex);

        while (!queue.empty())
        {
            VertexNumber vertex = queue.front();
            queue.pop();

            for (EdgeNumber edge: network->edgesTo(vertex))
            {
                if (!toBePushed[vertex])
                {
                    break;
                }
                if (distance[vertex] != distance[network->getEdgeSource(edge)] + 1)
                {
                    continue;
                }

                T willBePushed = std::min(network->getEdgeCapacity(edge) - network->getEdgeFlow(edge), toBePushed[vertex]);
                if (!willBePushed)
                {
                    continue;
                }

                if (!toBePushed[network->getEdgeSource(edge)] && network->getEdgeSource(edge) != network->getFlowSource())
                {
                    queue.push(network->getEdgeSource(edge));
                }

                network->updateEdgeFlow(edge, willBePushed);

                potentialOut[network->getEdgeSource(edge)] -= willBePushed;
                potentialIn[vertex] -= willBePushed;

                toBePushed[vertex] -= willBePushed;
                toBePushed[network->getEdgeSource(edge)] += willBePushed;
            }
        }
    }

    T findBlockingFlow()
    {
        while (true)
        {
            VertexNumber minimumPotentialVertex = DISTANCE_INFINITY;

            for (VertexNumber vertex = 0; vertex < network->numberOfVertices(); ++vertex)
            {
                if (distance[vertex] == DISTANCE_INFINITY)
                {
                    continue;
                }

                if (minimumPotentialVertex == DISTANCE_INFINITY || (getPotential(vertex) < getPotential(minimumPotentialVertex)))
                {
                    minimumPotentialVertex = vertex;
                }
            }

            if (minimumPotentialVertex == DISTANCE_INFINITY)
            {
                return 0;
            }

            if (!getPotential(minimumPotentialVertex))
            {
                for (EdgeNumber edge: network->edgesFrom(minimumPotentialVertex))
                {
                    if (distance[network->getEdgeTarget(edge)] != distance[minimumPotentialVertex] + 1)
                    {
                        continue;
                    }
                    T willBeReduced = network->getEdgeCapacity(edge) - network->getEdgeFlow(edge);
                    potentialOut[minimumPotentialVertex] -= willBeReduced;
                    potentialIn[network->getEdgeTarget(edge)] -= willBeReduced;
                }
                for (EdgeNumber edge: network->edgesTo(minimumPotentialVertex))
                {
                    if (distance[minimumPotentialVertex] != distance[network->getEdgeSource(edge)] + 1)
                    {
                        continue;
                    }
                    T willBeReduced = network->getEdgeCapacity(edge) - network->getEdgeFlow(edge);
                    potentialIn[minimumPotentialVertex] -= willBeReduced;
                    potentialOut[network->getEdgeSource(edge)] -= willBeReduced;
                }
                distance[minimumPotentialVertex] = DISTANCE_INFINITY;
                continue;
            }

            T delta = getPotential(minimumPotentialVertex);
            push(minimumPotentialVertex, delta);
            pull(minimumPotentialVertex, delta);
            return delta;
        }
    }

public:
    MKMSolver(MKMSolver const&) = delete;
    MKMSolver(MKMSolver&&) = delete;
    MKMSolver& operator =(MKMSolver const&) = delete;
    MKMSolver& operator =(MKMSolver&&) = delete;

    static MKMSolver& getInstance()
    {
        static MKMSolver instance;
        return instance;
    }

    T findMaximumFlow(Network<T>* network)
    {
        MKMSolver::network = network;
        T maxFlow = 0;

        distance.resize(network->numberOfVertices());
        while (bfs())
        {
            initializePotentials();

            T addFlow = findBlockingFlow();
            while (addFlow)
            {
                maxFlow += addFlow;
                addFlow = findBlockingFlow();
            }
        }

        return maxFlow;
    }
};

int maxflow(ifstream& infile)
{
    size_t numberOfVertices;
    size_t numberOfEdges;
    VertexNumber start;
    VertexNumber finish;

    infile >> numberOfVertices >> numberOfEdges;
    
    start = 0;
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

    auto& solver1 = MKMSolver<int>::getInstance();

    int maximumFlow1 = solver1.findMaximumFlow(&network);

    return maximumFlow1;
}
