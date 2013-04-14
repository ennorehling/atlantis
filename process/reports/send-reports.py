#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

from email.mime.text import MIMEText

import email.utils
import re
import os
import os.path
import logging
import sys
import smtplib

LOG_FILENAME='orders.log'

# the main body of the script:
logging.basicConfig(filename=LOG_FILENAME, level=logging.INFO)
logger = logging

def send_report(smtp, path, email, subject):
    fp = open(path, 'r')
    msg = MIMEText(fp.read())
    fp.close()
    
    me = 'atlantis@twigtechnology.com'
    name = "Atlantis Server"
    msg['From'] = '"%s" <%s>' % (name, me)
    msg['To'] = email
    msg['Subject'] = subject
    smtp.sendmail(me, [email], msg.as_string())

def read_turn():
    try:
        fo = fopen('turn', 'r')
        line = fo.readline()
        return int(line)
    except:
        logger.error("could not open 'turn' file, assuming turn=%d" % (turn, ))
    return 0

smtp = smtplib.SMTP('localhost')
game = sys.argv[1]
path = os.path.join('game-'+game, 'send')
fp = open(path, 'r')
re_path = re.compile('mail (\d+)-(\d+)\.r')
re_email = re.compile('in%"(.*)"')
for line in fp.readlines():
    m = re_path.match(line)
    if m is not None:
        turn = int(m.group(1))
        faction = m.group(2)
        continue
    m = re_email.match(line)
    if m is not None:
        email = m.group(1)
        print turn, faction, email
        path = os.path.join('game-'+game, 'reports', '%s-%s.r' % (turn, faction))
        subject = 'Atlantis Report %d, Game %s, Faction %s' % (turn, game, faction)
        send_report(smtp, path, email, subject)
        logger.info("sent report %d to %s" % (turn, email))
smtp.quit()
