#!/bin/bash
# Send a signal to gled once a macro has reached a given point, and time the
# exit.
#
#   [SIG=INT] sigtest.sh <out> <pattern> <macro> [gled options...]
#
# Runs 'gled --run --noprompt --logflush <options> <macro>' from this
# directory with its output in <out>, waits until <pattern> appears there,
# sends SIG (default TERM) a second later and reports when gled exits.  Gled
# is killed if it is still running 120 s after the signal.  Needs the gled
# environment (build_env.sh) and DISPLAY (an off-screen X server is best).

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
