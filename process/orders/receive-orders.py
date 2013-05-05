#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

from email.Parser import Parser
import email.utils
import os
import re
import os.path
import time
import rfc822
import logging
import sys
import sqlite3
import uuid

LOG_FILENAME='orders.log'

def write_part(outfile, part):
    charset = part.get_content_charset()
    payload = part.get_payload(decode=True)

    if charset is None:
        charset = "latin1"
    try:
        msg = payload.decode(charset, "ignore")
    except:
        msg = payload
        charset = None
    try:
        utf8 = msg.encode("utf-8", "ignore")
        outfile.write(utf8)
    except:
        outfile.write(msg)
        return False
    outfile.write("\n");
    return True

def copy_orders(message, filename, writeheaders = True):
    dirname, basename = os.path.split(filename)
    if not os.path.exists(dirname): os.mkdir(dirname)
    if writeheaders:
        dirname = os.path.join(dirname, 'headers')
        if not os.path.exists(dirname): os.mkdir(dirname)
        outfile = open(os.path.join(dirname, basename), "w")
        for name, value in message.items():
            outfile.write(name + ": " + value + "\n")
        outfile.close()

    found = False
    outfile = open(filename, "w")
    if message.is_multipart():
        for part in message.get_payload():
            content_type = part.get_content_type()
            logger.debug("found content type %s for %s" % (content_type, basename))
            if content_type=="text/plain":
                logger.info('writing orders to %s' % filename)
                if write_part(outfile, part):
                    found = True
                else:
                    charset = part.get_content_charset()
                    logger.error("could not write text/plain part (charset=%s) for %s" % (charset, basename))

    else:
        if write_part(outfile, message):
            found = True
        else:
            charset = message.get_content_charset()
            logger.error("could not write text/plain message (charset=%s) for %s" % (charset, basename))
    outfile.close()
    if not found:
        os.unlink(filename)
        return False
    return True

def accept(message, filename, sender):
    maildate = message.get("Date")
    turndate = None
    if maildate is not None:
        turndate =rfc822.parsedate_tz(maildate)
        if turndate is not None:
            turndate = rfc822.mktime_tz(turndate)
            logger.debug("mail date is '%s' (%d)" % (maildate, turndate))
    else:
        logger.warning("missing message date " + sender)
        pass

    if not copy_orders(message, filename):
        logger.warning("rejected - no text/plain in orders from " + sender)
        os.unlink(filename)
        pass
    
    return turndate

def valid_email(addr):
    rfc822_specials = '/()<>@,;:\\"[]'
    # First we validate the name portion (name@domain)
    c = 0
    while c < len(addr):
        if addr[c] == '"' and (not c or addr[c - 1] == '.' or addr[c - 1] == '"'):
            c = c + 1
            while c < len(addr):
                if addr[c] == '"': break
                if addr[c] == '\\' and addr[c + 1] == ' ':
                    c = c + 2
                    continue
                if ord(addr[c]) < 32 or ord(addr[c]) >= 127: return 0
                c = c + 1
            else: return 0
            if addr[c] == '@': break
            if addr[c] != '.': return 0
            c = c + 1
            continue
        if addr[c] == '@': break
        if ord(addr[c]) <= 32 or ord(addr[c]) >= 127: return 0
        if addr[c] in rfc822_specials: return 0
        c = c + 1
    if not c or addr[c - 1] == '.': return 0

    # Next we validate the domain portion (name@domain)
    domain = c = c + 1
    if domain >= len(addr): return 0
    count = 0
    while c < len(addr):
        if addr[c] == '.':
            if c == domain or addr[c - 1] == '.': return 0
            count = count + 1
        if ord(addr[c]) <= 32 or ord(addr[c]) >= 127: return 0
        if addr[c] in rfc822_specials: return 0
        c = c + 1
    return count >= 1

def get_sender(header):
    replyto = header.get("Reply-To")
    if replyto is None:
        replyto = header.get("From")
        if replyto is None: return None
    return parseaddr(replyto)[1]

def read_turn(path, default=0):
    try:
        fo = open(os.path.join(path, 'turn'), 'r')
    except:
        logger.error("could not open 'turn' file, assuming turn=%d" % (default, ))
        return default
    line = fo.readline()
    return int(line)

# the main body of the script:
logging.basicConfig(filename=LOG_FILENAME, level=logging.INFO)
logger = logging

infile = sys.stdin
if len(sys.argv)>1:
    logger.info('reading message from %s' % sys.argv[1])
    infile = open(sys.argv[1], "r")
else:
    logger.info('reading message from stdin')

message = Parser().parse(infile)
replyto = message.get('Reply-To') or message.get('From')
sender = None if (replyto is None) else email.utils.parseaddr(replyto)[1]
if sender is None or valid_email(sender)==0:
    logger.error("invalid email address: " + str(sender))
    sys.exit(-1)
else:
    logger = logging.getLogger(sender)
    logger.info("received orders from " + sender)
subject = message.get('Subject')
m = re.match(r'ATLA\S*\s*ORDER\S*\s*GAME\s*(\d+)', subject, re.IGNORECASE)
if m is None:
    logger.error("invalid subject line: " + str(subject))
    sys.exit(-2)
game = m.expand(r'game-\1')
if not os.path.exists(game):
    logger.error('invalid game %s' % game)
    sys.exit(-3)
turn = read_turn(game)
logger.info('using database %s/atlantis.db' % game)
con = sqlite3.connect('%s/atlantis.db' % game)
cur = con.cursor()
path = os.path.join(game, 'orders.dir')
if not os.path.exists(path):
    os.mkdir(path)

while sender is not None:
    try:
        filename = "%d-%s" % (turn, uuid.uuid4().hex)
        cur.execute("INSERT INTO orders (email, filename, turn) VALUES (?, ?, ?)", (sender, filename, turn))
        break
    except sqlite3.IntegrityError:
        print 'non-uniq UUID', filename

filename = os.path.join(path, filename)
date = accept(message, filename, sender)
if date is not None:
    os.utime(filename, (date, date))
    cur.execute("UPDATE orders SET date = ? WHERE filename = ?", (date, filename))
con.commit()
con.close()
if infile!=sys.stdin:
    infile.close()
sys.exit(0)

