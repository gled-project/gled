#!/bin/bash
# Runs GUI demos and checks how they ran and shut down; see test/README.md.
#   run_demos.sh <outdir> [<LibSet>/<macro>.C ...]

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
