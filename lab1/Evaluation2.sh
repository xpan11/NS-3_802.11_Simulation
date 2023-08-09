#! /bin/bash

echo "Evaluation 2"

echo "if run for case A: Before run case A please make sure the setMinCw() is set to 1 and setMaxCw() is set to 1023 in wifi-mac.cc file."
echo ""
echo "if run for case B: Before run case B please make sure the setMinCw() is set to 63 and setMaxCw() is set to 127 in wifi-mac.cc file."

echo "Case A Evaluation 2"

../waf --run "lab1 --nWifi=20 --datarate=3000000"
../waf --run "lab1 --nWifi=20 --datarate=6000000"
../waf --run "lab1 --nWifi=20 --datarate=9000000"
../waf --run "lab1 --nWifi=20 --datarate=12000000"
../waf --run "lab1 --nWifi=20 --datarate=15000000"

