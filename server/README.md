`ioh-server` is an executable that serves objective function calls for many
problems available in IOHexperimenter.
It holds an `Analyzer` logger so that runs performed by a client solver can be
analyzed in IOHanalyzer later on.
It use a simple protocol where messages between the client and the server are
encoded in JSON.

## Build and run

### Build

The `ioh-server` is build along with IOHexperimenter if you enabled
the `BUILD_SERVER` option.

For example with:

```sh
cmake -DBUILD_SERVER=ON ..
```

### Run

The server expects you to indicates the I/O named pipes FIFO.
Under Linux, you would create them with:

```sh
mkfifo query reply
```

You can then run the server with the default configuration as:

```sh
./server/ioh-server query reply
```

To set up the problem actually served, use the following options:
- `-t`, `--type arg`:  Problem type (`integer` or `real`),
- `-p`, `--problem arg`:  Problem name (default: OneMax),
- `-i`, `--instance arg`:  Instance identifier (default: 0),
- `-d`, `--dimension arg`:  Problem dimension (default: 1).

For example:

```sh
ioh-server -t real -p Rosenbrock -i 1 -d 10 query reply
```

You can list the available problems using the `help-problems` option, but you
need to indicate the type of problems ("integer" or "real"):

```sh
ioh-server -t real --help-problems
```

More options are available, see `ioh-server --help`.


## Messages

### Definition

Messages can be either queries, from the (solver) client to the (problem)
server; or replies, from the problem (server) to the solver (client).

A message is a JSON object always holding a `query_type` (string) field.

| `query_type`:| Possible replies:  | Description:
|--------------|--------------------|-------------------------------------------------------------------------------------|
| `call`       | `value` or `error` | The solver send a solution and expect its value.
| `new_run`    | `ack` or `error`   | The solver ask for reseting the logger state.
| `stop`       | `ack` or `error`   | The solver ask for the server to stop (probably not enabled on production servers).


### Calls and values

A `call` query holds the solution to be evaluated by the objective function, in
the form of an array of `number`.

Example of a `call` query message:

```json
{
    "query_type":"call",
    "solution": [10,10]
}
```

which would be answered by a reply similar to:

```json
{
    "reply_type": "value",
    "value": 21
}
```

Optionaly, the `value` reply can send back the solution.

### Errors

An `error` reply should always provide a description in the `message` (string) field.

Optionally, it can give a `code` (integer), as a unique identifier of the error
type.

### Metadata fields

All messages have the following optional fields:
- `id` (integer): a unique identifier of the query (in a reply, refers to the related query),
- `timestamp` (date-time): date and time of the message,
- `remarks` (string): generic comment (e.g. software version).

## Service

The communication between the client and the server goes through named pipe
FIFO.
There is one named pipe for queries (in which the client writes and the server
reads) and one for replies (in which the server writes and the client reads).

### Client implementation

Implementation is simple as it just consists in writing (resp. reading) a query
(resp. reply) file, just as you would do with regular files.

The simplest client, making a single query, can be written in Python as:

```python
import json

# Forge a JSON `call` query:
squery = json.dumps( {"query_type":"call", "solution":[0,1,1,0,1]} )

# Send it to the server:
with open("query",'w') as fd:
    fd.write(squery)

# Wait for the answer and read it when it's available:
with open("reply",'r') as fd:
    sreply = fd.read()

# Decode the JSON:
jreply = json.loads(sreply)

# Extract the objective function value:
value = jreply["solution"]
```

An example implementation of a solver client is given in Python in
`example_client.py`.

### Fundamentals

Named pipes are special files with blocking input/output, which means that a
process reading such a file will be suspended until there is something to read.
This allows for very simple I/O code, avoiding polling and complex network
management: all you have to do to implement a client is to write the query in
the query file, then read the reply in the reply file.
The execution will advance only when the reply have been written in the file.

The theoretical principle can be represented by this UML sequence diagram:

```
            Named pipes
         ┌───────┴───────┐
┌──────┐ ┌─────┐   ┌─────┐ ┌──────┐
│Client│ │reply│   │query│ │Server│
└───┬──┘ └─┬───┘   └─┬───┘ └───┬──┘
    │      │         │         │
    │      │         │  ┌──────╢
    │      │    block│  │ wait ║
    │query │         │  └─────→║
    ├───────────────→│         │
    ╟─────┐│         ├────────→│
    ║wait ││block    │         ║process
    ║←────┘│         │         ║
    │      │←──────────────────┤
    │←─────┤         │    reply│
    │      │         │         │
    ┊      ┊         ┊         ┊
```

Note that the service should be started first, waiting for the input.

## Going further

### Validate messages

The formal description of a queries is available in
`ioh-query.schema.yaml`, the one for replies in `ioh-reply.schema.yaml`.

The description follows the [JSON Scehma format (draft
07)](http://json-schema.org/draft-07/schema), but is presented in the
(strictly equivalent) YAML format, easier to read for humans and allowing for
comments.

One can validate any message against those schema, using the [Singularity](https://sylabs.io/singularity/)
container defined by `message_validator.sindef`.
To build this container,
install SingularityCE and run `singularity build message_validator.sif
message_validator.sindef`.
To validate a message simply run: `singularity run message_validator.sif
<schema.yaml> <message.json>`.

### Network gateway

If you want to expose such a service as a network server, just use socat.

For example, to get _data_ query from the network for `service1`:

```sh
socat -v -u TCP-LISTEN:8423,reuseaddr PIPE:./query
```

You can test it by sending something on the connection:

```sh
echo "Hello World!" > /dev/tcp/127.0.0.1/8423
```

Conversely, to send automatically back the answer to some server:

```sh
socat -v -u PIPE:./reply TCP2:8424:host
```

Be aware that `socat` will terminate as soon as it receives the end of the message.
Thus, if you want to establish a permanent gate, you will have to use the `fork`
option:

```sh
socat TCP-LISTEN:8478,reuseaddr,fork PIPE:/./query
```
