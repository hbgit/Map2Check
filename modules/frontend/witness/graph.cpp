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
    cnvt << "\t<key attr.name=\"invariant\" attr.type=\"string\" for=\"node\" id=\"invariant\"/>\n";

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
    cnvt << "\t<key attr.name=\"sourcecode\" attr.type=\"string\" for=\"edge\" id=\"sourcecode\"/>\n";
    cnvt << "\t<key attr.name=\"control\" attr.type=\"string\" for=\"edge\" id=\"control\"/>\n";
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
	cnvt << "\t<key attr.name=\"invariant\" attr.type=\"string\" for=\"node\" id=\"invariant\"/>\n";
	
    cnvt << "\t<key attr.name=\"witness-type\" attr.type=\"string\" for=\"graph\" id=\"witness-type\"/>\n";    
    cnvt << "\t<key attr.name=\"sourcecodelang\" attr.type=\"string\" for=\"graph\" id=\"sourcecodelang\"/>\n";    
    cnvt << "\t<key attr.name=\"producer\" attr.type=\"string\" for=\"graph\" id=\"producer\"/>\n";
    cnvt << "\t<key attr.name=\"specification\" attr.type=\"string\" for=\"graph\" id=\"specification\"/>\n";
    cnvt << "\t<key attr.name=\"programFile\" attr.type=\"string\" for=\"graph\" id=\"programfile\"/>\n";
    cnvt << "\t<key attr.name=\"programHash\" attr.type=\"string\" for=\"graph\" id=\"programhash\"/>\n";
    cnvt << "\t<key attr.name=\"architecture\" attr.type=\"string\" for=\"graph\" id=\"architecture\"/>\n";
    
    cnvt << "\t<key attr.name=\"startline\" attr.type=\"int\" for=\"edge\" id=\"startline\"/>\n";
    cnvt << "\t<key attr.name=\"sourcecode\" attr.type=\"string\" for=\"edge\" id=\"sourcecode\"/>\n";
    cnvt << "\t<key attr.name=\"control\" attr.type=\"string\" for=\"edge\" id=\"control\"/>\n";
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
		   //cout << specTrueString << "\n";
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
    
    //--- remove rows with error location    
    std:vector<int> removeItem;
    for(int c = 0; c < stateTrueLogRows.size(); c++){
	   if(stateTrueLogRows[c].isErrorLabel == "1"){
			removeItem.push_back(c);
	   }
	}
	
	vector <int>::iterator iremove = removeItem.begin ();
	while (iremove != removeItem.end ()){
		//cout << *iremove << "\n";
	    stateTrueLogRows.erase(stateTrueLogRows.begin() + *iremove);
	    ++ iremove;
	}
	//--- 
        
    
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
    std::string getSt;
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
        for(int k = 0; k < stateTrueLogRows.size(); k++) 
        {	
			stateTrueNumLineBeginBB = std::stoi(stateTrueLogRows[k].numLineBeginBB);
			stateTrueNumLineStart = std::stoi(stateTrueLogRows[k].numLineStart);
			
			if(trackBBFunctName == stateTrueLogRows[k].functionName)
			{				
				if( (trackBBLineNum >= stateTrueNumLineBeginBB) && (trackBBLineNum <= stateTrueNumLineStart))
				{					
					cout << getSt << "!=" << cnvt.str() << "\n";
					if(getSt != cnvt.str())
					{
						// This state (i.e., the BB) was executed
						std::unique_ptr<Node> newNode = std::make_unique<Node>(cnvt.str());
						getSt = cnvt.str();
						//TODO node attributes to invariants
						//std::unique_ptr<NodeElement> violationNode = std::make_unique<ViolationNode>();
						//newNode->AddElement(std::move(violationNode));
						//cout << lastStateId << "===\n";
						//cout << cnvt.str() << "===\n";
						this->automata->AddNode(std::move(newNode));
						//flagCreateNewNode = true;
					}
					
					//cout << lastStateId << "-----\n";
					std::string tmpLastStateId = lastStateId; 
					//cout << cnvt.str() << "<<<<<<\n";
					
					// Create the edge to the new node
					std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());
					lastStateId =  cnvt.str();
										
					// attribute startline
					std::unique_ptr<EdgeData> startLine = std::make_unique<StartLine>(std::to_string(stateTrueNumLineStart));
					newEdge->AddElement(std::move(startLine));
					
										
					if(std::stoi(stateTrueLogRows[k].hasControlCode))
					{						
						//cout << stateTrueLogRows[k].controlCode << "\n";
						
						// attribute sourcecode
						std::unique_ptr<EdgeData> sourcecode = std::make_unique<SourceCode>(stateTrueLogRows[k].controlCode);
						newEdge->AddElement(std::move(sourcecode));
						
						// attribute control 
						// search in trackBBLogRows which condition (TRUE or FALSE) by line number in 
						// stateTrueLogRows was executed
						int tmpCount=i+1; //true cond	
						bool hasTrueCond = false;
											
						if(tmpCount < trackBBLogRows.size())
						{
							//cout << stateTrueLogRows[k].numLineControlTrue << " == " << trackBBLogRows[tmpCount].numLineInBB << "\n";
							if(std::stoi(stateTrueLogRows[k].numLineControlTrue) == std::stoi(trackBBLogRows[tmpCount].numLineInBB))
							{								
								std::unique_ptr<EdgeData> control = std::make_unique<Control>("condition-true");
								newEdge->AddElement(std::move(control));								
								this->automata->AddEdge(std::move(newEdge));
								hasTrueCond = true;;
							}
						}
						
						unsigned tmpLastState;
						std::ostringstream cnvtF;
						if(hasTrueCond)
						{
							tmpCount=i+2; //false cond
							tmpLastState = lastState;							
							cnvtF.str("");
							// tmpLastStateId source
							cnvtF << "s" << tmpLastState; //target
						}else{
							tmpCount=i+1; //false cond
							tmpLastState = lastState-1;							
							cnvtF.str("");							
							cnvtF << "s" << tmpLastState; //target
						}
						
						if(tmpCount < trackBBLogRows.size())
						{
							// creating a edge to its negation
							// create a new node, only if we have a false cond, otherwise we point to the same node
							// from true cond
							//cout << stateTrueLogRows[k].numLineControlFalse << "==" << trackBBLogRows[tmpCount].numLineInBB << "\n";
							if(std::stoi(stateTrueLogRows[k].numLineControlFalse) == std::stoi(trackBBLogRows[tmpCount].numLineInBB))
							{
								// Create a new node for false cond													

								// Create the edge to the new node
								//std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());								
								//std::unique_ptr<Node> newNodeF = std::make_unique<Node>(cnvtF.str());
								//this->automata->AddNode(std::move(newNodeF));								
								// Create the edge to the new node
								std::unique_ptr<Edge> newEdgeF = std::make_unique<Edge>(tmpLastStateId, cnvtF.str());
								
								// attribute startline
								std::unique_ptr<EdgeData> startLineF = std::make_unique<StartLine>(std::to_string(stateTrueNumLineStart));								
								newEdgeF->AddElement(std::move(startLineF));
								// attribute sourcecode
								std::string falseSourceCond = "[!" + stateTrueLogRows[k].controlCode + "]";
								std::unique_ptr<EdgeData> sourcecodeF = std::make_unique<SourceCode>(falseSourceCond);
								newEdgeF->AddElement(std::move(sourcecodeF));
								// attribute control
								std::unique_ptr<EdgeData> controlF = std::make_unique<Control>("condition-false");
								newEdgeF->AddElement(std::move(controlF));
								
								this->automata->AddEdge(std::move(newEdgeF));
							}							
							
						}	
						
						
					}else{
						// attribute sourcecode
						std::unique_ptr<EdgeData> sourcecode = std::make_unique<SourceCode>(stateTrueLogRows[k].sourceCode);
						newEdge->AddElement(std::move(sourcecode));
						this->automata->AddEdge(std::move(newEdge));
					}
					
					
				}
				
			}
		}      
		
	}
    
}

