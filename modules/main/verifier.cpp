#include "verifier.h"
#include "log.h"
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp> 
#include <boost/graph/graphml.hpp>


namespace fs = boost::filesystem;

// TODO: make constructor private and create static method to execute
Witness::Witness(string kleeResultFolder) {
  genKleeFilesListFromFolder(kleeResultFolder);
  generateWitness();

#ifdef DEBUG
  boost::dynamic_properties dp;
  boost::write_graphml(std::cout, this->graph, dp);
#endif
}

void Witness::convertKleeFileToTextFile(KleeResult* kleeResult) {
  Map2Check::Log::Debug("Converting ktest to test file");
  
  std::ostringstream command; 
  command.str("");  
  command << Map2Check::Tools::ktestBinary << " --write-ints ";
  command << kleeResult->name << ".ktest";
  command << " >> " << kleeResult->name << ".test";

  Map2Check::Log::Debug(command.str());
  system(command.str().c_str());

  std::regex reData("object .*: data: (.*)");
  // TODO: Use regex to determine number of objects
  //       and use fixed array list instead of vector
  
  // std::regex reNumObjects("num objects: (.*)");
  std::smatch match;
  std::string result;
  
  std::string line;
  std::ostringstream filename;
  filename.str("");
  filename  << kleeResult->name << ".test";

  std::ifstream testFile;
  testFile.open(filename.str().c_str());

  if(testFile.is_open()) {
    while ( std::getline (testFile,line) )
    {
      if (std::regex_search(line, match, reData) && match.size() > 1) {
	Map2Check::Log::Debug("Adding " + match.str(1) + " to " + kleeResult->name);
	int result = std::stoi(match.str(1));
	kleeResult->states.push_back(result);	
      }       
    }
    testFile.close();
    
  }
  else {
    throw ErrorOpeningFileException();
  }  
}  

void Witness::genKleeFilesListFromFolder(std::string kleeResultFolder) {
  Map2Check::Log::Debug("Obtaining files from folder: " + kleeResultFolder);

  // Checks if folder exists
  if (!fs::exists(kleeResultFolder) || !fs::is_directory(kleeResultFolder))
  {
    throw InvalidKleeFolderException(kleeResultFolder);   
  }
  else {
    Map2Check::Log::Debug("Folder does exist, continuing");
    fs::directory_iterator end_iter;

    std::regex ktest("(.*)\.ktest");
    std::regex error("(.*)\.assert\.err");
    for( fs::directory_iterator dir_iter(kleeResultFolder) ; dir_iter != end_iter ; ++dir_iter)
      {
	KleeResult result;
	result.name = "";
	
	if (fs::is_regular_file(dir_iter->status()) )
	  {	   
	    string current_file = dir_iter->path().string();
	    bool kleeError = false;
	    
	    if(std::regex_match (current_file, ktest)) {
	      Map2Check::Log::Debug("Found ktest file: " + current_file); 
	      std::size_t pos = current_file.find(".ktest");
	      result.name =  current_file.substr(0,pos);
	      result.kleeStatus = KleeStatus::OK;
	      
	    } else if (std::regex_match (current_file, error)) {
	      Map2Check::Log::Debug("Found err file: " + current_file);
	      std::size_t pos = current_file.find(".assert.err");
	      result.name =  current_file.substr(0,pos);
	      result.kleeStatus = KleeStatus::ERROR;
	      
	    }

	    if (result.name != "") {
	      bool notFound = true;
	      string key = result.name;
	      
	      std::vector<KleeResult>::iterator it;
	      it = std::find_if (kleeResults.begin(),
				 kleeResults.end(),
				 [key] (const KleeResult& o) -> bool {
				   return o.name == key;});

	      if (it != kleeResults.end()) {
		Map2Check::Log::Debug("Found old entry for " + it->name);
		it->kleeStatus   = KleeStatus::ERROR;
	      }
	      else {
		Map2Check::Log::Debug("Didn't found old entry for " + result.name);
		kleeResults.push_back(result);
		
	      }
	     
	    }
	  }
      }
  }

  if (kleeResults.size() == 0) {
    throw InvalidKleeFolderException(kleeResultFolder);  
  }

  if(!fs::is_regular_file(Map2Check::Tools::ktestBinary)) {
    throw InvalidKtestBinException();
  }
  
  for(int i; i < this->kleeResults.size(); i++) {
    convertKleeFileToTextFile(&this->kleeResults[i]);
    
  }
  
}

void Witness::generateWitness() {
   Map2Check::Log::Debug("Started graph creation");

   // Creates initial state for graph   
   vertex_t initialState = boost::add_vertex(this->graph);
   this->graph[initialState].status = StateStatus::NON_ACCEPTING;
   
   for(auto i = this->kleeResults.begin();
       i != this->kleeResults.end(); i++) {
     KleeResult kleeResult = *i;
     Map2Check::Log::Debug("Got test: " + kleeResult.name);
     vertex_t currentState = initialState;
     
     for(int j = 0; j < kleeResult.states.size(); j++) {
       Witness::Graph::edge_iterator ei, eiEnd;
       std::vector<edge_t> adjacentEdges;
       
       for (boost::tie(ei, eiEnd) = boost::edges(this->graph);
	    ei != eiEnd; ++ei) {
	 vertex_t source = boost::source(*ei, this->graph);
	 if(source == currentState) {	  
	   adjacentEdges.push_back(*ei);
	   Map2Check::Log::Debug("Found transition");
	 }	 
       }

       int value = kleeResult.states[j];
       bool foundEdge = false;

       for (auto adj = adjacentEdges.begin();
	    adj != adjacentEdges.end(); adj++) {
	 if(this->graph[*adj].value == value) {
	   Map2Check::Log::Debug("Found edge for transition with value: " + std::to_string(value));
	   foundEdge = true;
	   currentState  = boost::target(*adj, this->graph);
	   break;
	 }
       }

       if (!foundEdge) {
	 Map2Check::Log::Debug("Creating new edge with value: " +  std::to_string(value));
	 vertex_t source = currentState;
	 currentState = boost::add_vertex(this->graph);
	 this->graph[currentState].status = StateStatus::NON_ACCEPTING;
	 this->generateEdge(source, currentState, value);
       }
     }        

     this->changeStateStatus(currentState, kleeResult.kleeStatus);  
   }
   
}

void Witness::changeStateStatus(vertex_t state, KleeStatus status) {
  StateStatus stateStatus = status == KleeStatus::OK ?
    StateStatus::SINK : StateStatus::ACCEPTING;
  if(this->graph[state].status == StateStatus::NON_ACCEPTING)
    {
      Map2Check::Log::Debug("Changing state status");
      this->graph[state].status = stateStatus;
    }
  else if ( this->graph[state].status != stateStatus) {
    throw AmbigousKleeResult();
  }	  
}

void Witness::generateEdge(vertex_t origin,
			   vertex_t destiny,
			   int weight) {

  edge_t e; bool b;   
  boost::tie(e,b) = boost::add_edge(origin, destiny, this->graph);

  this->graph[e].value = weight;  
}


const char* WitnessException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}

const char* InvalidKleeFolderException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << "Folder " <<  this->folder << " does not contain Klee Results!";
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}

const char* InvalidKtestBinException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << "Could not find binary " <<  Map2Check::Tools::ktestBinary;
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}

const char* ErrorOpeningFileException::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << "Could not open file in reading mode";
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}

const char* AmbigousKleeResult::what() const throw() {
  std::ostringstream cnvt; 
  cnvt.str("");
  cnvt << "Klee determined state to be Sink and Accepting";
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();  
}
