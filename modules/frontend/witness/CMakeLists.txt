add_library(Witness OBJECT witness.cpp)
set_target_properties(Witness PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})

add_library(Edge OBJECT edge.cpp)
set_target_properties(Edge PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})

add_library(Graph OBJECT graph.cpp)
set_target_properties(Graph PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})

add_library(Node OBJECT node.cpp)
set_target_properties(Node PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