// when we don't have KLEE values
void SVCompWitness::makeViolationAutomataAux() 
{
	
	Map2Check::Log::Debug("Starting Violation Automata Generation Mode 2");
	unsigned runState = 0;  
	std::ostringstream cnvt; 
	cnvt.str("");
	cnvt << "s" << runState; 
	
    std::unique_ptr<Node> startNode = std::make_unique<Node>(cnvt.str());
    std::string lastStateId;// = "s0";
    runState++; //s1
    
    
    std::unique_ptr<NodeElement> entryNode = std::make_unique<EntryNode>();
    startNode->AddElement(std::move(entryNode));

    std::vector<Tools::StateTrueLogRow> stateTrueLogRows = Tools::StateTrueLogHelper::getListLogFromCSV();   
            
    //The only diference that is not removed the error location
    
    std::vector<Tools::TrackBBLogRow> trackBBLogRows = Tools::TrackBBLogHelper::getListLogFromCSV();
    
    if(stateTrueLogRows.size() == 0 || trackBBLogRows.size() == 0) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>("s1");
        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>("s0", "s1");
        this->automata->AddEdge(std::move(newEdge));
        this->automata->AddNode(std::move(newNode));
    }
    
    this->automata->AddNode(std::move(startNode));    
    
    /**
     * Creating the automata nodes
     * The total number of automata nodes is egual to number lines in 
     * automata_list_log.st file take into accounting the BB executed in
     * track_bb_log.st file
     * */
    
    unsigned runSearchIndx = 0;
    
    for(int i = 0; i < trackBBLogRows.size(); i++) {
		
		int trackBBLineNum = std::stoi(trackBBLogRows[i].numLineInBB);
		std::string trackBBFunctName = trackBBLogRows[i].functionName;
		bool flagCreateNewNode = false;
		
		int stateTrueNumLineBeginBB;
		int stateTrueNumLineStart;		
		
		bool searchLineBB = true;
		
		cout << i << "-------------- \n";
		
        // Checking if the state in stateTrueLogRows was executed in TrackBBLogRow
        for(int k = 0; (k < stateTrueLogRows.size() && searchLineBB); k++) 
        {	
			stateTrueNumLineBeginBB = std::stoi(stateTrueLogRows[k].numLineBeginBB);
			stateTrueNumLineStart = std::stoi(stateTrueLogRows[k].numLineStart);
			
			if(trackBBFunctName == stateTrueLogRows[k].functionName)
			{				
				if( (trackBBLineNum >= stateTrueNumLineBeginBB) && (trackBBLineNum <= stateTrueNumLineStart))
				{
					searchLineBB = false; //Stop search
					
					lastStateId =  cnvt.str();
					
					cnvt.str("");
					cnvt << "s" << runState;
					std::string tmpLastStateId;
					
					// This state (i.e., the BB) was executed
					// CREATING NODE
					tmpLastStateId = lastStateId;
					std::unique_ptr<Node> newNode = std::make_unique<Node>(cnvt.str());																
					this->automata->AddNode(std::move(newNode));
					//lastStateId =  cnvt.str();
					runState++; 						 
										
					// Create the edge to the new node
					// CREATING EDGE
					//cout << lastStateId << "," << cnvt.str() << "\n";
					std::unique_ptr<Edge> newEdge = std::make_unique<Edge>(lastStateId, cnvt.str());					
										
					// attribute startline
					std::unique_ptr<EdgeData> startLine = std::make_unique<StartLine>(std::to_string(stateTrueNumLineStart));
					newEdge->AddElement(std::move(startLine));
					//this->automata->AddEdge(std::move(newEdge));
					
										
					if(std::stoi(stateTrueLogRows[k].hasControlCode))
					{	
						cout << "CONTROL \n";
						// attribute sourcecode
						std::unique_ptr<EdgeData> sourcecode = std::make_unique<SourceCode>(stateTrueLogRows[k].controlCode);
						newEdge->AddElement(std::move(sourcecode));
						
						
						// search in trackBBLogRows which condition (TRUE or FALSE) by line number in 
						// stateTrueLogRows was executed
						int tmpCount=i+1; //true cond in trackBBLogRows, i.e., next line executed	
						bool hasTrueCond = false;
											
						//=============== TRUE COND
						if(tmpCount < trackBBLogRows.size())
						{							
							cout << std::stoi(stateTrueLogRows[k].numLineControlTrue) << "==" << std::stoi(trackBBLogRows[tmpCount].numLineInBB) << "\n";
							if(std::stoi(stateTrueLogRows[k].numLineControlTrue) == std::stoi(trackBBLogRows[tmpCount].numLineInBB))
							{								
								cout << " TRUE COND\n";
								//i = tmpCount; //true cound found in trackBBLogRows	
								//cout << i << "---------- \n";
														
								// create edge
								std::unique_ptr<EdgeData> control = std::make_unique<Control>("condition-true");
								newEdge->AddElement(std::move(control));								
								this->automata->AddEdge(std::move(newEdge));
								hasTrueCond = true;
								
								
								//  create node
								//cnvt.str("");
								//cnvt << "s" << runState;
								//std::unique_ptr<Node> newNodeT = std::make_unique<Node>(cnvt.str());
								//this->automata->AddNode(std::move(newNodeT));
								//lastStateId =  cnvt.str();
								//runState++;
								
							}
						}
						
						//=============== FALSE COND
						/**
						 *  creating a edge to its negation
						 *  create a new node, only if we have a false cond, otherwise we point to the same node
						 *	from true cond							
						 * */
						if(tmpCount < trackBBLogRows.size() && !hasTrueCond)
						{							
							cout << std::stoi(stateTrueLogRows[k].numLineControlFalse) << "==" << std::stoi(trackBBLogRows[tmpCount].numLineInBB) << "\n";
							if(std::stoi(stateTrueLogRows[k].numLineControlFalse) == std::stoi(trackBBLogRows[tmpCount].numLineInBB))
							{
								// Create a new node for false cond													
								//i = tmpCount; //false cond found in trackBBLogRows
								//cout << i << "---------- \n";
								//  create node
								//cnvt.str("");
								//cnvt << "s" << runState;
								//std::unique_ptr<Node> newNodeF = std::make_unique<Node>(cnvt.str());
								//this->automata->AddNode(std::move(newNodeF));
								//lastStateId =  cnvt.str();
								//runState++; 
								
								// Create the edge to the new node
								//std::unique_ptr<Edge> newEdgeF = std::make_unique<Edge>(lastStateId, cnvt.str());
								
								// attribute startline
								std::unique_ptr<EdgeData> startLineF = std::make_unique<StartLine>(std::to_string(stateTrueNumLineStart));								
								newEdge->AddElement(std::move(startLineF));
								// attribute sourcecode
								std::string falseSourceCond = "[!" + stateTrueLogRows[k].controlCode + "]";
								std::unique_ptr<EdgeData> sourcecodeF = std::make_unique<SourceCode>(falseSourceCond);
								newEdge->AddElement(std::move(sourcecodeF));
								// attribute control
								std::unique_ptr<EdgeData> controlF = std::make_unique<Control>("condition-false");
								newEdge->AddElement(std::move(controlF));
								
								this->automata->AddEdge(std::move(newEdge));
							}							
							
						}	
					}else{
						// attribute sourcecode
						std::unique_ptr<EdgeData> sourcecode = std::make_unique<SourceCode>(stateTrueLogRows[k].sourceCode);
						newEdge->AddElement(std::move(sourcecode));
						this->automata->AddEdge(std::move(newEdge));						
					}
					
					
				}
				
			}			
		}
		
		//if(!searchLineBB){
			//i++;
			//cout << i << "---------- \n";
		//} 
		     
		
	}
    
	
    
}


