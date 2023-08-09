/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/flow-probe.h"
#include "ns3/flow-classifier.h"
#include "ns3/flow-monitor-module.h"
// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("Lab1S1");

int 
main (int argc, char *argv[])
{

  bool verbose = true;
  uint32_t nWifi = 3;
  bool tracing = true;
  uint32_t nRx = 1;
  uint32_t SentPackets = 0;
  uint32_t ReceivedPackets = 0;
  uint32_t LostPackets = 0;
  uint32_t pksize = 512;
  uint32_t datarate = 6000000;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("packetsize", "packetsize", pksize);
  cmd.AddValue ("datarate", "datarate", datarate);
 

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }


  NodeContainer wifiTxNodes;
  wifiTxNodes.Create (nWifi);
  NodeContainer wifiRxNodes;
  wifiRxNodes.Create(nRx);  

  YansWifiChannelHelper channel =  YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211a);

  WifiMacHelper mac;
  mac.SetType("ns3::AdhocWifiMac");

  NetDeviceContainer TxDevices;
  TxDevices = wifi.Install (phy, mac, wifiTxNodes);

  NetDeviceContainer RxDevices;
  RxDevices = wifi.Install (phy, mac, wifiRxNodes);

  MobilityHelper mobility;
/*

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
*/
  mobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator","rho", DoubleValue(13.0), "X", DoubleValue(0.0), "Y",DoubleValue(0.0));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (wifiTxNodes);
  mobility.Install (wifiRxNodes);

  NS_LOG_INFO("Setting up IPv4 stack.");

  InternetStackHelper stack;
  stack.Install (wifiTxNodes);
  stack.Install (wifiRxNodes);

  NS_LOG_INFO("Assigning IPv4 address.");
  Ipv4AddressHelper address;

  address.SetBase ("192.168.0.0", "255.255.255.0");

  Ipv4InterfaceContainer wifiInterfaces;
  wifiInterfaces = address.Assign (TxDevices);
  Ipv4InterfaceContainer RxInterfaces;
  RxInterfaces = address.Assign (RxDevices);

/*
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifiRxNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
*/

//receiver socket on Rx
/*
  TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket(wifiRxNodes.Get(0), tid);

*/




  NS_LOG_INFO("Setting up UDP recieve sink.");
//create a packet sink on the reciver
  uint16_t port = 9;
  Address sinkLocalAddress(InetSocketAddress(RxInterfaces.GetAddress (0),port));
  //cout<< AddressValue(InetSocketAddress(RxInterfaces.GetAddress (0),port))<<endl;
  PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApp = sinkHelper.Install(wifiRxNodes.Get(0));
  
  sinkApp.Start(Seconds(1.0));
  sinkApp.Stop(Seconds(10.0));

/*

  //problem here
  UdpEchoClientHelper echoClient (RxInterfaces.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  for(uint32_t i = 0;i<nWifi;i++){
        ApplicationContainer clientApps = echoClient.Install (wifiTxNodes.Get (i));
        clientApps.Start (Seconds (2.0));
        clientApps.Stop (Seconds (10.0));}
*/
//OnOffApplication traffic source to use UDP



   NS_LOG_INFO("Setting up onoff application.");
   for(uint32_t i= 0; i< nWifi;i++){
        OnOffHelper clientHelper("ns3::UdpSocketFactory", wifiInterfaces.GetAddress(i));
        clientHelper.SetAttribute("Remote", AddressValue(InetSocketAddress(RxInterfaces.GetAddress (0),port)));
        clientHelper.SetAttribute("PacketSize", UintegerValue(pksize));
        clientHelper.SetConstantRate(DataRate(datarate));
        ApplicationContainer clientApps = (clientHelper.Install(wifiTxNodes.Get(i)));
        clientApps.Start(Seconds(2.0));
        clientApps.Stop(Seconds(10.0));
   }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));
  //Throughput and Delay using Tracemetrics
  AsciiTraceHelper ascii;
  phy.EnableAsciiAll(ascii.CreateFileStream("phy.tr"));
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  if (tracing == true)
    {
      phy.EnablePcapAll ("third");
      //wifi.EnablePcap ("third", RxDevices.Get(0),true)
    }

  
  Simulator::Run ();
  //flowMonitor->SerializeToXmlFile("Lab1.xml",true,true);
int j=0;
float AvgThroughput = 0;
float Throughput = 0;

Time Delay;

Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

NS_LOG_UNCOND("Node:" <<iter->first);
NS_LOG_UNCOND("Src Addr" <<t.sourceAddress << "         Dst Addr "<< t.destinationAddress);
NS_LOG_UNCOND("Throughput =" <<(iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024)/1000<<"Mbps");

SentPackets = SentPackets +(iter->second.txPackets);
ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
LostPackets = LostPackets + (iter->second.txPackets-iter->second.rxPackets);
AvgThroughput = AvgThroughput + (iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024)/1000;
Delay = Delay + (iter->second.delaySum);


j = j + 1;

}
Throughput = AvgThroughput;
AvgThroughput = AvgThroughput/j;
NS_LOG_UNCOND("----------------------------------------------"<<std::endl);
NS_LOG_UNCOND("Total sent packets  =" << SentPackets);
NS_LOG_UNCOND("Total Received Packets =" << ReceivedPackets);
NS_LOG_UNCOND("Total Lost Packets =" << LostPackets);
NS_LOG_UNCOND("Packet Loss ratio =" << ((LostPackets*100)/SentPackets)<< "%");
NS_LOG_UNCOND("Packet delivery ratio =" << ((ReceivedPackets*100)/SentPackets)<< "%");
NS_LOG_UNCOND("Average Throughput =" << AvgThroughput<< "Mbps");
NS_LOG_UNCOND("Throughput =" << Throughput<< "Mbps");

monitor->SerializeToXmlFile("manet-routing.xml", true, true);


  Simulator::Destroy ();
  return 0;
}
