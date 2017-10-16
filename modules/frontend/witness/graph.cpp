#include "witness.hpp"
#include "graph.hpp"
#include "../utils/log.hpp"
#include "../utils/tools.hpp"
#include <fstream>
using namespace Map2Check;

void Graph::AddElement(std::unique_ptr<DataElement> element) {
    this->elements.push_back(std::move(element));
}

void Graph::AddNode(std::unique_ptr<Node> node) {
    this->states.push_back(std::move(node));
}

void Graph::AddEdge(std::unique_ptr<Edge> edge) {
     this->transitions.push_back(std::move(edge));
}


std::string Graph::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");

    return cnvt.str();
}

std::string ViolationWitnessGraph::convertToString() {
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    cnvt << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";

    cnvt << "\t<key attr.name=\"isEntryNode\" attr.type=\"boolean\" for=\"node\" id=\"entry\">\n";
    cnvt << "\t\t<default>false</default>\n";
    cnvt << "\t</key>\n";

    cnvt << "\t<key attr.name=\"isViolationNode\" attr.type=\"boolean\" for=\"node\" id=\"violation\">\n";
    cnvt << "\t\t<default>false</default>\n";
    cnvt << "\t</key>\n";

    cnvt << "\t<key attr.name=\"witness-type\" attr.type=\"string\" for=\"graph\" id=\"witness-type\"/>\n";
    cnvt << "\t<key attr.name=\"sourcecodelang\" attr.type=\"string\" for=\"graph\" id=\"sourcecodelang\"/>\n";
    cnvt << "\t<key attr.name=\"producer\" attr.type=\"string\" for=\"graph\" id=\"producer\"/>\n";
    cnvt << "\t<key attr.name=\"specification\" attr.type=\"string\" for=\"graph\" id=\"specification\"/>\n";
    cnvt << "\t<key attr.name=\"programFile\" attr.type=\"string\" for=\"graph\" id=\"programfile\"/>\n";
    cnvt << "\t<key attr.name=\"programHash\" attr.type=\"string\" for=\"graph\" id=\"programhash\"/>\n";
    cnvt << "\t<key attr.name=\"architecture\" attr.type=\"string\" for=\"graph\" id=\"architecture\"/>\n";
    cnvt << "\t<key attr.name=\"startline\" attr.type=\"int\" for=\"edge\" id=\"startline\"/>\n";
    cnvt << "\t<key attr.name=\"assumption\" attr.type=\"string\" for=\"edge\" id=\"assumption\"/>\n";
    cnvt << "\t<key attr.name=\"assumption.scope\" attr.type=\"string\" for=\"edge\" id=\"assumption.scope\"/>\n";
    cnvt << "\t<key attr.name=\"assumption.resultfunction\" attr.type=\"string\" for=\"edge\" id=\"assumption.resultfunction\"/>\n";

    cnvt << "\t<graph edgedefault=\"directed\">\n";

    for (int i = 0; i < this->elements.size(); i++ ) {
            cnvt << (std::string) *this->elements[i];
            cnvt << "\n";
    }

    for (int i = 0; i < this->states.size(); i++ ) {
            cnvt << (std::string) *this->states[i];
            cnvt << "\n";
    }

    for (int i = 0; i < this->transitions.size(); i++ ) {
            cnvt << (std::string) *this->transitions[i];
            cnvt << "\n";
    }

    cnvt << "\t</graph>\n";
    cnvt << "</graphml>";

    return cnvt.str();
}

