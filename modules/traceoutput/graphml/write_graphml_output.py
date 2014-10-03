#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-


__author__ = 'Herbert OLiveira Rocha'

#Python
import sys
import re

import networkx as nx
import matplotlib.pyplot as plt


# From project

import reader_csv_output


class WriteGraphMLOutput(object):

    def __init__(self):
        self.actual_name_node = ''
        self.actual_name_edge = ''
        self.pathanalyzedprog = ''
        self.list_body_outmapbycolumns = None
        self.listdataviolated = []
        # Attributes of the graph according to SVCOMP
        # Is a dictionary with a KEY:[Value,Type]
        self.dic_edges_attributes = {
            "assumption": None, #"str"],
            "sourcecode": None, #"str"],
            "tokenSet": None, #"str"],
            "originTokenSet": None, #"str"],
            "negativeCase": None, #"str"],
            "lineNumberInOrigin": None, #"int"],
            "originFileName": None, #"str"],
            "enterFunction": None, #"str"],
            "returnFromFunction": None, #"str"]
        }
        self.dic_nodes_attributes = {
            "nodeType": None, #"str"
            "isFrontierNode": None, #"bool"
            "isViolationNode": None, #"bool"
            "isEntryNode": None, #"bool"
            "isSinkNode": None #"bool"
        }


    def reset_dic_attributes(self):
        self.dic_edges_attributes = {
            "assumption": None, #"str"],
            "sourcecode": None, #"str"],
            "tokenSet": None, #"str"],
            "originTokenSet": None, #"str"],
            "negativeCase": None, #"str"],
            "lineNumberInOrigin": None, #"int"],
            "originFileName": None, #"str"],
            "enterFunction": None, #"str"],
            "returnFromFunction": None, #"str"]
        }
        self.dic_nodes_attributes = {
            "nodeType": None, #"str"
            "isFrontierNode": None, #"bool"
            "isViolationNode": None, #"bool"
            "isEntryNode": None, #"bool"
            "isSinkNode": None #"bool"
        }



    def preprocess_outmap(self, _pathfileoutmap):

        filedata = open(_pathfileoutmap, "r")
        linesoutmap = filedata.readlines()
        filedata.close()

        # Isolating data from map2check output
        # >> Path of the analyzed C program
        matchnameprog = re.search(r'>>> Trace of (.*)', linesoutmap[1])
        if matchnameprog:
            self.pathanalyzedprog = matchnameprog.group(1)

        # >> The body of the trace
        startpointbody = 5
        endpointbody = 0
        for index, line in enumerate(linesoutmap):
            if index >= startpointbody:
                if len(line.strip()) == 0:
                    endpointbody = index
                    break

        csvrun = reader_csv_output.ReaderCsv()
        self.list_body_outmapbycolumns = csvrun.load_csv_list(linesoutmap[startpointbody:endpointbody],"|")

        # >> Data about the property violation
        for index, line in enumerate(linesoutmap[(endpointbody+1):len(linesoutmap)]):
            # The number of the line where the property has been violated
            matchnumlineviolated = re.search(r'Location at original code in line:(.*)$', line)
            if matchnumlineviolated:
                self.listdataviolated.append(matchnumlineviolated.group(1).strip())


    def check_dict_to_set(self, _dic_attributes):
        final_dic_attr = {key: _dic_attributes[key] for key in _dic_attributes if not _dic_attributes[key] == None }
        print(final_dic_attr)
        return final_dic_attr


    def generate_graphml(self):
        """
        The graph is generated based on the data collected from the output of the tool
        """
        # Create a directed graph
        Gmap = nx.DiGraph(sourcecodeLanguage="C")

        basenodename = 'N'
        lastnode = None

        # Adding the nodes and the edges of the graph
        #print(self.list_body_outmapbycolumns[])
        for index, eachline in enumerate(self.list_body_outmapbycolumns['Line']):
            actualnodename = basenodename + str(index)
            self.reset_dic_attributes()

            # adding the nodes
            if index == 0:
                self.dic_nodes_attributes['isEntryNode'] = "True"
                dic_attr = self.check_dict_to_set(self.dic_nodes_attributes)
                Gmap.add_node(actualnodename,dic_attr)
            else:
                Gmap.add_node(actualnodename)
                # adding the edge from the last node added to the actual node
                lastnodename = basenodename + str(index-1)
                # adding edge attributes
                self.dic_edges_attributes['lineNumberInOrigin'] = eachline.strip()
                self.dic_edges_attributes['originFileName'] = self.pathanalyzedprog.strip()
                self.dic_edges_attributes['assumption'] = self.list_body_outmapbycolumns['Var Value'][index].strip()
                dic_attr = self.check_dict_to_set(self.dic_edges_attributes)

                Gmap.add_edge(lastnodename,actualnodename,dic_attr)


        # TODO: Add the node for property violation


        # Generating the graph in the GraphML format
        string_graph = '\n'.join(nx.generate_graphml(Gmap,encoding='utf-8'))
        print("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>")
        print(string_graph)

        #limits = plt.axis('off') # turn of axis
        #nx.draw_networkx(Gmap,pos=nx.spring_layout(Gmap),with_labels=True)
        #plt.show()










# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":


    runwriteml = WriteGraphMLOutput()
    runwriteml.preprocess_outmap(sys.argv[1])
    runwriteml.generate_graphml()

    # # Create a directed graph
    # Gm = nx.DiGraph(sourcecodelang="C")
    #
    # #nx.set_node_attributes()
    # # Adding a node
    # Gm.add_node("N1",entry="True")
    # Gm.add_node("N2")
    # Gm.add_node("N3")
    #
    # # Adding edges
    # dic_edgeeattr = {"originline":11,
    #              "origintokens":"15,16",
    #              "sourcecode":"int a;",
    #              "originfile":"path-examples/example4_false-unreach-label.c",
    #              "tokens":"51,61",
    #              "assumption":"r = 0;"}
    # Gm.add_edge("N1","N2",dic_edgeeattr)
    #
    # Gm.add_edge("N2","N3")
    #
    # s='\n'.join(nx.generate_graphml(Gm,encoding='utf-8'))
    #
    # # TODO: In the end replace the id by correct name based on its attr.name
    #
    # print(s)
    #
    #
    # #limits=plt.axis('off') # turn of axis
    # #nx.draw_networkx(Gm,pos=nx.spring_layout(Gm),with_labels=True)
    # #plt.show()