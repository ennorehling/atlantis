# Stacking and Sorting units

## Sorting

In regular Atlantis 1.0, the order of execution depends on the ordering of units in a region. When you have a list of units in your report, they will process the same command from the top of the list down. Even though there is seldom a benefit from being first to execute an order, this can be interesting for ENTER or GIVE commands, and some other less obvious ones.

It gets a little complicated when units are in a building or ship. All buildings and units inside them are listed at the top of the region, and ships with their crew below them, no matter where the units are in the order of execution. For example, if a unit from the end of your report enters a ship, it gets listed above all other units in the region, even though it will still execute commands last.

That is pretty confusing, so I decided to change my game to always sort units internally the same way they are sorted in the report, i.e. if a unit enters a ship or building, they are moved upwards in the chain of units for that region.

## Stacking

I mentioned earlier that I wanted to implement the stacking mechanism for units that Olympia has. Without implementing any rules for it yet, I've added the mechanism to my code, and a configuration switch to enable STACK and UNSTACK commands. There is only one level of stacking: Each stack has one leader and a list of units stacked with him. If A stacks B, then all units previously stacked with A will automatically stack with B. Stacked units follow each other in execution order (see above).

My next goal is to display this in the report - it is easy in JSON, but I haven't decided on how to display it in the human-readable report, and without better tool support, I can't really deprecate that report yet.

My other next step is to split all orders and separate the ones that can only be done by the leader of a stack (MOVE, ENTER, LEAVE). I realize that I'm a little short on the vision for my next game, and I have to write down what the rules are going to be for recruiting, skills, and combat first. Implementing mechanics sure is easier than coming up with rules :-)

Enno.
