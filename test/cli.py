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
   conn = httplib.HTTPConnection("localhost", 10000)
   while True:
      line = sys.stdin.readline()
      split_line = line.split()
      op = split_line[0]
      err = False
      if op == "Exit":
         break;
      elif op == "Publish":
         if len(split_line) < 2:
            err = True
         else:
            topic = split_line[1]
            content = " ".join(split_line[2::])
            func_map[op](conn, topic, content)
      elif op == "Subscribe" or op == "Unsubscribe" or op == "GetNext":
         if len(split_line) < 3:
            err = True
         else:
            topic = split_line[1]
            username = split_line[2]
            func_map[op](conn, topic, username)
      else:
         Usage()

   return

if __name__ == "__main__":
    StartCLI()
