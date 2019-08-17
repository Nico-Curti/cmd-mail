#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import yaml
import getpass
import smtplib
import argparse
import subprocess
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.application import MIMEApplication

__package__ = 'email sender'
__author__  = ['Nico Curti']
__email__   = ['nico.curti2@unibo.it']


def parse_args ():

  description = '*-* Send Mail *-*'
  parser = argparse.ArgumentParser(description = description)

  parser.add_argument('-t', required=True, type=str, dest='TO', action='store', help='mail.config key TO send e-mail')
  parser.add_argument('-f', required=True, type=str, dest='FROM', action='store', help='mail.config key FROM send e-mail')
  parser.add_argument('-x', required=True, type=str, dest='cmd', action='store', help='command line to run')

  args = parser.parse_args()

  return args




if __name__ == '__main__':


  args = parse_args()

  with open('mail.config', 'r') as fp:
    config = yaml.load(fp)

  FROM = config[args.FROM]
  TO = config[args.TO]
  cmd = args.cmd

  pswd = getpass.getpass('Password {}: '.format(FROM['address']))

  msg = MIMEMultipart()
  msg['From'] = FROM['username']
  msg['To'] = TO['username']
  msg['Subject'] = 'Results run {}'.format(cmd.split(' ')[0])

  proc = subprocess.Popen([cmd], stdout=subprocess.PIPE, shell=True)
  (out, err) = proc.communicate()

  with open('log.txt', 'wb') as f:
    f.write(out)

  if err == None:
    msg.attach(MIMEText('Success! Script exit with 0. For the output stream see the logfile.', 'plain'))

  else:
    msg.attach(MIMEText('Somethig goes wrong. Please see the logfile for more informations.'))

  with open('log.txt', 'rb') as f:
    part = MIMEApplication(f.read(), Name='log.txt')

  # After the file is closed
  part['Content-Disposition'] = 'attachment; filename="log.txt"'
  msg.attach(part)

  print('Send e-mail... ', end='', flush=True)

  try:

    server = smtplib.SMTP(FROM['smtp'], 25)
    server.connect(FROM['smtp'], int(FROM['port']))
    server.ehlo()
    server.starttls()
    server.ehlo()
    server.login(FROM['address'], pswd)

    text = msg.as_string()
    server.sendmail(FROM['address'], TO['address'], text)
    server.quit()
    print('[DONE]')

  except:
    print('[FAILED]')