std::string CorrectnessWitnessGraph::convertToString() {    
    std::ostringstream cnvt;
    cnvt.str("");
    cnvt << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    cnvt << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n";

    cnvt << "\t<key attr.name=\"isEntryNode\" attr.type=\"boolean\" for=\"node\" id=\"entry\">\n";
    cnvt << "\t\t<default>false</default>\n";
    cnvt << "\t</key>\n";

    cnvt << "\t<key attr.name=\"witness-type\" attr.type=\"string\" for=\"graph\" id=\"witness-type\"/>\n";
    cnvt << "\t<key attr.name=\"invariant\" attr.type=\"string\" for=\"node\" id=\"invariant\"/>\n";
    cnvt << "\t<key attr.name=\"sourcecodelang\" attr.type=\"string\" for=\"graph\" id=\"sourcecodelang\"/>\n";
    cnvt << "\t<key attr.name=\"producer\" attr.type=\"string\" for=\"graph\" id=\"producer\"/>\n";
    cnvt << "\t<key attr.name=\"specification\" attr.type=\"string\" for=\"graph\" id=\"specification\"/>\n";
    cnvt << "\t<key attr.name=\"programFile\" attr.type=\"string\" for=\"graph\" id=\"programfile\"/>\n";
    cnvt << "\t<key attr.name=\"programHash\" attr.type=\"string\" for=\"graph\" id=\"programhash\"/>\n";
    cnvt << "\t<key attr.name=\"architecture\" attr.type=\"string\" for=\"graph\" id=\"architecture\"/>\n";
    cnvt << "\t<key attr.name=\"startline\" attr.type=\"int\" for=\"edge\" id=\"startline\"/>\n";
    cnvt << "\t<key attr.name=\"assumption\" attr.type=\"string\" for=\"edge\" id=\"assumption\"/>\n";
    cnvt << "\t<key attr.name=\"assumption.scope\" attr.type=\"string\" for=\"edge\" id=\"assumption.scope\"/>\n";
    cnvt << "\t<key attr.name=\"assumption.resultfunction\" attr.type=\"string\" for=\"edge\" id=\"assumption.resultfunction\"/>\n";

    cnvt << "\t<graph edgedefault=\"directed\">\n";
	    
    for (int i = 0; i < this->elements.size(); i++ ) {            
            cnvt << (std::string) *this->elements[i];            
            cnvt << "\n";
    }
	
    for (int i = 0; i < this->states.size(); i++ ) {
            cnvt << (std::string) *this->states[i];
            cnvt << "\n";
    }

    for (int i = 0; i < this->transitions.size(); i++ ) {
            cnvt << (std::string) *this->transitions[i];
            cnvt << "\n";
    }    

    cnvt << "\t</graph>\n";
    cnvt << "</graphml>";

    return cnvt.str();
}

void SVCompWitness::Testify() {
    ofstream outputFile("witness.graphml");      
    cout << (std::string) (*this->automata) << "\n" ;
    outputFile << (std::string) (*this->automata);
}


SVCompWitness::SVCompWitness(std::string programPath, std::string programHash, std::string targetFunction, std::string specTrueString) {
    Map2Check::Log::Debug("Starting Witness Generation");

    std::unique_ptr<DataElement> specification;
    Tools::CheckViolatedProperty violated;
    bool violationWitness = true;
    //TODO: Add the spection to automa true, what is the property was I checking?
    switch(violated.propertyViolated) {
        case Tools::PropertyViolated::FALSE_FREE:
            specification = std::make_unique<Specification>(SpecificationType::FREE);
            this->automata = std::make_unique<ViolationWitnessGraph>();
            break;
        case Tools::PropertyViolated::FALSE_DEREF:
            specification = std::make_unique<Specification>(SpecificationType::DEREF);
            this->automata = std::make_unique<ViolationWitnessGraph>();
            break;
        case Tools::PropertyViolated::FALSE_MEMTRACK:
            specification = std::make_unique<Specification>(SpecificationType::MEMLEAK);
            this->automata = std::make_unique<ViolationWitnessGraph>();
            break;
        case Tools::PropertyViolated::TARGET_REACHED:
            specification = std::make_unique<Specification>(SpecificationType::TARGET, targetFunction);
            this->automata = std::make_unique<ViolationWitnessGraph>();
            break;
        case Tools::PropertyViolated::FALSE_OVERFLOW:
            specification = std::make_unique<Specification>(SpecificationType::SPECOVERFLOW);
            this->automata = std::make_unique<ViolationWitnessGraph>();
            break;
        default:
            this->automata = std::make_unique<CorrectnessWitnessGraph>();
            violationWitness = false;            
            break;
    }
    
    std::unique_ptr<DataElement> witnessType;
    if(violationWitness) {
       witnessType = std::make_unique<WitnessType>(WitnessTypeValues::VIOLATION);
    } else {
       witnessType = std::make_unique<WitnessType>(WitnessTypeValues::CORRECTNESS);
       if(specTrueString == "target-function")
       {
		   cout << specTrueString << "\n";
		   specification = std::make_unique<Specification>(SpecificationType::TARGET, targetFunction);
	   }else if(specTrueString == "overflow")
	   {
		   specification = std::make_unique<Specification>(SpecificationType::SPECOVERFLOW);
	   }else{
		   specification = std::make_unique<Specification>(SpecificationType::MEMSAFETY);
	   }
    }

    this->automata->AddElement(std::move(witnessType));
    
    std::unique_ptr<DataElement> sourceCodeType = std::make_unique<SourceCodeLang>(SupportedSourceCodeLang::C);
    this->automata->AddElement(std::move(sourceCodeType));

    std::unique_ptr<DataElement> producer = std::make_unique<Producer>();
    this->automata->AddElement(std::move(producer));

    this->automata->AddElement(std::move(specification));

    std::unique_ptr<DataElement> programFile = std::make_unique<ProgramFile>(programPath);
    this->automata->AddElement(std::move(programFile));

    std::unique_ptr<DataElement> programHashElement = std::make_unique<ProgramHash>(programHash);
    this->automata->AddElement(std::move(programHashElement));

    std::unique_ptr<DataElement> architecture = std::make_unique<Architecture>(ArchitectureType::Bit32);
    this->automata->AddElement(std::move(architecture));

    if(violationWitness) {
       this->makeViolationAutomata();
    }else{	   
	   this->makeCorrectnessAutomata();
	}


}

