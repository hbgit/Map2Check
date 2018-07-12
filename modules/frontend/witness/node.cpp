#include "node.hpp"


using namespace Map2Check;
std::string NodeElement::convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "NodeElement";
                return cnvt.str();
}

std::string ViolationNodeElement::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "ViolationNodeElement";
    return cnvt.str();
}

std::string EntryNode::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "\t\t\t<data key=\"entry\">true</data>";
    return cnvt.str();
}

std::string SinkNode::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "\t\t\t<data key=\"sink\">true</data>";
    return cnvt.str();
}

std::string ViolationNode::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "\t\t\t<data key=\"violation\">true</data>";
    return cnvt.str();
}

std::string Node::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "\t\t<node id=\"";
    cnvt << this->id;
    cnvt << "\"";

    if(this->elements.size() == 0) {
         cnvt << "/>";
    }
    else {
        cnvt << ">\n";
        for (int i = 0; i < this->elements.size(); i++ ) {
            cnvt << (std::string) *this->elements[i];
            cnvt << "\n";
        }
        cnvt << "\t\t</node>";
    }
    return cnvt.str();
}

void Node::AddElement(std::unique_ptr<NodeElement> element) {
    this->elements.push_back(std::move(element));
}
