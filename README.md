##Introduction

Node-Erlang-Bridge

A nodejs node (cnode) communicate with Erlang node. 
The nodejs node is only implemented as a cnode client. 
Only show the way how to send and receive string message.

## Configuration and run

###Start the erlang node service:

    erl -sname e1 -setcookie secret
    c(enode).
    enode:start().

###Build nodejs addon and run the nodejs node client:

    node-waf distclean configure build
    node nnode.js

TODO: Map Erlang Eterm to json and reverse.
