#!/usr/bin/python3

import sys
import math
import random
import time
import itertools

def readEdges(fn):
  E = []
  P = {}
  with open(fn) as f:
    for line in f:
      if line:
        E.append(tuple(int(x) for x in line.split()))
      for v in E[-1]:
        P[v] = (v // 100000, v % 100000)
  return E,P

def buildGraph(E):
  G = {}
  for u,v in E:
    if u in G:
      G[u].add(v)
    else:
      G[u] = {v}
    if v in G:
      G[v].add(u)
    else:
      G[v] = {u}

  return G

def readInd(fn):
  with open(fn) as f:
    I = {int(line) for line in f if line}
  return I

def testInd(G,E,I):
  err = set()

  for v in I:
    if v not in G:
      print("Vertex does not exist:", v)
      err.add(v)

  for u,v in E:
    if v in I and u in I:
      print("Adjacent vertices:", u, v)
      err.add(u)
      err.add(v)

  return err


def readPoints(fn):
  P = []
  with open(fn) as f:
    for line in f:
      if line:
        P.append(tuple(float(x) for x in line.split()))
  return P

def writeSVG(fn, P, E, I, err):
  imgsz = 1000.0
  x0 = min([p[0] for p in P.values()])
  y0 = min([p[1] for p in P.values()])
  x1 = max([p[0] for p in P.values()])
  y1 = max([p[1] for p in P.values()])
  sz = max(x1-x0, y1-y0)
  r2 = 10.0

  with open(fn, "w") as fsvg:
    fsvg.write('<?xml version="1.0" encoding="utf-8"?>\n')
    fsvg.write(f'<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="{imgsz+2*r2}" height="{imgsz+2*r2}">\n')
    
    fsvg.write(f' <text x="0" y="20">|I|={len(I)}</text>\n')

    for e in E:
      q1 = (r2 + (P[e[0]][1]-y0) * imgsz / sz, r2 + (x1-P[e[0]][0]) * imgsz / sz)
      q2 = (r2 + (P[e[1]][1]-y0) * imgsz / sz, r2 + (x1-P[e[1]][0]) * imgsz / sz)
      if e[0] in err and e[1] in err:
        fsvg.write(f' <polygon stroke="red" stroke-width="2" fill="none" points="{q1[0]},{q1[1]} {q2[0]},{q2[1]}"/>\n')
      else:
        fsvg.write(f' <polygon stroke="black" stroke-width="1" fill="none" points="{q1[0]},{q1[1]} {q2[0]},{q2[1]}"/>\n')

    for i,p in P.items():
      q = (r2 + (p[1]-y0) * imgsz / sz, r2 + (x1-p[0]) * imgsz / sz)
      if i in err:
        fsvg.write(f' <circle stroke="black" fill="red" stroke-width="2" cx="{q[0]}" cy="{q[1]}" r="6">\n')
        fsvg.write(f'  <title>{i} !</title>\n')
        fsvg.write(' </circle>\n')
      elif i in I:
        fsvg.write(f' <circle stroke="darkgreen" fill="lightgreen" stroke-width="2" cx="{q[0]}" cy="{q[1]}" r="5">\n')
        fsvg.write(f'  <title>{i} I</title>\n')
        fsvg.write(' </circle>\n')
      else:
        fsvg.write(f' <circle stroke="darkblue" stroke-width="2" cx="{q[0]}" cy="{q[1]}" r="3">\n')
        fsvg.write(f'  <title>{i}</title>\n')
        fsvg.write(' </circle>\n')
    fsvg.write("</svg>\n")

def main():
  if len(sys.argv) != 3:
    print("./testind.py input.edges output.ind")
    exit(1)
  E,P = readEdges(sys.argv[1])
  G = buildGraph(E)
  I = readInd(sys.argv[2])
  print("Size:", len(I))

  err = testInd(G,E,I)
  if err:
    print("There are", len(err), "errors")
  else:
    print("Solution is correct")

  fn = sys.argv[2]+".svg"
  writeSVG(fn, P, E, I, err)
  print("Solution saved to",fn)

  exit(len(err))

if __name__ == '__main__':
  main()
