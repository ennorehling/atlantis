# Using hex tiles in Atlantis

Atlantis 1.0 uses square tiles for its map, and each tile has a neighbour in four directions, NORTH, SOUTH, WEST, EAST. Hex tiles are another way to draw maps, and they are more versatile than squares, because they offer six directions, and travel in a diagonal direction is less lossy than on a square grid. On a square, traveling from tile (0,0) to tile (2,2) will cost the unit 4 movement, 2 on the x-axis, 2 on the y-axis, no matter how you move, even if the distance as a bird flies is only sqrt(8) = 2.8; hex tiles are far less wasteful than that.

## theory of hex tiles

### A third axis of travel

If N/S is one axis of travel and W/E is the second, then hex tiles add a third axis, which we'll name Y/M in reference to Hugh Walker's world of Magira, where the additional directions are called Yd and Mir.

On a rectangular grid, N/S and W/E align nicely with the X and Y axis of a two-dimensional array. In Atlantis, the square to the east of (x,y) is (x+1,y), and the square to the north is (x, y-1). Or, to write it out ass functions:

    W(x, y) = (x-1, y)
    E(x, y) = (x-1, y)
    N(x, y) = (x, y-1)
    S(x, y) = (x, y+1)

An interesting property of these functions is that you can do simple math with them. As an example, the tile to the east is a tile that we can reach by going south, then east, then north, or written as a formula: E(x, y) = N(E(S(x,y))).

As we will see, hex grids are not this simple.

### Orientation

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

## refactoring the code

In the original code base, the translations for coordinates are done all over the code. Our first step is to preserve the original behaviour, but to move all translations along a direction to a separate function. then in a second step, we will introduce the two new directions.

### split the code

So far, all of the code has been in one file, atlantis1.c, and because that file includes a main() function already, it's not possible to link this code with another executable for testing. We are splitting the main function off into its own file, and creating atlantis.h and atlantis.c which contain all functionality that is not in main().

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/60e87d35032e5ad393caef95157e6a56280275de)

### unit testing

We can now introduce our new function, and we will use TDD to write it, meaning we write a test first, and then change the code until the test passes. the boilerplate code for atlantis.test.c is very much like the code we wrote in rtl.test.c, and I'm stealing from it to get the general framework. Then I'm adding a single test that should pass if transform() is working as intended. This isn't the case, because transform currently only handles one error case and has none of the math.

In the changes on  [github](https://github.com/badgerman/atlantis1/commit/dd61a0ba672dbd62dc5ac2e70d4f56e7aab93732), you will also see the new keywords.h file, which contains the enum for keywords slit out. That's because we need the constants in there to write our tests, but it would be a shame to clutter atlantis.h with them. our main.c includes atlantis.h, but it has no use for these constants, and by having two header files, we only include what we actually need in main.c.

### fleshing out the transform function

The transform function takes three arguments. The first two are pointers to coordinates, which we are usng for input and output (we're reading the (x,y) tuple from there, and returning them through the caller by overwriting them). The return value of the function is only used to communicate errors (you cannot easily return a tuple in C). We never want these poitners to be NULL, so we verify that first thing, with an assertion:

    assert(x || !"invalid reference to X coordinate");
    assert(y || !"invalid reference to Y coordinate");

### A Few Words on Error Handling

Notice that we don't return an error code here, but we abort the program. That's because passing a null-pointer to this function is a programming error, not an error that might be caused by bad user data. Contrast that with the way we handle an invalid keyword: If the keyword does not match any of the cardinal directions, we return EINVAL (Invalid argument), because we don't want to put the burden of checking correct directions on the caller. Contrast this code:

    int x = r->x, y = r->y;
    int k = getkeyword(buf);
    if (transform(&x, &y, k)==0) {
        moveunit(u, x, y);
    }

with this:

    int x = r->x, y = r->y;
    int k = getkeyword(buf);
    if (k==K_NORTH || k==K_SOUTH || k==K_EAST || k==K_WEST) {
        transform(&x, &y, k);
        moveunit(u, x, y);
    }

The first example is easier because the transform function does error checking on the user's input for us. It's also easier to make changes to it, because adding another two directions only affects the transform() function, not everybody who is calling it.

You can see the full implementation on transform on [github](https://github.com/badgerman/atlantis1/commit/ee8a3fc067d3503d391614f726828b106689389c). It is passing the tests that we wrote earlier, so from here on, we just need to replace all hand-crafted coordinate transformations with our new code.

### Working with Legacy Code
