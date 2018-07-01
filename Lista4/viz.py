#%%
import pydot

G = pydot.Dot(graph_type='graph')

node0 = pydot.Node("abc")
G.add_node(node0)

node1 = pydot.Node("kkk")
G.add_node(node1)

edge = pydot.Edge(node0, node1)
G.add_edge(edge)

#%%
from IPython.display import Image, display
im = Image(G.create(format='png'))
display(im)