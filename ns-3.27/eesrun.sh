#!/bin/bash
echo "Cleaning files (if any)"
sudo rm -r outp.txt
echo "Simulating with Threshold Energy 40.0 dbm and SACK = false"
for value in {1..3}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=false --edt=40.0" >> outp.txt
done
echo "Simulating with Threshold Energy 40.0 dbm and SACK = true"
for value in {1..3}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=true --edt=40.0" >> outp.txt
done
echo "Simulating with Threshold Energy 45.0 dbm and SACK = false"
for value in {4..6}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=false --edt=45.0" >> outp.txt
done
echo "Simulating with Threshold Energy 45.0 dbm and SACK = true"
for value in {4..6}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=true --edt=45.0" >> outp.txt
done
echo "Simulating with Threshold Energy 50.0 dbm and SACK = false"
for value in {7..9}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=false --edt=50.0" >> outp.txt
done
echo "Simulating with Threshold Energy 50.0 dbm and SACK = true"
for value in {7..9}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=true --edt=50.0" >> outp.txt
done
echo "Simulating with Threshold Energy 55.0 dbm and SACK = false"
for value in {10..12}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=false --edt=55.0" >> outp.txt
done
echo "Simulating with Threshold Energy 55.0 dbm and SACK = true"
for value in {10..12}
do
	NS_GLOBAL_VALUE="RngRun=$value" ./waf --run="scratch/error-model2 --sack=true --edt=55.0" >> outp.txt
done
