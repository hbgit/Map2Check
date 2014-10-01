#!/usr/bin/env python
from __future__ import print_function
# -*- coding: latin1 -*-

__author__ = 'Herbert OLiveira Rocha'

#Python
import sys
import os
import re
import csv

import networkx as nx
import matplotlib.pyplot as plt


# Create a directed graph
Gm = nx.DiGraph(sourcecodelang="C")

# Adding a node
Gm.add_node("N1",entry="True")
Gm.add_node("N2")
Gm.add_node("N3")

# Adding edges
dic_edgeeattr = {"originline":11,
             "origintokens":"15,16",
             "sourcecode":"int a;",
             "originfile":"path-examples/example4_false-unreach-label.c",
             "tokens":"51,61",
             "assumption":"r = 0;"}
Gm.add_edge("N1","N2",dic_edgeeattr)

Gm.add_edge("N2","N3")


s='\n'.join(nx.generate_graphml(Gm))

print(s)

limits=plt.axis('off') # turn of axis
nx.draw_networkx(Gm,pos=nx.spring_layout(Gm),with_labels=True)
#nx.draw(Gm)

plt.show()