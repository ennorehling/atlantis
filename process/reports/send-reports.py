#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

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

def send_report(smtp, path, filename, body, email, subject):
    msg = MIMEMultipart()

    fp = open(os.path.join(path, body), 'r')
    hello = MIMEText(fp.read(), 'plain')
    msg.attach(hello)
    
    fp = open(os.path.join(path, 'reports', filename), 'r')
    report = MIMEText(fp.read(), 'plain')
    fp.close()
    report.add_header('Content-Disposition', 'attachment', filename=filename+'.txt')
    msg.attach(report)
    
    me = 'atlantis@twigtechnology.com'
    name = "Atlantis Server"
    msg['From'] = '"%s" <%s>' % (name, me)
    msg['To'] = email
    msg['Subject'] = subject
    msg.preamble="atlantis report mail"
    msg.epilogue="enno rehling 2013"
    smtp.sendmail(me, [email], msg.as_string())

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
        filename = '%s-%s.r' % (turn, faction)
        path = os.path.join('game-'+game)
        subject = 'Atlantis Report %d, Game %s, Faction %s' % (turn, game, faction)
        send_report(smtp, path, filename, 'mailbody.txt', email, subject)
        logger.info("sent report %d to %s" % (turn, email))
smtp.quit()
