
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/applications-module.h"
#include "ns3/basic-energy-source.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/simple-device-energy-model.h"
#include "ns3/wifi-radio-energy-model.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-standards.h"
#include "aarf-wifi-manager.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HW2_Task1_Team_28"); 

int
main(int argc, char* argv[])
{
    
    uint32_t nWifi = 5;
    NodeContainer wifiNodes;
    wifiNodes.Create(nWifi);
    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    SetType ("ns3::AdhocWifiMac", "QosSupported", BooleanValue (false));

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    wifi.SetStandard(WIFI_STANDARD_80211g);
    
    NetDeviceContainer adhocDevices;
    adhocDevices = wifi.Install(phy, mac, wifiNodes);

    MobilityHelper mobility;
    /* 
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(10.0),
                                  "MinY",
                                  DoubleValue(10.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(2.0),
                                  "GridWidth",
                                  UintegerValue(5),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    */

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-90, 90, -90, 90)));
    mobility.Install(wifiNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    /*
    Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
    Ptr<WifiRadioEnergyModel> energyModel = CreateObject<WifiRadioEnergyModel>();

    energySource->SetInitialEnergy(300);
    energyModel->SetEnergySource(energySource);
    energySource->AppendDeviceEnergyModel(energyModel);

    // aggregate energy source to node
    wifiApNode.Get(0)->AggregateObject(energySource);
    */

    InternetStackHelper stack;
    stack.Install(wifiNodes);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "/24");

    Ipv4InterfaceContainer wifiInterfaces;
    wifiInterfaces = address.Assign(adhocDevices);

    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(wifiNodes.Get(0));
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(5.0));

    UdpEchoClientHelper echoClient4(wifiInterfaces.GetAddress(4), 20);
    echoClient4.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient4.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient4.SetAttribute("PacketSize", UintegerValue(512));

    UdpEchoClientHelper echoClient3(wifiInterfaces.GetAddress(3), 20);
    echoClient3.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient3.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient3.SetAttribute("PacketSize", UintegerValue(512));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}