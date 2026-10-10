#!/bin/bash
# Authenticated sun and saturn on localhost; see test/README.md.
# usage: run_auth.sh <outdir> [<port>]

out=${1:?usage: run_auth.sh <outdir> [<port>]}
port=${2:-9161}
mkdir -p $out || exit 1
out=$(cd $out && pwd)
rm -rf $out/auth $out/auth-1024 $out/auth-wrong

gled-auth-init -dir $out/auth > $out/auth-init.log 2>&1 || { echo "gled-auth-init failed"; exit 1; }

# A 1024-bit sun with 2048-bit clients.
cp -a $out/auth $out/auth-1024
gled-auth-genkeypair -dir $out/auth-1024 -name sun1024 -keysize 1024 >> $out/auth-init.log 2>&1
for d in private_keys public_keys; do
  mv $out/auth-1024/$d/sun1024 $out/auth-1024/$d/sun.absolute
done

# The client holds a private key that does not match its public key.
cp -a $out/auth $out/auth-wrong
cp $out/auth/private_keys/venus $out/auth-wrong/private_keys/saturn

fails=0

# run <name> <sun authdir> <client authdir> <ok|client log pattern> [client options]
run()
{
  local name=$1 sun_auth=$2 cli_auth=$3 expect=$4
  shift 4
  local sl=$out/$name.sun.log cl=$out/$name.cli.log
  timeout 30 saturn --auth --authdir $sun_auth -p $port --allowmoons --noprompt --run --logflush \
    < /dev/null > $sl 2>&1 &
  local sp=$!
  for i in $(seq 100); do grep -q fix_fire_king_id $sl 2>/dev/null && break; sleep 0.2; done
  timeout 12 saturn --authdir $cli_auth --master localhost:$port --noprompt --run --logflush "$@" \
    < /dev/null > $cl 2>&1
  local ce=$?
  kill $sp 2>/dev/null; wait $sp

  local ok=0
  if [ "$expect" = ok ]; then
    [ $ce = 124 ] && grep -q arrival_of_kings $cl && ok=1
  else
    [ $ce = 1 ] && grep -q "$expect" $cl && ok=1
  fi
  if [ $ok = 1 ]; then
    printf "PASS  %-12s\n" $name
  else
    printf "FAIL  %-12s (client exit %s)\n" $name $ce
    fails=$((fails + 1))
  fi
}

run match    $out/auth      $out/auth       ok
run sun1024  $out/auth-1024 $out/auth-1024  ok
run wrongkey $out/auth      $out/auth-wrong "auth failed"
run unknown  $out/auth      $out/auth       "denied.*unknown identity 'nobody'" --saturnid nobody
run guest    $out/auth      $out/auth       "denied.*not accepting guest Saturns" --saturnid guest

echo "$fails failed"
exit $fails
