#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

/* This header describes all used witness arguments from
*  https://github.com/sosy-lab/sv-witnesses
*/


namespace Map2Check {
    class DataElement {
        protected:            
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "DataElement";
                return cnvt.str();
            }
        public:
            DataElement() {}           
            operator std::string() {
                return this->convertToString();
            }
    }; 

    enum class WitnessTypeValues {
        CORRECTNESS,
        VIOLATION
    };

    class WitnessType : public DataElement {
        protected:
            WitnessTypeValues witnessType;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"witness-type\">";
                
                switch(this->witnessType) {
                    case WitnessTypeValues::CORRECTNESS:
                        cnvt << "correctness_witness";
                        break;
                    case WitnessTypeValues::VIOLATION:
                        cnvt << "violation_witness";
                        break;
                }               
                cnvt << "</data>";
                return cnvt.str();
            }

        public: 
            WitnessType(WitnessTypeValues witnessType) : DataElement(), witnessType(witnessType) {}   

    }; 

    enum class SupportedSourceCodeLang {
        C
    };

    class SourceCodeLang : public DataElement {
        protected:
            SupportedSourceCodeLang language;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"sourcecodelang\">";
                
                switch(this->language) {
                    case SupportedSourceCodeLang::C:
                        cnvt << "C";
                        break;
                }               
                cnvt << "</data>";
                return cnvt.str();
            }
        public:
            SourceCodeLang(SupportedSourceCodeLang language) : DataElement(), language(language) {}
    };

    class Producer : public DataElement {
        protected:
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"producer\">";
                cnvt << "Map2Check";
                cnvt << "</data>";
                return cnvt.str();
            }
        public:
            Producer() : DataElement() {}    
    };

    enum class SpecificationType {
        FREE,
        MEMLEAK,
        DEREF,
        TARGET
    };

    class Specification : public DataElement {
        protected:
            std::string value;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"specification\">";
                cnvt << this->value;
                cnvt << "</data>";
                return cnvt.str();
            }

        public:
            Specification(SpecificationType type) : DataElement() {
                switch(type) {
                    case SpecificationType::FREE: 
                        this->value = "CHECK( init(main()), LTL(G valid-free) )";
                        break;
                    case SpecificationType::MEMLEAK:
                        this->value = "CHECK( init(main()), LTL(G valid-memtrack) )";
                        break;
                    case SpecificationType::DEREF:
                        this->value = "CHECK( init(main()), LTL(G valid-deref) )";
                        break;
                    case SpecificationType::TARGET:
                        this->value = "ERROR";                        
                        break;    
                }

            }
            Specification(SpecificationType type, std::string target) : DataElement() {
                switch(type) {
                    case SpecificationType::FREE: 
                        this->value = "ERROR";
                        break;
                    case SpecificationType::MEMLEAK:
                        this->value = "ERROR";
                        break;
                    case SpecificationType::DEREF:
                        this->value = "ERROR";
                        break;
                    case SpecificationType::TARGET:
                        std::ostringstream cnvt;
                        cnvt.str("");
                        cnvt << "CHECK( init(main()), LTL(G ! call(";
                        cnvt << target;
                        cnvt << "())) )";
                        this->value = cnvt.str();
                        break;    
                }
            }
    };

    class ProgramFile : public DataElement {
        protected:
            std::string path;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"programfile\">";
                cnvt << this->path;
                cnvt << "</data>";
                return cnvt.str();
            }
        public:
            ProgramFile(std::string path) : path(path), DataElement() {} 

    };

    class ProgramHash : public DataElement {
        protected:
            std::string hash;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"programhash\">";
                cnvt << this->hash;
                cnvt << "</data>";
                return cnvt.str();
            }
        public:
            ProgramHash(std::string hash) : hash(hash), DataElement() {} 

    };

    enum class ArchitectureType {
        Bit32,
        Bit64
    };

    class Architecture : public DataElement {
        protected:
            ArchitectureType type;
            virtual std::string convertToString() {
                std::ostringstream cnvt;
                cnvt.str("");
                cnvt << "\t\t<data key=\"architecture\">";
                switch(this->type) {
                    case ArchitectureType::Bit32:
                        cnvt << "32bit";
                        break;
                    case ArchitectureType::Bit64:
                        cnvt << "64bit";
                        break;
                }
                cnvt << "</data>";
                return cnvt.str();
            }
        public:
            Architecture(ArchitectureType type) : DataElement(),    type(type) {}
    };

    class NodeElement {
    protected:
    	virtual std::string convertToString() {
    	                std::ostringstream cnvt;
    	                cnvt.str("");
    	                cnvt << "NodeElement";
    	                return cnvt.str();
    	}
    public:
    	NodeElement() {}
    	operator std::string() {
    		return this->convertToString();
    	}
    };

    class ViolationNodeElement : public NodeElement {
    protected:
    	virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");
    	    cnvt << "ViolationNodeElement";
    	    return cnvt.str();
    	}
    public:
    	ViolationNodeElement() : NodeElement() {}
    };

    class EntryNode : public ViolationNodeElement {
    protected:
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t\t<data key=\"entry\">true</data>";
    	    return cnvt.str();
    	}
    public:
        EntryNode() : ViolationNodeElement() {}    
    };

    class SinkNode : public ViolationNodeElement {
    protected:
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t\t<data key=\"sink\">true</data>";
    	    return cnvt.str();
    	}
    public:
        SinkNode() : ViolationNodeElement() {}        
    };

    class ViolationNode : public ViolationNodeElement {
    protected:
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t\t<data key=\"violation\">true</data>";
    	    return cnvt.str();
    	}
    public:
        ViolationNode() : ViolationNodeElement() {}        
    };

    class EdgeData {
    protected:
    	virtual std::string convertToString() {
    	                std::ostringstream cnvt;
    	                cnvt.str("");
    	                cnvt << "EdgeData";
    	                return cnvt.str();
    	}
    public:
    	EdgeData() {}
    	operator std::string() {
    		return this->convertToString();
    	}

    };

    class StartLine : public EdgeData {
    protected:
        std::string line;
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t\t<data key=\"startline\">";
            cnvt << this->line;
            cnvt << "</data>";

    	    return cnvt.str();
    	}

     public:
        StartLine(std::string line) : line(line), EdgeData() {}  

    };

    class AssumptionEdgeData : public EdgeData {
    protected:
        std::string result;
        std::string functionName;
        std::string scope;
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t\t<data key=\"assumption\">\\result == ";
            cnvt << this->result;
            cnvt << "</data>\n";

            cnvt << "\t\t\t<data key=\"assumption.scope\">";
            cnvt << this->scope;
            cnvt << "</data>\n";

            cnvt << "\t\t\t<data key=\"assumption.resultfunction\">";
            cnvt << this->functionName;
            cnvt << "</data>";

    	    return cnvt.str();
    	}

     public:
        AssumptionEdgeData(std::string result, std::string functionName, std::string scope) :
            result(result), functionName(functionName), scope(scope),EdgeData() {} 

    };


    class Node {
    protected:
        std::string id;
        std::vector<std::unique_ptr<NodeElement>> elements;
        virtual std::string convertToString() {
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
    public:
        Node(std::string id) : id(id) {}
        void AddElement(std::unique_ptr<NodeElement> element);
    	operator std::string() {
    		return this->convertToString();
    	}
    };

    class Edge {
    protected:
        std::string source;
        std::string target;
        std::vector<std::unique_ptr<EdgeData>> elements;
        virtual std::string convertToString() {
    		std::ostringstream cnvt;
    	    cnvt.str("");            
    	    cnvt << "\t\t<edge source=\"";
            cnvt << this->source;
            cnvt << "\" target=\"";
            cnvt << this->target;
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
                cnvt << "\t\t</edge>";
            }
    	    return cnvt.str();
    	}

    public:
        Edge(std::string source, std::string target) : source(source), target(target) {}
        void AddElement(std::unique_ptr<EdgeData> element);
    	operator std::string() {
    		return this->convertToString();
    	}    

    };


    class Graph {
    protected:
        std::string id;
        std::vector<std::unique_ptr<DataElement>> elements;
        std::vector<std::unique_ptr<Node>> states;
        std::vector<std::unique_ptr<Edge>> transitions;
        virtual std::string convertToString() {
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
    public:
        Graph() {}
        void AddElement(std::unique_ptr<DataElement> element);
        void AddNode(std::unique_ptr<Node> node);
        void AddEdge(std::unique_ptr<Edge> edge);
    	operator std::string() {
    		return this->convertToString();
    	}    

    };

    class SVCompWitness {
    protected:        
        Graph automata;
    public:
        SVCompWitness(std::string programPath, std::string programHash, std::string targetFunction = "");    
        void Testify();

    }; 

}

