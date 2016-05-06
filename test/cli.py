__author__ = "Yihua Eric Fang (yihuaf)"

import httplib
import sys
import lib

def Usage():
   print "Supported Command:"
   print "Subscribe <topic> <username>"
   print "Unsubscribe <topic> <username>"
   print "Publish <topic> <content>"
   print "GetNext <topic> <username>"
   print "Exit: To exit..."
   print "Help: Print this Message."

def StartCLI():
   port = sys.argv[1]
   conn = httplib.HTTPConnection("localhost", port)
   while True:
      line = sys.stdin.readline()
      # Use space as delimiter.
      split_line = line.split()
      op = split_line[0]
      err = False
      if op == "Exit":
         break;
      elif op == "Publish":
         # Publish <topic> <content>
         # Need to parse out Publish and <topic> out first.
         if len(split_line) < 2:
            err = True
         else:
            topic = split_line[1]
            # The content may contain space, so we should rejoin the splited
            # string.
            content = " ".join(split_line[2::])
            # Use function map to avoid excessive if else blocks.
            lib.func_map[op](conn, topic, content)
      elif op == "Subscribe" or op == "Unsubscribe" or op == "GetNext":
         # <OP> <topic> <username>
         # Three items required.
         if len(split_line) < 3:
            err = True
         else:
            topic = split_line[1]
            username = split_line[2]
            # Use function map to avoid excessive if else blocks.
            lib.func_map[op](conn, topic, username)
      else:
         Usage()

   return

if __name__ == "__main__":
    StartCLI()
