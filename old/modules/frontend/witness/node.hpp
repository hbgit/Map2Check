#pragma once
#include "witness.hpp"
namespace Map2Check {
class NodeElement {
protected:
    virtual std::string convertToString();
public:
    NodeElement() {}
    operator std::string() {
        return this->convertToString();
    }
};

class ViolationNodeElement : public NodeElement {
protected:
    virtual std::string convertToString();
public:
    ViolationNodeElement() : NodeElement() {}
};

class EntryNode : public ViolationNodeElement {
protected:
    virtual std::string convertToString();
public:
    EntryNode() : ViolationNodeElement() {}
};

class SinkNode : public ViolationNodeElement {
protected:
    virtual std::string convertToString();
public:
    SinkNode() : ViolationNodeElement() {}
};

class ViolationNode : public ViolationNodeElement {
protected:
    virtual std::string convertToString();
public:
    ViolationNode() : ViolationNodeElement() {}
};


class Node {
protected:
    std::string id;
    std::vector<std::unique_ptr<NodeElement>> elements;
    virtual std::string convertToString();
public:
    Node(std::string id) : id(id) {}
    void AddElement(std::unique_ptr<NodeElement> element);
    operator std::string() {
        return this->convertToString();
    }
};

}

