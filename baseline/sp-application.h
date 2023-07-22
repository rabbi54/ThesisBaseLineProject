#ifndef WAVETEST_SP_APPLICATION_H
#define WAVETEST_SP_APPLICATION_H
#include "ns3/application.h"
#include "ns3/core-module.h"
#include "ns3/wave-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include <vector>
#include <utility>
#include <map>
#include<float.h>
#include "ns3/tcp-header.h"
#include "packet-data.h"

using namespace std;
using namespace ns3;
namespace ns3
{
    /**
     * Main three function
     * 
     * 1. Request for trust to its neighbour ttp
     * 2. Request for service to a service provider
     * 3. Send feedback window when ask for trust info to its neighbour ttp
     * 4. Manage the trust window haha
     * 
     * 
    */

    class SPApplication : public ns3::Application
    {
        public: 
            
            static TypeId GetTypeId (void);
            virtual TypeId GetInstanceTypeId (void) const;

            SPApplication();
            ~SPApplication();

            
            void AddIoTAddr(Address iot);
            void ReceivePacket (Ptr<Socket> scokt);
            void ScheduleService(Time ts, uint16_t addrss_ind, Address adrs);
            void SetWifiMode (WifiMode mode);

            void Setup (Ptr<Socket> _speaker_socket, Ptr<Socket> _listener_socket, uint32_t packetSize, uint32_t nPackets, DataRate dataRate,uint16_t _port);
            
        private:
            /** \brief This is an inherited function. Code that executes once the application starts
             */
        

            void StartApplication();
            void StopApplication();
            void HandleAccept(Ptr<Socket> s, const Address& from);
            // **< Packet size in bytes */
            Ptr<NetDevice> m_waveDevice; /**< A WaveNetDevice that is attached to this device */  
            vector<ns3::Address> iot_addrs;
             /**< Time limit to keep neighbors in a list */
            vector<Ptr<Socket>> m_socketList;
            WifiMode m_mode; /**< data rate used for broadcasts */
            //You can define more stuff to record statistics, etc.

            Ptr<Socket> listener_socket;
            Ptr<Socket> speaker_socket;

            uint32_t m_packetSize;
            uint32_t m_nPackets;
            DataRate m_dataRate;
            uint16_t port;

            bool m_running;
            uint32_t m_packetsSent;


    };
}

#endif