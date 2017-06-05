#pragma once
#include "witness.hpp"
#include "node.hpp"
#include "edge.hpp"

using namespace Map2Check;
namespace Map2Check {
class Graph {
protected:
    std::string id;
    std::vector<std::unique_ptr<DataElement>> elements;
    std::vector<std::unique_ptr<Node>> states;
    std::vector<std::unique_ptr<Edge>> transitions;
    virtual std::string convertToString();
public:
    Graph() {}
    virtual void AddElement(std::unique_ptr<DataElement> element);
    virtual void AddNode(std::unique_ptr<Node> node);
    virtual void AddEdge(std::unique_ptr<Edge> edge);
    operator std::string() {
        return this->convertToString();
    }

};

class ViolationWitnessGraph : public Graph {
protected:
    virtual std::string convertToString();
public:
    ViolationWitnessGraph() : Graph() {}
};

class CorrectnessWitnessGraph : public Graph {
protected:
    virtual std::string convertToString();
public:
    CorrectnessWitnessGraph() : Graph() {}
};

class SVCompWitness {
protected:
    std::unique_ptr<Graph> automata;
    void makeViolationAutomata();
public:
    SVCompWitness(std::string programPath, std::string programHash, std::string targetFunction = "");
    void Testify();

};
}
