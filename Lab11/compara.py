#! /usr/bin/env python3

import sys

dirx = [1, -1, 0,  0]
diry = [0,  0, 1, -1]

arq_name1 = str( sys.argv[ 2 ] )
arq_name2 = str( sys.argv[ 1 ] )

arq1 = open(arq_name1, 'r')

lines = arq1.readlines()

h = int( lines[0].split(' ')[0] )
dx = int( lines[0].split(' ')[1] )
dy = int( lines[0].split(' ')[2] )

ox = int( lines[1].split(' ')[0] )
oy = int( lines[1].split(' ')[1] )

nrow = int( lines[1].split(' ')[2] )
ncol = int( lines[1].split(' ')[3] )

mapa =  [ [ int( lines[2+i].split(' ')[x] ) for x in range(0, ncol) ] for i in range(0, nrow) ]

arq2 = open(arq_name2, 'r')

lines = arq2.readlines()

for l in lines:
	c = 0
	for i in l.split(' '):
		c += 1
	if( c != ncol ):
		print('intância invalida!')

res = [ [ lines[i].split(' ')[x].strip() for x in range(0, ncol) ] for i in range(0, nrow) ]

visit = [ [ (False if mapa[i][x] <= h else True) for x in range(0, ncol) ] for i in range(0, nrow) ]


x, y = dx, dy
passo = 0

invalid = False
for i in range(0, nrow):
	for j in range(0, ncol):
		if( visit[i][j] and res[i][j] != '#'):
			print("O drone não pode sobrevoar na posição ("+str(j)+","+str(i)+")" )
			invalid = True
if( invalid ):
	sys.exit(1)

for i in range(0, nrow):
	for j in range(0, ncol):
		invalid = True
		if( res[i][j] == '#' ):
			continue
		for k in range(0, 4):
			if( j+dirx[k] >= 0 and j+dirx[k] < ncol and i+diry[k] >= 0 and i+diry[k] < nrow and res[i+diry[k]][j+dirx[k]] != '#'):
				invalid = False
		if( invalid ):
			print("movimento da posição ("+str(i)+","+str(j)+") está isolado!")
			sys.exit(1)

while( not( y==oy and x==ox) ):
	if( res[y][x] == str(passo) ):
		visit[y][x] = True
		for i in range(0, 4):
			if( x+dirx[i] >= 0 and x+dirx[i] < ncol and y+diry[i] >= 0 and y+diry[i] < nrow and
				(not visit[y+diry[i]][x+dirx[i]]) and res[y+diry[i]][x+dirx[i]] != '#' and res[y+diry[i]][x+dirx[i]] == str(passo+1) ):
				x, y = x+dirx[i], y+diry[i]
				break
		passo += 1
	else:
		print("caminho errado!")
		sys.exit(1)

sys.exit(0)