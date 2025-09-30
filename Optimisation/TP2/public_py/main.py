#!/usr/bin/pypy3
import sys
import json
import math

def readInstance(fn):
  """Read a json instance and return the point array and the radius."""
  with open(fn) as f:
    data = json.load(f)

  points = [(p['x'],p['y']) for p in data['points']]
  radius = data['radius']
  return points,radius


def buildGrid(points, gridsize):
  """Insert the points in a grid, returning the grid dict and a getCell function."""
  def getCell(p):
    """Take a point p and return the cell containing p"""
    return (int(p[0]/gridsize), int(p[1]/gridsize))

  grid = {}
  for p in points:
    cell = getCell(p)
    grid.setdefault(cell, []).append(p)
  return grid,getCell


def distsq(p,q):
  """Return the squared Euclidean distance between two points."""
  return sum((p[i]-q[i])**2 for i in range(len(p)))


def getNeighbors(grid, getCell, radius, p):
  """Generates the points within distance 2*radius of p"""
  pcell = getCell(p)
  diamsq = 4*radius*radius
  for dx in (-1,0,1):
    for dy in (-1,0,1):
      cell = (pcell[0]+dx, pcell[1]+dy)
      for q in grid.get(cell, []):
        if p != q and distsq(p,q) <= diamsq:
          yield q


def greedy(points, radius, direction, grid, getCell):
  """Greedly create a maximal independent set from left to right."""

  points = sorted(points, key = lambda p : direction[0]*p[0] + direction[1]*p[1])
  sol = set()
  forbidden = set()
  for p in points:
    if p not in forbidden:
      sol.add(p)
      for q in getNeighbors(grid, getCell, radius, p):
        forbidden.add(q)

  return sol


def writeSolutionSVG(fn, points, sol, input_radius, image_size=1000):
  """Draw disks around points, with the ones in sol colored blue"""
  x0 = min(p[0] for p in points) - input_radius
  y0 = min(p[1] for p in points) - input_radius
  x1 = max(p[0] for p in points) + input_radius
  y1 = max(p[1] for p in points) + input_radius
  input_size = max(x1-x0, y1-y0)
  image_radius = image_size * input_radius / input_size

  points = sorted(points, key = lambda p : p in sol) # solution points in the end for improved visibility

  def input_to_image_point(p):
    x = (p[0] - x0) * image_size / input_size
    y = (y1 - p[1]) * image_size / input_size
    return x,y

  with open(fn, "w") as fsvg:
    fsvg.write('<?xml version="1.0" encoding="utf-8"?>\n')
    image_size_x,image_size_y = input_to_image_point((x1,y0))
    fsvg.write(f'<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="{image_size_x}" height="{image_size_y}">\n')

    for input_p in points:
      image_p = input_to_image_point(input_p)
      color = 'blue' if input_p in sol else 'black'

      fsvg.write(f' <circle stroke="{color}" fill="none" stroke-width="2" cx="{image_p[0]}" cy="{image_p[1]}" r="{image_radius}">\n')
      fsvg.write(f'  <title>{input_p}</title>\n')
      fsvg.write( ' </circle>\n')

    fsvg.write("</svg>\n")

def manyRuns(points, radius, angles=8):
  """Runs greedy angles times with different directions
     Returns the best solution found"""
  bestSol = []
  gg = buildGrid(points, 2*radius)

  print(f'Found {angles} independent sets of size:', end='', flush=True)
  for i in range(angles):
    angle = i * 2 * math.pi / angles
    direction = (math.cos(angle), math.sin(angle))
    sol = greedy(points, radius, direction, *gg)
    print('', len(sol), end='', flush=True)
    if len(sol) > len(bestSol):
      print('*', end='', flush=True)
      bestSol = sol

  print('\nBest:', len(bestSol))
  return bestSol


def main():
  if len(sys.argv) != 3:
    print(f'{sys.argv[0]} instance.json output.svg', file=sys.stderr)
    exit(1)
  points,radius = readInstance(sys.argv[1])
  print(f'Read {len(points)} points with radius {radius}.')

  sol = manyRuns(points, radius)

  writeSolutionSVG(sys.argv[2], points, sol, radius)


if __name__ == '__main__':
  main()
