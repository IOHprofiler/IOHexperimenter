#!/bin/sh

mkfifo ../debug/query
mkfifo ../debug/reply

../debug/service/ioh-service --dbg-level XDebug --dimension 10 ../debug/query ../debug/reply

