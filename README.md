# CS425 – A1 to A4 (Ultra‑Explained Edition)

This repository contains four networking assignments rewritten with friendly, step‑by‑step explanations so that you can skim comments and understand the “why” as well as the “what”.

## A1 — Epoll‑based Group Chat (TCP)

- Folder: `CS425/A1`
- Files: `server_grp.cpp`, `client_grp.cpp`, `server_grp.h`, `Makefile`, `users.txt`
- What you’ll learn:
  - How a TCP server handles many clients using `epoll` (no threads required).
  - How to authenticate clients and keep tiny session state.
  - Private messages, broadcast, groups (create/join/leave), group messages.
- Start here: read the big header at the top of `server_grp.cpp`. It explains the approach and data structures.

## A2 — DNS Resolver (Iterative vs Recursive)

- Folder: `CS425/A2_220285_220276_220638`
- Files: `dnsresolver.py`, `README.md`
- What you’ll learn:
  - How real DNS resolution walks ROOT → TLD → AUTHORITATIVE servers.
  - How to use the system’s recursive resolver.
  - How to send queries, parse answers, and handle timeouts.
- Start here: the docstring at the top of `dnsresolver.py` explains the flow.

## A3 — Raw TCP Handshake (Client + Server)

- Folder: `CS425/A3220285220276220638`
- Files: `client.cpp`, `server.cpp`, `Makefile`, `README`
- What you’ll learn:
  - What happens under the hood of a TCP 3‑way handshake.
  - How to craft IP/TCP headers and checksums manually (for learning only!).
- Start here: read the headers at the top of `client.cpp` and `server.cpp`.

## A4 — Routing Simulator (Distance Vector + Link State)

- Folder: `CS425/A4220638220276220285`
- Files: `routing_sim.cpp`, `input*.txt`, `README.md`, `Makefile`
- What you’ll learn:
  - How routing tables are computed in Distance Vector and Link State.
  - How next hops emerge from Bellman‑Ford style and Dijkstra updates.
- Start here: the header at the top of `routing_sim.cpp` sets the mental model.

## Philosophy of this rewrite

- Explain Like I’m New: each main file begins with a plain‑language overview.
- Name things after what they do: data structures are described with intent.
- No magic leaps: tricky spots (epoll, raw sockets) are annotated.
- Preserve functionality: code still builds and runs as before; it’s just more readable.

## Build & Run Quick Notes

- A1: `cd CS425/A1 && make` (then run server, then client on another terminal).
- A2: `cd CS425/A2_220285_220276_220638 && python3 dnsresolver.py iterative example.com`
- A3: `cd CS425/A3220285220276220638 && make` (root permissions might be needed for raw sockets).
- A4: `cd CS425/A4220638220276220285 && make` then `./routing_sim < input1.txt`

If you want even more guidance (e.g., diagrams / flowcharts in the READMEs), say the word and I’ll add them.
