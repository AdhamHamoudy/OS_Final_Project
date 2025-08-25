Stage 6 — Euler Server
Files: euler_server.cpp, server_protocol.hpp
Build reuses Stage1 (graph) + Stage2 (euler).


Example:

Client: EULER RAND 6 8 42
Server: OK CIRCUIT 8
        0 3 2 1 5 4 0
        END