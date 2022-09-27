//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"
#include "veins/modules/application/traci/Registry.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/TraCIDemoRSU11p.h"
#include "veins/modules/application/traci/constants.h"
#include "veins/base/utils/Coord.h"


#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>
#include <map>
#include <vector>
#include <iterator>
#include <list>
#include <sstream>
#include <fstream>



using namespace std;

using namespace veins;



// Result Analysis Works Start

//ifstream fin("Result.txt");



std::string varName;
double value;
double MessageCount=0;
double RecievedMessage;



// Result Analysis Works End


// For updating the registry
Registry registry;
std::vector<std::tuple<int, int, int>> vehicleLocation;
//list<int>NodeWithMessage;



Define_Module(veins::TraCIDemo11p);


int RandomNumberGenerator(int min, int max)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range

    int random = distr(gen); // generate numbers
    return random;
}


using Point = std::pair<double, double>;
std::ostream &operator<<(std::ostream &os, const Point &p) {
    auto x = p.first;
    if (x == 0.0) {
        x = 0.0;
    }
    auto y = p.second;
    if (y == 0.0) {
        y = 0.0;
    }
    return os << '(' << x << ", " << y << ')';
}




template <typename T> std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    auto itr = v.cbegin();
    auto end = v.cend();

    os << '[';
    if (itr != end) {
        os << *itr;
        itr = std::next(itr);
    }
    while (itr != end) {
        os << ", " << *itr;
        itr = std::next(itr);
    }
    return os << ']';
}



void printList(std::list<int> const &list)
{
    for (auto it = list.cbegin(); it != list.cend(); it++) {
        std::cout << *it << ", ";
    }
    std::cout<<std::endl;
}




void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        stage ++;
        TaskDeadline = RandomNumberGenerator(500,1000);
        ServiceTime = RandomNumberGenerator(50,TaskDeadline);

    }


}



