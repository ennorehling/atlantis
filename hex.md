# Using hex tiles in Atlantis

Atlantis 1.0 uses square tiles for its map, and each tile has a neighbour in four directions, NORTH, SOUTH, WEST, EAST. Hex tiles are another way to draw maps, and they are more versatile than squares, because they offer six directions, and travel in a diagonal direction is less lossy than on a square grid. On a square, traveling from tile (0,0) to tile (2,2) will cost the unit 4 movement, 2 on the x-axis, 2 on the y-axis, no matter how you move, even if the distance as a bird flies is only sqrt(8) = 2.8; hex tiles are far less wasteful than that.

## A third axis of travel

If N/Sis one axis of travel and W/E is the second, then hex tiles add a third axis, which we'll name Y/M in reference to Hugh Walker's world of Magira, where the additional directions are called Yd and Mir.

On a rectangular grid, N/S and W/E align nicely with the X and Y axis of a two-dimensional array. In Atlantis, the square to the east of (x,y) is (x+1,y), and the square to the north is (x, y-1). Or, to write it out ass functions:

    W(x, y) = (x-1, y)
    E(x, y) = (x-1, y)
    N(x, y) = (x, y-1)
    S(x, y) = (x, y+1)

An interesting property of these functions is that you can do simple math with them. As an example, the tile to the east is a tile that we can reach by going south, then east, then north, or written as a formula: E(x, y) = N(E(S(x,y))).

As we will see, hex grids are not this simple.

## Orientation

It's possible to map a hex grid to a two-dimensional array, too, but the mapping is not as easy. first of all, there are two main ways to orient the hexes:
     / \            _____
    /   \          /     \
    |   |         /       \
    |   |         \       /
    \   /          \_____/ = horizontal
     \ / = vertical

Each orientation has its own advantages: The vertical layout has a clearly defined West and East direction, the horizontal layout has obvious North and South edges. My own experience and those of players I've spoken are that East and West are easier to mix up than North and South, and while Eressea chose the horizontal layout, I'm going with the vertical one for Atlantis.

Note: For the sake of easier illustration, I am going to cut the pointy edges off the hexes in future illustrations.

To illustrate the map and the six directions, here's a tile (C) with all its neighbours:

       +-----+-----+
       |     |     |
       |  Y  |  N  |
       |     |     |
    +--+--+--+--+--+--+
    |     |     |     |
    |  W  |  C  |  E  |
    |     |     |     |
    +--+--+--+--+--+--+
       |     |     |
       |  S  |  M  |
       |     |     |
       +-----+-----+

For the mapping of this arrangement to a 2D grid, we can still map two of the three cardinal directions to an axis on the grid, and we choose the same ones as before. This makes calculating the coordinates of a neighbour in N/S or W/E directions easy, and the Y/M direction can be arrived at by using the same math trick that I showed off after introducing the rectangular grid mapping. The Yd tile is to the west of the northern tile, the Mir tile is east of the southern tile. To summarize, the functions for the neighbours of a tile are:

    W(x, y) = (x-1, y)
    E(x, y) = (x+1, y)
    N(x, y) = (x, y-1)
    S(x, y) = (x, y+1)
    M(x, y) = W(N(x,y)) = (x-1, y-1)
    Y(x, y) = E(S(x,y)) = (x+1, y+1)

## refactoring step

In the original code base, the translations for coordinates are done all over the code. Our first step is to preserve the original behaviour, but to move all translations along a direction to a separate function.
