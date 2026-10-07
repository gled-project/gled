#!/bin/bash
# Run GUI demos and check how they ran and how they shut down.
#
#   run_demos.sh <outdir> [<LibSet>/<macro>.C ...]
#
# Without a list, runs the base demos below.  A macro that does not exist
# fails.  Each one runs as
# 'timeout 22 gled --noprompt <macro>' in $DEMOS_DIR/<LibSet>, with its output
# in <outdir>/<LibSet>-<macro>.log.  DEMOS_DIR defaults to $GLEDSYS/demos.
# A demo passes when
#   - the exit status is 124: it ran until timeout's SIGTERM and then exited;
#   - the log has no crash, heap-corruption or cling error lines (timeout
#     still returns 124 when gled aborts while shutting down);
#   - the GUI message loop exited exactly once.
# Needs the gled environment (build_env.sh) and DISPLAY; demos open windows,
# so an off-screen X server (Xvnc) is best.  Exit status: number of failures.

BASE_DEMOS="Geom1/eden.C Geom1/gl_tests.C Geom1/images2.C Geom1/images.C Geom1/metagui_test.C
  Geom1/remove_test.C Geom1/rot_lamps.C Geom1/spheres.C Geom1/WS_demo.C
  GledCore/arc.C GledCore/hello_gled.C GledCore/perf_meter.C
  GledCore/sig_test_operator.C GledCore/string_map.C"

BAD='Terminal signal|Segmentation|SEGV|There was a crash|free\(\)|dumped core|corrupt|Assertion|: error: |Error in <'

[ -n "$1" ] || { sed -n '2,17s/^# \{0,1\}//p' "$0"; exit 1; }
[ -n "$GLEDSYS" ] || { echo "GLEDSYS not set; source build_env.sh"; exit 1; }
DEMOS_DIR=${DEMOS_DIR:-$GLEDSYS/demos}
out=$(realpath -m "$1"); shift
mkdir -p "$out"
demos=${*:-$BASE_DEMOS}

fail=0
for d in $demos; do
  ls=${d%%/*}; m=${d#*/}; log=$out/$ls-${m%.C}.log
  if [ ! -f "$DEMOS_DIR/$ls/$m" ]; then
    printf "FAIL  %-32s no such macro in %s\n" "$d" "$DEMOS_DIR/$ls"
    fail=$((fail+1)); continue
  fi
  (cd "$DEMOS_DIR/$ls" && timeout 22 gled --noprompt "$m" > "$log" 2>&1)
  st=$?
  nbad=$(grep -a -c -E "$BAD" "$log")
  nloop=$(grep -a -c 'exiting GledGUI::MessageLoop' "$log")
  if [ $st -eq 124 ] && [ $nbad -eq 0 ] && [ $nloop -eq 1 ]; then
    printf "PASS  %-32s\n" "$d"
  else
    printf "FAIL  %-32s exit=%d bad-lines=%d msgloop-exits=%d\n" "$d" $st $nbad $nloop
    grep -a -m 3 -E "$BAD" "$log" | sed 's/^/        /' | cut -c1-150
    fail=$((fail + 1))
  fi
done
echo "$fail failed"
exit $fail
