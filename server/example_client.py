#!/usr/bin/env python3
import json
import iohservice

if __name__ == "__main__":
    import random
    import sys

    dimension = 10
    fquery = "../debug/query"
    freply = "../debug/reply"

    for i in range(5):

        if i == 3:
            jq = json.dumps( {"query_type":"new_run"} )
            iohservice.query(jq, fquery, freply)
        
        sol = [random.randint(0,1) for j in range(dimension)]

        val = iohservice.call(sol, fquery, freply)
        print("f({}) = {}".format(sol,val))

    # Nicely ask the server to stop.
    print("Send an EXIT query...", file=sys.stderr, end="")
    jq = json.dumps( {"query_type":"stop"}, indent=4)
    iohservice.query(jq, fquery, freply)