void SVCompWitness::makeViolationAutomata() {

    Map2Check::Log::Debug("Starting Violation Automata Generation");
    unsigned lastState = 0;
    std::string lastStateId = "s0";
    std::unique_ptr<Node> startNode = std::make_unique<Node>("s0");    
    lastState++;

    std::unique_ptr<NodeElement> entryNode = std::make_unique<EntryNode>();
    startNode->AddElement(std::move(entryNode));

    std::vector<Tools::KleeLogRow> kleeLogRows = Tools::KleeLogHelper::getListLogFromCSV();
    std::vector<Tools::ListLogRow> listLogRows = Tools::ListLogHelper::getListLogFromCSV();
    // for target function error
    std::vector<Tools::TrackBBLogRow> trackBBLogRows = Tools::TrackBBLogHelper::getListLogFromCSV();
    

    if(kleeLogRows.size() == 0 && listLogRows.size() == 0 && trackBBLogRows.size() == 0) {		
        std::unique_ptr<Node> newNode = std::make_unique<Node>("s1");

        std::unique_ptr<NodeElement> violationNode = std::make_unique<ViolationNode>();
        newNode->AddElement(std::move(violationNode));

        std::unique_ptr<Edge> newEdge = std::make_unique<Edge>("s0", "s1");
        this->automata->AddEdge(std::move(newEdge));
        this->automata->AddNode(std::move(newNode));
    }else if(kleeLogRows.size() == 0 && (listLogRows.size() > 0 || trackBBLogRows.size() > 0)){
		//this->automata->AddNode(std::move(startNode));		
		this->makeViolationAutomataAux();
	}else{
        
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
}
