#!/bin/bash
# Authenticated sun and saturn on localhost; see test/README.md.
# usage: run_auth.sh <outdir> [<port>]

out=${1:?usage: run_auth.sh <outdir> [<port>]}
port=${2:-9161}
dir=$(cd $(dirname $0) && pwd)
mkdir -p $out || exit 1
out=$(cd $out && pwd)
rm -rf $out/auth $out/auth-1024 $out/auth-wrong $out/auth-wrong-eye $out/auth-grp-sub $out/auth-grp

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

# The Eye identity mercury holds venus's private key.
cp -a $out/auth $out/auth-wrong-eye
cp $out/auth/private_keys/venus $out/auth-wrong-eye/private_keys/mercury

# Group @test lists xsaturn, which contains saturn, or saturn itself.
cp -a $out/auth $out/auth-grp-sub
printf "venus\nxsaturn\n" > $out/auth-grp-sub/groups/@test
cp -a $out/auth $out/auth-grp
printf "venus\n  saturn  \n" > $out/auth-grp/groups/@test

fails=0

# run <name> <sun authdir> <client authdir> <client exit> <client log pattern> [client args]
# Sun macros go in $sun_args, the client program in $client (default saturn).
run()
{
  local name=$1 sun_auth=$2 cli_auth=$3 exit=$4 expect=$5
  shift 5
  local sl=$out/$name.sun.log cl=$out/$name.cli.log
  timeout 30 saturn --auth --authdir $sun_auth -p $port --allowmoons --noprompt --run --logflush \
    $sun_args < /dev/null > $sl 2>&1 &
  local sp=$!
  for i in $(seq 100); do grep -q fix_fire_king_id $sl 2>/dev/null && break; sleep 0.2; done
  timeout -k 10 15 ${client:-saturn} --authdir $cli_auth --master localhost:$port \
    --noprompt --run --logflush "$@" < /dev/null > $cl 2>&1
  local ce=$?
  kill $sp 2>/dev/null; wait $sp

  if [ $ce = $exit ] && grep -q "$expect" $cl; then
    printf "PASS  %-12s\n" $name
  else
    printf "FAIL  %-12s (client exit %s)\n" $name $ce
    fails=$((fails + 1))
  fi
}

conn=arrival_of_kings
run match     $out/auth      $out/auth       124 $conn
run sun1024   $out/auth-1024 $out/auth-1024  124 $conn
run wrongkey  $out/auth      $out/auth-wrong 1   "auth failed"
run unknown   $out/auth      $out/auth       1   "denied.*unknown identity 'nobody'" --saturnid nobody
run guest     $out/auth      $out/auth       1   "denied.*not accepting guest Saturns" --saturnid guest

sun_args=$GLEDSYS/macros/std_auth.C
run group-sub $out/auth-grp-sub $out/auth-grp-sub 124 "ATTACH refused:.*insufficient karma" $dir/attach_group.C
run group     $out/auth-grp     $out/auth-grp     124 "ATTACH granted" $dir/attach_group.C

# The Eye of a gled logs in as mercury, through the sun's direct socket.
if [ -n "$DISPLAY" ]; then
  client=gled
  eye_args="--log + --eyeid mercury --rnr GL $GLEDSYS/macros/moon.C"
  run eye          $out/auth $out/auth       124 finalize_eye_connection $eye_args
  run eye-wrongkey $out/auth $out/auth-wrong-eye 124 "Eye creation failed.*auth failed" $eye_args
  client=
else
  echo "SKIP  eye, eye-wrongkey (no DISPLAY)"
fi

echo "$fails failed"
exit $fails
