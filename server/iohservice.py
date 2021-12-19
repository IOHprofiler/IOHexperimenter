import sys
import json

def send(jq, fquery):
    print("Send a query...", file=sys.stderr, end="")
    with open(fquery,'w') as fd:
        fd.write(jq)
    print("OK", file=sys.stderr)
    print(jq, file=sys.stderr)

def read(freply):
    print("Read the reply...", file=sys.stderr, end="")
    with open(freply,'r') as fd:
        ans = fd.read()
    print("OK",file=sys.stderr)
    r = json.loads(ans)
    print(json.dumps(r, indent=4))
    return r

def query(jq, fquery, freply):
    send(jq, fquery)
    return read(freply)

def is_error(jr):
    if "reply_type" not in jr:
        return True
    elif jr["reply_type"] == "error":
        return True
    else:
        return False

def call(sol, fquery, freply):
    jq = json.dumps( {"query_type":"call", "solution":sol}, indent=4 )
    reply = query(jq, fquery, freply)
    if not is_error(reply):
        return reply["value"]
    else:
        print("ERROR:",reply["message"])
        return None
