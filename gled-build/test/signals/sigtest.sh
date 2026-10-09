#!/bin/bash
# Sends a signal to gled once a macro's output shows <pattern> and times the
# exit; see test/README.md.
#   [SIG=INT] sigtest.sh <out> <pattern> <macro> [gled options...]

out=$(realpath -m "$1"); pat=$2; shift 2
cd "$(dirname "$0")" || exit 1

t0=$(date +%s.%N)
ts() { printf "%6.1f" $(echo "$(date +%s.%N) - $t0" | bc); }
pause() { timeout $1 tail -f /dev/null; }

gled --run --noprompt --logflush "$@" > "$out" 2>&1 &
P=$!
for i in $(seq 1 600); do grep -q "$pat" "$out" && break; pause 0.2; done
echo "$(ts) saw '$pat'"
pause 1
echo "$(ts) SIG${SIG:-TERM}"; kill -${SIG:-TERM} $P
for i in $(seq 1 600); do kill -0 $P 2>/dev/null || break; pause 0.2; done
if kill -0 $P 2>/dev/null; then
  echo "$(ts) STILL ALIVE after 120 s"; kill -9 $P; wait $P 2>/dev/null; exit 99
fi
wait $P; st=$?
echo "$(ts) exited, status $st"
grep -n -E 'Received|pump|TApplication::Run|canvas batch|exiting GledGUI|Terminal|SEGV|crash|free\(\)|corrupt' "$out"
exit $st
