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
game = sys.argv[1] or '1'
game = 'game-' . game
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

cur.execute("SELECT filename FROM orders ORDER BY date DESC", (sender, filename))
con.close()
sys.exit(0)

