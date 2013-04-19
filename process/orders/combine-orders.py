#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

import os
import re
import os.path
import time
import rfc822
import logging
import sys
import sqlite3

LOG_FILENAME='orders.log'
logging.basicConfig(filename=LOG_FILENAME, level=logging.INFO)
logger = logging

turn=None
try:
    fo = open('turn', 'r')
    line = fo.readline()
    turn = int(line)
    fo.close()
except IOError:
    logger.warning("could not open 'turn' file, processing all orders")

game = sys.argv[1] or '1'
game = 'game-' + game
if not os.path.exists(game):
    logger.error('invalid game %s' % game)
    sys.exit(-1)
logger.info('using database %s/atlantis.db' % game)
con = sqlite3.connect('%s/atlantis.db' % game)
cur = con.cursor()
path = os.path.join(game, 'orders.dir')
if not os.path.exists(path):
    logger.error('no orders for game %s' % game)
    sys.exit(-2)

if turn is None:
    cur.execute("SELECT email, filename FROM orders ORDER BY date ASC")
else:
    cur.execute("SELECT email, filename FROM orders ORDER BY date ASC WHERE turn=?" % (turn, ))

for row in cur.fetchall():
    email, filename = row
    filename = os.path.join(path, filename)
    try:
        fin = open(filename, 'r')
        print('#ORDERS %s\n#ORDERS %s' % (email, filename))
        for line in fin.readlines():
            line = line.strip()
            if len(line)>1:
                print(line)
    except IOError:
        logger.error("I/O error: %s" % (filename,))

con.close()
sys.exit(0)

