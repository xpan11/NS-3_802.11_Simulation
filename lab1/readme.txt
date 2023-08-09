Before run evaluation for Case A or Case B please go to wifi-mac.cc ~/ns-3-allinone/ns-3-dev/src/wifi/model first to set the setMinCw and setMaxCw value to correspondng setting(case A minCw=1, maxCw=1023, case B minCw=63, maxCw = 127)


for a single run of the lab1:
./waf --run "lab1"
there are some parameters we can set on the command line:
--nWifi :the number of Tx nodes
--tracing: enable tracing
--packetsize: the size of packet of the on-off application to reach CBR in bytes
--datarate: the datarate of the on-off application to reach CBR in bits

in order to run with parameters setting:
example: ./waf --run "lab1 --nWifi =3 --packetsize=3000000"

In order to run evaluation, just use Lab1Run.sh:
first cd to scratch, then type:
bash Lab1Run.sh 
then it will run Evaluation 1 and Evaluation 2.

So in In order to run Case A E1 and E2:
1. change all parameters in setMinCw() to 1 and all parameters in setMaxCw() to 1023.
2.go to scratch and type: bash Lab1Run.sh 



In In order to run Case B E1 and E2:
1. change all parameters in setMinCw() to 63 and all parameters in setMaxCw() to 127.
2.go to scratch and type: bash Lab1Run.sh 

If you just want to run certain evalutaion, there also a Evaluation1.sh and Evaluation2.sh provided.
