#include "witness.hpp"
#include "log.h"
#include "tools.h"
#include <algorithm>
#include <memory>
#include <fstream>

using namespace Map2Check;

void Node::AddElement(std::unique_ptr<NodeElement> element) {
    this->elements.push_back(std::move(element));
}

void Edge::AddElement(std::unique_ptr<EdgeData> element) {
    this->elements.push_back(std::move(element));
}

void Graph::AddElement(std::unique_ptr<DataElement> element) {
    this->elements.push_back(std::move(element));
}

void Graph::AddNode(std::unique_ptr<Node> node) {
    this->states.push_back(std::move(node));
}

void Graph::AddEdge(std::unique_ptr<Edge> edge) {
     this->transitions.push_back(std::move(edge));
}

void SVCompWitness::Testify() {
    ofstream outputFile("witness.graphml");
    outputFile << (std::string) this->automata;
}
SVCompWitness::SVCompWitness(std::string programPath, std::string programHash, std::string targetFunction) {
    Map2Check::Log::Debug("Starting Witness Generation");

    std::unique_ptr<DataElement> witnessType = std::make_unique<WitnessType>(WitnessTypeValues::VIOLATION);
    this->automata.AddElement(std::move(witnessType));

    std::unique_ptr<DataElement> sourceCodeType = std::make_unique<SourceCodeLang>(SupportedSourceCodeLang::C);
    this->automata.AddElement(std::move(sourceCodeType));

    std::unique_ptr<DataElement> producer = std::make_unique<Producer>();
    this->automata.AddElement(std::move(producer));

    std::unique_ptr<DataElement> specification;
    Tools::CheckViolatedProperty violated;
    switch(violated.propertyViolated) {
        case Tools::PropertyViolated::FALSE_FREE:
            specification = std::make_unique<Specification>(SpecificationType::FREE);
            break;
        case Tools::PropertyViolated::FALSE_DEREF:
            specification = std::make_unique<Specification>(SpecificationType::DEREF);
            break;    
        case Tools::PropertyViolated::FALSE_MEMTRACK:
            specification = std::make_unique<Specification>(SpecificationType::MEMLEAK);
            break;    
        case Tools::PropertyViolated::TARGET_REACHED:
            specification = std::make_unique<Specification>(SpecificationType::TARGET, targetFunction);
            break; 
        default:
            break;          
    }
    this->automata.AddElement(std::move(specification));

    std::unique_ptr<DataElement> programFile = std::make_unique<ProgramFile>(programPath);
    this->automata.AddElement(std::move(programFile));

    std::unique_ptr<DataElement> programHashElement = std::make_unique<ProgramHash>(programHash);
    this->automata.AddElement(std::move(programHashElement));

    std::unique_ptr<DataElement> architecture = std::make_unique<Architecture>(ArchitectureType::Bit32);
    this->automata.AddElement(std::move(architecture));

    Map2Check::Log::Debug("Starting Automata Generation");
    unsigned lastState = 0;
    std::string lastStateId = "s0";
    std::unique_ptr<Node> startNode = std::make_unique<Node>("s0");
    lastState++;

    std::unique_ptr<NodeElement> entryNode = std::make_unique<EntryNode>();
    startNode->AddElement(std::move(entryNode));
    
    std::vector<Tools::KleeLogRow> kleeLogRows = Tools::KleeLogHelper::getListLogFromCSV();

    if(kleeLogRows.size() == 0) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>("s1");
        
        std::unique_ptr<NodeElement> violationNode = std::make_unique<ViolationNode>();
        newNode->AddElement(std::move(violationNode));

        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>("s0", "s1");
        this->automata.AddEdge(std::move(newEdge));
        this->automata.AddNode(std::move(newNode));
    }
    this->automata.AddNode(std::move(startNode));

    for(int i = 0; i < kleeLogRows.size(); i++) {
        std::string lineNumber = kleeLogRows[i].line;
        std::string value =  kleeLogRows[i].value;
        std::string functionName =  kleeLogRows[i].functionName;

        std::ostringstream cnvt;
    	cnvt.str("");    
        cnvt << "s" << lastState;
        lastState++;

        std::unique_ptr<Node> newNode = std::make_unique<Node>(cnvt.str());
        if(i == (kleeLogRows.size() - 1) ) {
            std::unique_ptr<NodeElement> violationNode = std::make_unique<ViolationNode>();
            newNode->AddElement(std::move(violationNode));
        }

        this->automata.AddNode(std::move(newNode));

        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());
        lastStateId =  cnvt.str();

        std::unique_ptr<EdgeData> assumption = std::make_unique<AssumptionEdgeData>(value, "__VERIFIER_nondet_int", functionName);
        newEdge->AddElement(std::move(assumption));

        std::unique_ptr<EdgeData> startLine = std::make_unique<StartLine>(lineNumber);
        newEdge->AddElement(std::move(startLine));

        this->automata.AddEdge(std::move(newEdge));
    }

    // Map2Check::Log::Debug((std::string) this->automata);

}
