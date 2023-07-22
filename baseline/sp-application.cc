#include "sp-application.h"
#define PURPLE_CODE "\033[95m"
#define CYAN_CODE "\033[96m"
#define TEAL_CODE "\033[36m"
#define BLUE_CODE "\033[94m"
#define GREEN_CODE "\033[32m"
#define YELLOW_CODE "\033[33m"
#define LIGHT_YELLOW_CODE "\033[93m"
#define RED_CODE "\033[91m"
#define BOLD_CODE "\033[1m"
#define END_CODE "\033[0m"


using namespace std;
namespace ns3{

    NS_LOG_COMPONENT_DEFINE ("SPApplication");
    NS_OBJECT_ENSURE_REGISTERED (SPApplication);

    TypeId
    SPApplication::GetTypeId ()
    {
        static TypeId tid =
            TypeId ("ns3::SPApplication")
                .SetParent<Application> ()
                .AddConstructor<SPApplication> ();
                
        return tid;
    }

    TypeId
    SPApplication::GetInstanceTypeId () const
    {
        return SPApplication::GetTypeId ();
    }

    SPApplication::SPApplication()
    {

    }

    SPApplication::~SPApplication()
    {

    }

    void 
    SPApplication::Setup (Ptr<Socket> _speaker_socket, Ptr<Socket> _listener_socket, uint32_t packetSize, uint32_t nPackets, DataRate dataRate,uint16_t _port)
    {
        listener_socket = _listener_socket;
        speaker_socket = _speaker_socket;
        m_packetSize = packetSize;
        m_nPackets = nPackets;
        m_dataRate = dataRate;
        port = _port;
    }

     void 
    SPApplication::StopApplication (void)
    {
        m_running = false;

        
        if(listener_socket)
            {
            listener_socket->Close ();
            }
            if(speaker_socket)
            {
                speaker_socket->Close();
            }
        for(auto x : m_socketList)
        {
            x->Close();
        }
        m_socketList.clear();
    }

    void
    SPApplication::StartApplication ()
    {
        NS_LOG_FUNCTION (this);
        m_running = true;
        m_packetsSent = 0;


        InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), port);
        if (listener_socket->Bind(local) == -1)
        {
            NS_FATAL_ERROR("Failed to bind socket");
        }
        NS_LOG_INFO(RED_CODE<<"Service provider app started successfully"<<END_CODE);
        listener_socket->Listen();
        listener_socket->ShutdownSend();

        listener_socket->SetRecvCallback(MakeCallback( &SPApplication::ReceivePacket, this));

        listener_socket->SetAcceptCallback (
        MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
        MakeCallback (&SPApplication::HandleAccept, this));

    }

    void
    SPApplication::AddIoTAddr(Address a)
    {
        iot_addrs.push_back(a);
    }

    void 
    SPApplication::HandleAccept (Ptr<Socket> s, const Address& from)
    {
        s->SetRecvCallback (MakeCallback (&SPApplication::ReceivePacket, this));
        m_socketList.push_back (s);
    }

    void
    SPApplication::ReceivePacket (Ptr<Socket> sockt)
    {
    
        Address from;
        Ptr<Packet> packet;
        while( (packet = sockt->RecvFrom (from)) )
        {
            if(packet->GetSize()==0)break;
            packet->RemoveAllPacketTags ();
            packet->RemoveAllByteTags ();
            InetSocketAddress address = InetSocketAddress::ConvertFrom (from);

            Address adrs = InetSocketAddress(address.GetIpv4(), port);

            NS_LOG_INFO(YELLOW_CODE << "SP::APP Packet RECEIVED AT " << Now().GetSeconds()<<"\tfrom\t"<<InetSocketAddress::ConvertFrom(address).GetIpv4() << END_CODE);


            
            uint8_t data[255];
            packet->CopyData(data,255);

            uint8_t type = data[0];

            if(type == PacketData::REQUEST_FOR_SERVICE)
            {
                
                PacketData pd = PacketData();
                
                pd.SetBuffer(data, 255);
                pd.DeserializeRequestForService();
                Time ts = pd.GetTime();
                uint16_t addrss_ind = pd.GetFromAddress();

                NS_LOG_INFO(YELLOW_CODE<<"Incoming request for service from "<<InetSocketAddress::ConvertFrom(address).GetIpv4()<<" "<<InetSocketAddress::ConvertFrom(address).GetPort() <<END_CODE);

                
                Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable> ();
                Time service_rtime = Seconds (rand->GetValue (1, 3));
                // Time service_rtime = Seconds(3);
                Simulator::Schedule(service_rtime, &SPApplication::ScheduleService, this, ts, addrss_ind, adrs);
                
                NS_LOG_INFO(TEAL_CODE<<"Service packet send to "<<InetSocketAddress::ConvertFrom(address).GetIpv4()<<" after (secs) \t"<<service_rtime.GetSeconds()<<END_CODE);
                
            }
            
        }
        
        
    }

    void 
    SPApplication::ScheduleService(Time ts, uint16_t addrss_ind, Address adrs)
    {
        PacketData sendData = PacketData();
                
                sendData.SerializeResponseForService(
                    PacketData::RESPONSE_FOR_SERVICE,
                    ts,
                    addrss_ind
                );

                Ptr<Packet> sendpacket = Create<Packet> (sendData.GetBuffer(), 255);
                
                Ptr<Socket> speak = Socket::CreateSocket (GetNode(), TcpSocketFactory::GetTypeId ());
                speak->Bind();
                speak->Connect(adrs);
                speak->Send(sendpacket);

                
    }

    void
    SPApplication::SetWifiMode(WifiMode mode)
    {
        m_mode = mode;
    }



}