void SVCompWitness::makeCorrectnessAutomata() 
{
	Map2Check::Log::Debug("Starting Correctness Automata Generation");
    unsigned lastState = 0;
    std::string lastStateId = "s0";
    std::unique_ptr<Node> startNode = std::make_unique<Node>("s0");
    lastState++;
    
    std::unique_ptr<NodeElement> entryNode = std::make_unique<EntryNode>();
    startNode->AddElement(std::move(entryNode));

    std::vector<Tools::StateTrueLogRow> stateTrueLogRows = Tools::StateTrueLogHelper::getListLogFromCSV();
    
    std::vector<Tools::TrackBBLogRow> trackBBLogRows = Tools::TrackBBLogHelper::getListLogFromCSV();
    
    if(stateTrueLogRows.size() == 0 || trackBBLogRows.size() == 0) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>("s1");
        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>("s0", "s1");
        this->automata->AddEdge(std::move(newEdge));
        this->automata->AddNode(std::move(newNode));
    }
    
    this->automata->AddNode(std::move(startNode));    
    
    // Creating the automata nodes
    // The total number of automata nodes is egual to number lines in 
    // automata_list_log.st file take into accounting the BB executed in
    // track_bb_log.st file
    for(int i = 0; i < trackBBLogRows.size(); i++) {
		
		int trackBBLineNum = std::stoi(trackBBLogRows[i].numLineInBB);
		std::string trackBBFunctName = trackBBLogRows[i].functionName;
		bool flagCreateNewNode = false;
		
		int stateTrueNumLineBeginBB;
		int stateTrueNumLineStart;
		
		
		std::ostringstream cnvt;
        cnvt.str("");
        cnvt << "s" << lastState;
        lastState++;

        // Checking if the state in stateTrueLogRows was executed in TrackBBLogRow
        for(int i = 0; i < stateTrueLogRows.size(); i++) 
        {	
			stateTrueNumLineBeginBB = std::stoi(stateTrueLogRows[i].numLineBeginBB);
			stateTrueNumLineStart = std::stoi(stateTrueLogRows[i].numLineStart);
			
			if(trackBBFunctName == stateTrueLogRows[i].functionName)
			{
				if( (trackBBLineNum >= stateTrueNumLineBeginBB) && (trackBBLineNum <= stateTrueNumLineStart))
				{
					// This state (i.e., the BB) was executed
					std::unique_ptr<Node> newNode = std::make_unique<Node>(cnvt.str());
					//TODO node attributes to invariants
					//std::unique_ptr<NodeElement> violationNode = std::make_unique<ViolationNode>();
					//newNode->AddElement(std::move(violationNode));
					this->automata->AddNode(std::move(newNode));
					//flagCreateNewNode = true;
					
					
					// Create the edge to the new node
					std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());
					lastStateId =  cnvt.str();
										
					// attribute startline
					std::unique_ptr<EdgeData> startLine = std::make_unique<StartLine>(std::to_string(stateTrueNumLineStart));
					newEdge->AddElement(std::move(startLine));
					
					// attribute sourcecode
					
					this->automata->AddEdge(std::move(newEdge));
				}
				
			}
		}
        
        
			
		       
		
	}
    
}

void SVCompWitness::makeViolationAutomata() {

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
        this->automata->AddEdge(std::move(newEdge));
        this->automata->AddNode(std::move(newNode));
    }
    this->automata->AddNode(std::move(startNode));

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

        this->automata->AddNode(std::move(newNode));

        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());
        lastStateId =  cnvt.str();

        std::unique_ptr<EdgeData> assumption = std::make_unique<AssumptionEdgeData>(value, "__VERIFIER_nondet_int", functionName);
        newEdge->AddElement(std::move(assumption));

        std::unique_ptr<EdgeData> startLine = std::make_unique<StartLine>(lineNumber);
        newEdge->AddElement(std::move(startLine));

        this->automata->AddEdge(std::move(newEdge));
    }
}
