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

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/packet.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-classifier.h"

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

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

Ptr<PacketSink> sinkptr;                       /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

void
CalculateThroughput ()
{
   Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
   double cur = (sinkptr->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;      /*Convert Application RX Packets to MBits. */
   std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
   lastTotalRx = sinkptr->GetTotalRx ();
   Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

/*static void
RxDrop (Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
}*/

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  //uint32_t nCsma = 3;
  uint32_t nWifi = 1;
  bool tracing = true;

  CommandLine cmd;
  //cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e3));
  
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());
  
  phy.Set ("TxPowerStart", DoubleValue (10.0));
  phy.Set ("TxPowerEnd", DoubleValue (10.0));
  phy.Set ("TxPowerLevels", UintegerValue (1));
  phy.Set ("TxGain", DoubleValue (0));
  phy.Set ("RxGain", DoubleValue (0));
  phy.Set ("RxNoiseFigure", DoubleValue(40));
  phy.Set ("CcaMode1Threshold", DoubleValue (-79));
  phy.Set ("EnergyDetectionThreshold", DoubleValue (-79 + 3));
  phy.SetErrorRateModel ("ns3::YansErrorRateModel");


  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);


  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (p2pNodes.Get (1));

  InternetStackHelper stack;
  //stack.Install (csmaNodes);
  stack.Install (p2pNodes.Get (1));
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces, stInterface;
  p2pInterfaces = address.Assign (p2pDevices);

  //address.SetBase ("10.1.2.0", "255.255.255.0");
  //Ipv4InterfaceContainer csmaInterfaces;
  //csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  stInterface = address.Assign (staDevices);
  address.Assign (apDevices);

  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (false));

  //UdpEchoServerHelper echoServer (9);
  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (stInterface.GetAddress (0),sinkPort));
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                    InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = sink.Install (wifiStaNodes);
  sinkptr = StaticCast<PacketSink> (sinkApps.Get (0));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));


  OnOffHelper onOffHelper ("ns3::TcpSocketFactory", sinkAddress);
  onOffHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelper.SetAttribute ("DataRate",StringValue ("10Mbps"));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer source;

  source.Add (onOffHelper.Install (p2pNodes.Get (1)));
  source.Start (Seconds (1.0));
  source.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  phy.EnablePcap ("AccessPoint", apDevices);

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Schedule (Seconds (1.1), &CalculateThroughput);
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  
  //change
  //myWifiNetDevice->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));
 // Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&RxDrop));

  monitor->CheckForLostPackets();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
  for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i!= stats.end();i++)
  {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
          std::cout<<"Flow"<<i->first <<"("<<t.sourceAddress<<"->"<<t.destinationAddress <<")\n";
          std::cout<<" Tx Bytes: "<<i->second.txBytes<<"\n";
          std::cout<<" Rx Bytes: "<<i->second.rxBytes<<"\n";
          std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
          std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
          std::cout << "  lostPackets: " << (i->second.txPackets - i->second.rxPackets) << "\n";
  }
 

  double averageThroughput = ((sinkptr->GetTotalRx () * 8) / (1e6  * (double)10));

  std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;

  /*if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", staDevices.Get (0));
      //csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }*/

  Simulator::Destroy ();
  return 0;
}
