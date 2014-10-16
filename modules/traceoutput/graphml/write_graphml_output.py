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
        self.listdatatokens = None
        self.enable_enterFunction_attr = True
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
                break


    def check_dict_to_set(self, _dic_attributes):
        final_dic_attr = {key: _dic_attributes[key] for key in _dic_attributes if not _dic_attributes[key] == None }
        #print(final_dic_attr)
        return final_dic_attr


    def search_linenum_in_listdatatokens(self, _numline):

        for index, item in enumerate(self.listdatatokens):
            if item[0] == _numline:
                return index

        return -1 # not found



    def generate_graphml(self):
        """
        The graph is generated based on the data collected from the output of the tool
        """
        # Create a directed graph
        Gmap = nx.DiGraph(sourcecodeLanguage="C")

        basenodename = 'N'
        lastnode = None

        ldfunctname = None
        dataidfunctname = None
        if self.enable_enterFunction_attr:
            # loading data id function name
            ldfunctname = reader_csv_output.ReaderCsv()
            dataidfunctname = ldfunctname.loadCsvFile("/tmp/tmp_idfunct.map2check",";")


        # Adding the nodes and the edges of the graph
        #print(self.list_body_outmapbycolumns[])
        #print(self.listdatatokens)
        nextnode = ''
        lastnumnode = 0
        list_enterfunct = []
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
                #lastnodename = basenodename + str(index-1)

            # identify the next node to add the edge
            if (index+1) <= len(self.list_body_outmapbycolumns['Line']):
                nextnode = basenodename + str(index+1)

                # adding edge attributes
                self.dic_edges_attributes['lineNumberInOrigin'] = eachline.strip()

                #get data tokens attributes
                resultsearchtk = self.search_linenum_in_listdatatokens(int(eachline.strip()))
                if resultsearchtk != -1:
                    #print('\n'.join(self.listdatatokens[resultsearchtk][1][1]))
                    numbertokens = ', '.join(str(x) for x in self.listdatatokens[resultsearchtk][1][0])
                    texttokens = '\n'.join(self.listdatatokens[resultsearchtk][1][1])
                    self.dic_edges_attributes['tokenSet'] = numbertokens
                    self.dic_edges_attributes['originTokenSet'] = numbertokens

                    if self.enable_enterFunction_attr:
                        # >>> Adding function attribute
                        # identify the ID of the main function
                        idmainfunct = ''
                        for i, value in enumerate(dataidfunctname['functname']):
                            if value == "main":
                                idmainfunct = dataidfunctname['id'][i].strip()
                                break

                        if index+1 <= (len(self.list_body_outmapbycolumns['Function ID'])-1):
                            if self.list_body_outmapbycolumns['Function ID'][index] != \
                                self.list_body_outmapbycolumns['Function ID'][index+1] and \
                                self.list_body_outmapbycolumns['Function ID'][index+1] != idmainfunct:

                                # getting the function name
                                for i, value in enumerate(dataidfunctname['id']):
                                    if value == self.list_body_outmapbycolumns['Function ID'][index+1]:
                                        self.dic_edges_attributes['enterFunction'] = dataidfunctname['functname'][i]
                                        break

                    # TODO: returnFrom not suportted yet

                    self.dic_edges_attributes['sourcecode'] = texttokens


                self.dic_edges_attributes['originFileName'] = self.pathanalyzedprog.strip()

                # TODO: Validate in case of pointer and arrays
                # Identifying is a pointer
                if self.list_body_outmapbycolumns['Address points to'][index].strip() == "(nil)" and \
                   not self.list_body_outmapbycolumns['Var Value'][index].strip() == "--":
                    takeassumption = self.list_body_outmapbycolumns['Var Name'][index].strip() + " = " + \
                                     self.list_body_outmapbycolumns['Var Value'][index].strip() + ";"

                    self.dic_edges_attributes['assumption'] = takeassumption

                dic_attr = self.check_dict_to_set(self.dic_edges_attributes)

                Gmap.add_edge(actualnodename, nextnode, dic_attr)


        self.reset_dic_attributes()
        #print(len(self.list_body_outmapbycolumns['Line']))
        lastnumnode = len(self.list_body_outmapbycolumns['Line'])
        # Adding the nodes for property violation
        # node where the error has been identified
        propertynode = basenodename + str(lastnumnode)
        violationnode = basenodename + str(lastnumnode+1)
        Gmap.add_node(violationnode,isViolationNode=True)

        # adding edge attributes
        self.dic_edges_attributes['lineNumberInOrigin'] = self.listdataviolated[0].strip()

        #get data tokens attributes
        resultsearchtk = self.search_linenum_in_listdatatokens(int(self.listdataviolated[0].strip()))
        if resultsearchtk != -1:
            numbertokens = ', '.join(str(x) for x in self.listdatatokens[resultsearchtk][1][0])
            texttokens = '\n'.join(self.listdatatokens[resultsearchtk][1][1])
            self.dic_edges_attributes['tokenSet'] = numbertokens
            self.dic_edges_attributes['originTokenSet'] = numbertokens
            self.dic_edges_attributes['sourcecode'] = texttokens


        self.dic_edges_attributes['originFileName'] = self.pathanalyzedprog.strip()
        dic_attr = self.check_dict_to_set(self.dic_edges_attributes)

        Gmap.add_edge(propertynode, violationnode, dic_attr)
        # #
        # # node violation
        # Gmap.add_node(propertynode,isViolationNode=True)







        # Generating the graph in the GraphML format
        string_graph = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> \n"
        # old version
        string_graph += '\n'.join(nx.generate_graphml(Gmap,encoding='utf-8'))
        #return string_graph


        texttmp = string_graph.split("\n")

        # replace the ids name of the graph attributes
        # dictionary flags to identify what attributes we need to replace
        dic_attr_update = {} # ['name'] = {0 Status (True or False), 1 - old text to be replaced}

        for index, line in enumerate(texttmp):
            # # attr.name="assumption"
            # matchattr_assumption = re.search(r'attr.name=\"assumption\".*id=\"(.*)\"', line)
            # if matchattr_assumption:
            #     #matchattr_assumption.group(1)
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"assumption\"", texttmp[index])
            #     dic_attr_update["assumption"] = matchattr_assumption.group(1)
            #     continue
            #
            # matchattr_sourcecode = re.search(r'attr.name=\"sourcecode\".*id=\"(.*)\"', line)
            # if matchattr_sourcecode:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"sourcecode\"", texttmp[index])
            #     dic_attr_update["sourcecode"] = matchattr_sourcecode.group(1)
            #     continue
            #
            # matchattr_sourcecodeLanguage = re.search(r'attr.name=\"sourcecodeLanguage\".*id=\"(.*)\"', line)
            # if matchattr_sourcecodeLanguage:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"sourcecodelang\"", texttmp[index])
            #     dic_attr_update["sourcecodelang"] = matchattr_sourcecodeLanguage.group(1)
            #     continue
            #
            # matchattr_tokenSet = re.search(r'attr.name=\"tokenSet\".*id=\"(.*)\"', line)
            # if matchattr_tokenSet:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"tokens\"", texttmp[index])
            #     dic_attr_update["tokens"] = matchattr_tokenSet.group(1)
            #     continue
            #
            # matchattr_originTokenSet = re.search(r'attr.name=\"originTokenSet\".*id=\"(.*)\"', line)
            # if matchattr_originTokenSet:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"origintokens\"", texttmp[index])
            #     dic_attr_update["origintokens"] = matchattr_originTokenSet.group(1)
            #     continue
            #
            # matchattr_negativeCase = re.search(r'attr.name=\"negativeCase\".*id=\"(.*)\"', line)
            # if matchattr_negativeCase:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"negated\"", texttmp[index])
            #     dic_attr_update["negated"] = matchattr_negativeCase.group(1)
            #     continue
            #
            # matchattr_lineNumberInOrigin = re.search(r'attr.name=\"lineNumberInOrigin\".*id=\"(.*)\"', line)
            # if matchattr_lineNumberInOrigin:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"originline\"", texttmp[index])
            #     dic_attr_update["originline"] = matchattr_lineNumberInOrigin.group(1)
            #     continue
            #
            # matchattr_originFileName = re.search(r'attr.name=\"originFileName\".*id=\"(.*)\"', line)
            # if matchattr_originFileName:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"originfile\"", texttmp[index])
            #     dic_attr_update["originfile"] = matchattr_originFileName.group(1)
            #     continue
            #
            # matchattr_nodeType = re.search(r'attr.name=\"nodeType\".*id=\"(.*)\"', line)
            # if matchattr_nodeType:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"nodetype\"", texttmp[index])
            #     dic_attr_update["nodetype"] = matchattr_nodeType.group(1)
            #     continue
            #
            # matchattr_isFrontierNode = re.search(r'attr.name=\"isFrontierNode\".*id=\"(.*)\"', line)
            # if matchattr_isFrontierNode:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"frontier\"", texttmp[index])
            #     dic_attr_update["frontier"] = matchattr_isFrontierNode.group(1)
            #     continue
            #
            # matchattr_isViolationNode = re.search(r'attr.name=\"isViolationNode\".*id=\"(.*)\"', line)
            # if matchattr_isViolationNode:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"violation\"", texttmp[index])
            #     dic_attr_update["violation"] = matchattr_isViolationNode.group(1)
            #     continue

            matchattr_isEntryNode = re.search(r'attr.name=\"isEntryNode\".*id=\"(.*)\"', line)
            if matchattr_isEntryNode:
                texttmp[index] = re.sub(r"id=\".*\"", "id=\"entry\"", texttmp[index])
                dic_attr_update["entry"] = matchattr_isEntryNode.group(1)
                #continue
                break

            # matchattr_isSinkNode = re.search(r'attr.name=\"isSinkNode\".*id=\"(.*)\"', line)
            # if matchattr_isSinkNode:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"sink\"", texttmp[index])
            #     dic_attr_update["sink"] = matchattr_isSinkNode.group(1)
            #     continue
            #
            # matchattr_enterFunction = re.search(r'attr.name=\"enterFunction\".*id=\"(.*)\"', line)
            # if matchattr_enterFunction:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"enterFunction\"", texttmp[index])
            #     dic_attr_update["enterFunction"] = matchattr_enterFunction.group(1)
            #     continue
            #
            # matchattr_returnFromFunction = re.search(r'attr.name=\"returnFromFunction\".*id=\"(.*)\"', line)
            # if matchattr_returnFromFunction:
            #     texttmp[index] = re.sub(r"id=\".*\"", "id=\"returnFrom\"", texttmp[index])
            #     dic_attr_update["returnFrom"] = matchattr_returnFromFunction.group(1)
            #     continue



        # Checkout the dictionary and if need applying the properly replace in the graphml
        newgraphmltext = '\n'.join(texttmp)
        for key, value in dic_attr_update.items():
            #print(key,value)
            newgraphmltext = newgraphmltext.replace("<data key=\""+value+"\">","<data key=\""+key+"\">")
            #print("<data key=\""+key+"\">","<data key=\""+value+"\">")



        #string_graph += '\n'.join(nx.generate_graphml(Gmap,encoding='utf-8'))
        #print("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>")
        #print(string_graph)
        return newgraphmltext

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