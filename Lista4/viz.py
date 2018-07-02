#%%
from numpy.random import RandomState
from pydot import Dot, Node, Edge

__random_gen__ = RandomState(None)

class Grafo:
    def __init__(self, vertices, use_pydot=False):
        self.__verts__ = vertices
        self.__adjmx__ = [[] for i in range(vertices)]
        self.__arest__ = 0
        if use_pydot:
            self.__graph__ = Dot(graph_type='graph')
            for i in range(vertices):
                self.__graph__.add_node(Node(str(i)))
        else:
            self.__graph__ = None

    def aresta(self, v1, v2):
        if v2 in self.__adjmx__[v1]:
            return False
        self.__adjmx__[v1].append(v2)
        self.__adjmx__[v2].append(v1)
        self.__arest__ = self.__arest__ + 1
        if self.__graph__ is not None:
            self.__graph__.add_edge(Edge(str(v1), str(v2)))
        return True

    def aresta_aleat(self):
        done = False
        while not done:
            v1 = __random_gen__.randint(self.__verts__)
            v2 = v1
            while v2 == v1:
                v2 = __random_gen__.randint(self.__verts__)
            done = self.aresta(v1, v2)

    def conecta(self, alfa):
        max_art = (self.__verts__ * (self.__verts__ - 1)) // 2
        arestas = round(alfa * max_art)
        for _ in range(arestas):
            self.aresta_aleat()

    def show(self):
        if self.__graph__ is not None:
            from IPython.display import Image
            return Image(self.__graph__.create(format='png'))

    def contemC4(self):
        for v in range(self.__verts__):
            tem_caminho = [False] * self.__verts__

            for w in self.__adjmx__[v]:
                for u in self.__adjmx__[w]:
                    if u != v:
                        if tem_caminho[u]:
                            return v
                        else:
                            tem_caminho[u] = True
        return -1

    def testeC4(self, vert):
        def mostra_C4(v):
            if self.__graph__ is not None:
                for i in range(len(v)):
                    self.__graph__.get_node(str(v[i]))[0].set_style('filled')
                    self.__graph__.get_edge(str(v[i]), str(v[(i+1)%len(v)]))[0].set_style('bold')

        for v in self.__adjmx__[vert]:
            for w in self.__adjmx__[v]:
                if w != vert:
                    for u in self.__adjmx__[w]:
                        if u != vert and u != v:
                            for z in self.__adjmx__[u]:
                                if z == vert:
                                    mostra_C4([vert, v, w, u])
                                    return True
        return False


#%%
from time import time
import numpy

V = [2**v for v in range(3, 7)]
# E = [float(e) for e in numpy.arange(.2, 1.0, .05)]
E = [float(e) for e in numpy.arange(.0, .1, .01)]
T = [[None for _ in range(len(E))] for _ in range(len(V))]
for i in range(len(V)):
    for j in range(len(E)):
        g = Grafo(V[i])
        g.conecta(E[j])
        tempo = time()
        g.contemC4()
        tempo = time() - tempo

        T[i][j] = tempo * 1000. * 1000.

#%%
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np


fig = plt.figure(figsize=(10, 10))
ax = fig.gca(projection='3d')

X, Y = np.meshgrid(E, V)
Z = np.array(T)

# Plot the surface.
surf = ax.plot_surface(X, Y, Z, cmap=cm.get_cmap('coolwarm'), linewidth=0, antialiased=False)

# Customize the z axis.
# ax.set_zlim(-1.01, 1.01)
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
