#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/graph/adjacency_list.hpp> 
#include "tools.h"

using namespace std;
  
class Witness {
 public:   
  Witness(string kleeResultFolder);  

  enum class KleeStatus {
    OK,
    ERROR  
  };
  
  struct KleeResult {
    KleeStatus kleeStatus;
    vector<int> states;
    string name;
  };  
 private:  
  void genKleeFilesListFromFolder(string kleeResultFolder);  
  static void convertKleeFileToTextFile(KleeResult* kleeResult);
  void generateWitness();
  vector<KleeResult> kleeResults;

  enum class StateStatus {
    SINK,         // Final State with no ERROR
    ACCEPTING,    // Final State with ERROR
    NON_ACCEPTING // Normal State
  };
  
  struct State {
    int id;
    StateStatus status;
  };

   struct Edge {
    int value;
  };
  
   typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, State, Edge > Graph;
   /* typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, State > Graph; */
   typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
   typedef boost::graph_traits<Graph>::edge_descriptor edge_t;

   Graph graph;

   void generateEdge(vertex_t origin, vertex_t destiny, int weight);
   void changeStateStatus(vertex_t state, KleeStatus status);
};

class WitnessException : public runtime_error {
 public:
 WitnessException(string message) : runtime_error(message) {}
  virtual const char* what() const throw();
};

class InvalidKleeFolderException : public WitnessException {
 public:
 InvalidKleeFolderException(string folder) : WitnessException(""), folder(folder) {}
  virtual const char* what() const throw();

 private:
  string folder;
};

class InvalidKtestBinException : public WitnessException {
   public:
 InvalidKtestBinException() : WitnessException("") {}
  virtual const char* what() const throw();  
};

class ErrorOpeningFileException : public WitnessException {
 public:
 ErrorOpeningFileException() : WitnessException("") {}
  virtual const char* what() const throw();    
};

class AmbigousKleeResult : public WitnessException {
 public:
 AmbigousKleeResult() : WitnessException("") {}
  virtual const char* what() const throw();    
};