void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{

    //std::cout<<"WSM"<<std::endl;
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);



    findHost()->getDisplayString().setTagArg("i", 1, "green");

    Coord VehicleCoord = mobility->getPositionAt(simTime());

    if (wsm->getRSUSelfMessage()==true)
                {
                    std::cout<< "RSU Self message"<<std::endl;
                    return;
                }

    if (wsm->getNodeID() != mac->getMACAddress())
    {

        //std::cout<<"Test2"<<std::endl;
        if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
        int hopCounter = wsm->getHopCounter();
        if (hopCounter == 3) {
            //std::cout << "WSM " << wsm->getId() << " stopped due to reaching max number of Hops!!!" << std::endl;
            stopService();
            //delete (wsm);
         }

        bool AlreadyWithSameMessage = false;
        if (std::find(NodeWithMessage.begin(), NodeWithMessage.end(), mac->getMACAddress()) != NodeWithMessage.end())
        {
            AlreadyWithSameMessage = true;
        }
        else {
            AlreadyWithSameMessage = false;
        }

        if (!sentMessage && AlreadyWithSameMessage == false) {
        //if (!sentMessage) {
            sentMessage = true;
            hopCounter = hopCounter + 1;
            // repeat the received traffic update once in 2 seconds plus some random delay
            wsm->setSerial(3);
            //std::cout<<"VehicleID2: "<< mobility->getId()<<std::endl;
            //wsm->setNodeID(mobility->getId());
            wsm->setNodeID(mac->getMACAddress());
            //std::cout<<"test:::::::::::::::::::::: "<<wsm->getNodeID()<<" " << mac->getMACAddress() << " " << mobility->getId() << " " << std::endl;
            NodeWithMessage.push_back(mac->getMACAddress());
            //std::cout<<mobility->getId()<<std::endl;
            wsm->setTaskDeadline(TaskDeadline);
            wsm->setServiceTime(ServiceTime);
            wsm->setNodeMobilityCoord(mobility->getPositionAt(simTime()));
            double vehicleSpeed = mobility->getSpeed();
            wsm->setSenderNodeSpeed(vehicleSpeed);
            Coord SenderNextCoord = mobility->getPositionAt(simTime()) + mobility->getCurrentDirection();
            wsm->setNodeMobilityNextCoord(SenderNextCoord);
            wsm->setMessageInitTime(simTime());

            int VehcileID = mac->getMACAddress();
            //std::cout<<"VID: " << wsm->getNodeID()<<std::endl;

            /*// Add Value to registry
            if ( registry.vehicleRegistry.find(VehcileID) == registry.vehicleRegistry.end() )
            {
                // Error
                //registry.vehicleRegistry[VehcileID] =  VehicleCoord ;
            }*/

            //printList(NodeWithMessage);
            MessageCount++;
            MessageCountBase++;
            scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), wsm->dup());
            //cancelAndDelete(wsm);
        }
    }
}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{

    //std::cout<<"In Self"<<std::endl;



    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {


        if (wsm->getRSUSelfMessage()==true)
            {
            std::cout<<"In handle RSU Self"<<std::endl;
                //sendDown(wsm->dup());
                //scheduleAt(simTime() + .3 + uniform(0.001, 0.23), wsm->dup());
                return;
            }

        bool AlreadyWithSameMessage = false;

        if (std::find(NodeWithMessage.begin(), NodeWithMessage.end(), mac->getMACAddress()) != NodeWithMessage.end())
            {
                AlreadyWithSameMessage = true;
            }
            else {
                AlreadyWithSameMessage = false;
            }

        if (AlreadyWithSameMessage == false) {
        //if (AlreadyWithSameMessage == false || AlreadyWithSameMessage == true) {
        //std::cout<<"In Self"<<std::endl;
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
            //std::cout<<"Test2"<<std::endl;
            //wsm->setNodeID(mobility->getId());
            wsm->setNodeID(mac->getMACAddress());
            //std::cout<<"Mac Address: " << mac->getMACAddress()<<std::endl;
            //std::cout<<"vehilce id" <<mobility->getId()<<std::endl;
            NodeWithMessage.push_back(mac->getMACAddress());
            wsm->setTaskDeadline(TaskDeadline);
            wsm->setServiceTime(ServiceTime);
            wsm->setNodeMobilityCoord(mobility->getPositionAt(simTime()));
            double vehicleSpeed = mobility->getSpeed();
            wsm->setSenderNodeSpeed(vehicleSpeed);
            Coord SenderNextCoord = mobility->getPositionAt(simTime()) + mobility->getCurrentDirection();
            wsm->setNodeMobilityNextCoord(SenderNextCoord);
            wsm->setMessageInitTime(simTime());

            sendDown(wsm->dup());
            int hopCounter = wsm->getHopCounter();
            /*if (hopCounter < 3) {
                hopCounter = hopCounter + 1;
                wsm->setHopCounter(hopCounter);
                //sendDown(wsm->dup());
            }*/

            int VehcileID = mac->getMACAddress();
            Coord VehicleCoord = mobility->getPositionAt(simTime());
            // Add Value to registry
            if ( registry.vehicleRegistry.find(VehcileID) == registry.vehicleRegistry.end() )
            {
                //Error
                //registry.vehicleRegistry[VehcileID] =  VehicleCoord ;
            }

            for (auto const& Vehicle : registry.vehicleRegistry)
            {

                int simulationTime = (int) simTime().dbl();
                std::tuple<int, int, int> locationData (VehcileID, simulationTime, Vehicle.first);
                vehicleLocation.push_back(locationData);
            }


            //wsm->setSerial(wsm->getSerial() + 1);
            if (wsm->getSerial() >= 3) {
                // stop service advertisements
                stopService();
                //delete (wsm);
            }
            else {
                //printList(NodeWithMessage);
                MessageCount++;
                MessageCountBase++;
                scheduleAt(simTime() + 0.1, wsm);
                //cancelAndDelete(wsm);
            }
        }
        else {
            DemoBaseApplLayer::handleSelfMsg(msg);
        }
    }
}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
    // stopped for for at least 10s?

    Coord vehicleCoord = mobility->getPositionAt(simTime());
    int simulationTime = (int) simTime().dbl();
    double vehicleSpeed = mobility->getSpeed();
    int vehicleID = mac->getMACAddress();

    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            wsm->setDemoData(mobility->getRoadId().c_str());

            // host is standing still due to crash
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
                //cancelAndDelete(wsm);

            }
            else {
                // send right away on CCH, because channel switching is disabled
                sendDown(wsm);
                //cancelAndDelete(wsm);

            }
        }
    }
    else {
        lastDroveAt = simTime();
        // Creating tuple of information
        std::tuple<veins::Coord, double, double> vehicleData (vehicleCoord, vehicleSpeed, simulationTime);

        // Adding information into vehicle registry.
        registry.vehicleRegistry[vehicleID].insert(registry.vehicleRegistry[vehicleID].begin(), vehicleData) ;

        // Removing last data
        int size = registry.vehicleRegistry[vehicleID].size();
        if (size>3){// If we have more than 3 vehicle positions to work with
            for (auto const& vehicle : registry.vehicleRegistry)
                {
                Coord oldest = get<0>(registry.vehicleRegistry[vehicleID][size-1]) ;
                std::vector<std::vector<double>> xCoordTrain;
                std::vector<double> yCoordTrain;

                for(int j=0; j < size; j++){
                   Coord current = get<0>(registry.vehicleRegistry[vehicleID][j]);
                   //Error
                   //std::cout<<"current: " << registry.vehicleRegistry[vehicleID] << " " << current <<std::endl;
                     }

                }
            if( size > 15){
               registry.vehicleRegistry[vehicleID].pop_back();
            }
        }
    }
}


void TraCIDemo11p::finish()
{
    //printList(NodeWithMessage);
    DemoBaseApplLayer::finish();
    std::cout<<"Message Counter: " <<MessageCount<<std::endl;

}
