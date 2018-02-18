#!/usr/bin/python

import yaml
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import getpass
import sys
import argparse
import subprocess

if __name__ == "__main__":
    description = "*-* Send Mail *-*"
    parser = argparse.ArgumentParser(description = description)
    parser.add_argument("-t", required=True, dest="TO", action="store", help="mail.config key TO send e-mail")
    parser.add_argument("-f", required=True, dest="FROM", action="store", help="mail.config key FROM send e-mail")
    parser.add_argument("-x", required=True, dest="cmd", action="store", help="command line to run")

    if len(sys.argv) <= 1:
        parser.print_help()
        sys.exit(1)
    else:
        args = parser.parse_args()
        
    config = yaml.load(open("mail.config", "r"))
    FROM = config[args.FROM]
    TO = config[args.TO]
    cmd = args.cmd
    
    pswd = getpass.getpass("Password %s: " %(FROM["address"]))
    
    msg = MIMEMultipart()
    msg["From"] = FROM["address"]
    msg["To"] = TO["address"]
    msg["Subject"] = "Results run %s"%(cmd.split(" ")[0])


    proc = subprocess.Popen([cmd], stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    body = "".join(map(chr, out))
    if err == None:
        body += "\nSuccess! Script exit with 0. For the output stream see the logfile."
    else:
        body += "\nSomethig goes wrong. Please see the logfile for more informations."
        
    msg.attach(MIMEText(body, "plain"))
    
    print("Send e-mail... ", end="")
    try:
        server = smtplib.SMTP(FROM["smtp"], 25)
        server.connect(FROM["smtp"], int(FROM["port"]))
        server.ehlo()
        server.starttls()
        server.ehlo()
        server.login(FROM["address"], pswd)
        
        text = msg.as_string()
        server.sendmail(FROM["address"], TO["address"], text)
        server.quit()
        print("[DONE]")
    except:
        print("[FAILED]")
    
