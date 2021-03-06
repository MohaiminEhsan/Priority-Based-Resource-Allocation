//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include "veins/modules/application/traci/TraCIDemoRSU11p.h"
#include "veins/modules/application/traci/Registry.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/constants.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>
#include <map>
#include <vector>
#include <iterator>
#include <list>
#include <future>
#include <unistd.h>

using namespace std;


using namespace veins;

using namespace constants;

Define_Module(veins::TraCIDemoRSU11p);


//Result Analysis
std::string varNameRSU;
double valueRSU;
double MessageCountRSU;
double RecievedMessageRSU=0;





void TraCIDemoRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0)
    {
        QueueHandlingMessage = new cMessage("Queue Handling Message", QUEUE_HANDLING);
        RSUStatusChangeMessage = new cMessage("RSU Status Change Message", STATUS_CHANGE);
        //DeleteFromOtherRSUNotification = new cMessage ("Delete a request from other RSU", DELETE_REQUEST_FROM_RSU);
        RSUStatusChangeMessage->setName("Initialize");
        //DeleteFromOtherRSUNotification->setName("Initialize");
        scheduleAt(simTime()+uniform(0.05, 1), RSUStatusChangeMessage);
        //scheduleAt(simTime()+uniform(0.05, 0.5), DeleteFromOtherRSUNotification);
        scheduleAt(simTime()+5+uniform(0.05, 1), QueueHandlingMessage);
    }
}



void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}


bool NextDestinationinRSURange(float NextDistancewithRSU, double RSUmaxRange)
{
    if (NextDistancewithRSU>=RSUmaxRange)
    {
        return false;
    }
    else if (NextDistancewithRSU<=RSUmaxRange)
    {
        return true;
    }
    else return true;
}




double sq(double x) {
    return x * x;
}

int SubPriotizationFunction(bool Distance, bool Connection, bool InRSURange)
{
    int SP;
    if (Distance)
    {
        if (Connection)
        {
            SP = 1;
        }
        else if (!Connection)
        {
            SP = 2;
        }
    }
    else if (!Distance)
    {
        if (Connection)
        {
            if (!InRSURange)
            {
                SP = 3;
            }
            else
            {
                SP = 2;
            }
        }
        else if (!Connection)
        {
            SP = 3;
        }
    }
    return SP;
}

bool DefineDistance(float Distance, double rsuRange)
{
    float DistanceT = 0.50;
    if (Distance <= DistanceT * rsuRange)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DefineConnection(float connectionStr)
{
    float ConnectionT = 85;
    connectionStr = abs(connectionStr);
    if (connectionStr > ConnectionT)
    {
        return false;
    }
    else
    {
        return true;
    }

}


void showq(queue<tuple <int, double, std::string, double, int> > gq)
{
    queue<tuple <int, double, std::string, double, int> > g = gq;
    while (!g.empty()) {
        tuple <int, double, std::string, double, int> Tuple = g.front();
        std::cout << '\t' << get<0>(Tuple) << '\t' << get<1>(Tuple) <<'\t' << get<2>(Tuple) << '\t' << get<3>(Tuple)<< '\t' << get<4>(Tuple) <<"---->";
        g.pop();
    }
    std::cout<<std::endl;
}



double SimTimetoDoble (simtime_t time)
{
    std::string Time = time.str();
    double TimeDouble = ::atof(Time.c_str());
    return TimeDouble;
}





void printlist(std::list<int> const &list)
{
    for (auto it = list.cbegin(); it != list.cend(); it++) {
        std::cout << *it << ", ";
    }
    std::cout<<std::endl;
}



bool checkinQ(queue<tuple <int, double, std::string, double, int> > q, int id)
{
    /*std::cout<<"Before Check Started Status!! " << id << std::endl;
    showq(q);*/
    std::cout<<"Checked started!!" << std::endl;
    bool flag = false;
    queue<tuple <int, double, std::string, double, int> > g = q;
    while (!g.empty())
      {
        // Each element of the priority
        // queue is a tuple itself
        tuple <int, double, std::string, double, int> Tuple = g.front();
        if (get<0>(Tuple) == id)
        {
            flag = true;
        }
        g.pop();
      }
    //std::cout<<"Check Status: " << flag << std::endl;
    //std::cout<<"Checked !!" << std::endl;
    return flag;
}


queue<tuple <int, double, std::string, double, int>> replaceinQ(queue<tuple <int, double, std::string, double, int> > q, tuple <int, double, std::string, double, int> T)
{
    std::cout<<"Replace started!!" << std::endl;
    queue<tuple <int, double, std::string, double, int> > g = q;
    queue<tuple <int, double, std::string, double, int> > gnew;
    int id = get<0>(T);

    while (!g.empty())
          {
            tuple <int, double, std::string, double, int> Tuple = g.front();
            if (get<0>(Tuple) == id)
            {
                gnew.push(T);
            }
            else {
                gnew.push(g.front());
            }
            g.pop();

          }

    return gnew;

    std::cout<<"Replaced!!" << std::endl;
}


queue<tuple <int, double, std::string, double, int>> DeletefromQ(queue<tuple <int, double, std::string, double, int> > q, int id)
{
    //std::cout<<"Delete Function"<<std::endl;

    /*std::cout<<"Before Delete Function Status: "<< id <<std::endl;
    showq(q);*/

    queue<tuple <int, double, std::string, double, int> > g = q;
    queue<tuple <int, double, std::string, double, int> > gnew;
    while (!g.empty())
      {
        tuple <int, double, std::string, double, int> Tuple = g.front();
        if (get<0>(Tuple) != id)
        {
            gnew.push(g.front());
        }
        g.pop();

      }
    //std::cout<<"Deleted!!" << std::endl;

    /*std::cout<<"After Delete Function Status of Queue "<<std::endl;
    showq(gnew);*/

    return gnew;

}



int minIndex(queue<tuple <int, double, std::string, double, int> > q, int sortedIndex)
{
    int min_index = -1;
    int min_val = INT_MAX;
    int n = q.size();
    for (int i=0; i<n; i++)
    {
        tuple <int, double, std::string, double, int> T = q.front();

        double curr = get<1>(T);
        q.pop();
        if (curr <= min_val && i <= sortedIndex)
        {
            min_index = i;
            min_val = curr;
        }

        q.push(T);
    }
    return min_index;
}


queue<tuple <int, double, std::string, double, int> > insertMinToRear(queue<tuple <int, double, std::string, double, int> > q, int min_index)
{

    int min_val;
    tuple <int, double, std::string, double, int> Temp;
    int n = q.size();
    for (int i = 0; i < n; i++)
    {
        tuple <int, double, std::string, double, int> T = q.front();
        double curr =  get<1>(T);
        q.pop();
        if (i != min_index)
        {
            q.push(T);
        }
        else
        {
            min_val = curr;
            Temp = T;
        }

    }
    q.push(Temp);



    return q;
}



queue<tuple <int, double, std::string, double, int> > sortQ(queue<tuple <int, double, std::string, double, int> > q)
{

    for (int i = 1; i <= q.size(); i++)
    {
        int min_index = minIndex(q, q.size() - i);
        q = insertMinToRear(q, min_index);
    }


    //std::cout<<"Sorting done!!!!!!!!!!!!!!!!!!"<<std::endl;

    return q;
}





queue<tuple <int, double, std::string, double, int> >  QueueDeductTimeFromMembers(queue<tuple <int, double, std::string, double, int> > q)
{

    queue<tuple <int, double, std::string, double, int> > g = q;
    queue<tuple <int, double, std::string, double, int> > gnew;
    double TimeToDeduct = 5;
    double MaxWaitTime;

    while (!g.empty())
      {
        tuple <int, double, std::string, double, int> Tuple = g.front();

        tuple <int, double, std::string, double, int> NewTuple;

        double MaxWaitTimeInit = get<1>(Tuple);

        if (MaxWaitTimeInit < TimeToDeduct)
        {
            TimeToDeduct = 0;
        }

        MaxWaitTime = MaxWaitTimeInit - TimeToDeduct;

        NewTuple = make_tuple(get<0>(Tuple), MaxWaitTime,get<2>(Tuple), get<3>(Tuple), get<4>(Tuple));

        gnew.push(NewTuple);

        g.pop();
        TimeToDeduct = 5;

      }

    return gnew;

}


void TraCIDemoRSU11p::QueueHandling()
{

    if (QHigh.empty()==false){
        QHigh = QueueDeductTimeFromMembers(QHigh);
    }
    if (QMid.empty()==false){
        QMid = QueueDeductTimeFromMembers(QMid);
    }
    if (QLow.empty()==false){
        QLow = QueueDeductTimeFromMembers(QLow);
    }


}


tuple <int, double, std::string, double, int> VehilceToServe(queue<tuple <int, double, std::string, double, int> > q){

    tuple <int, double, std::string, double, int> ToServeTuple;
    queue<tuple <int, double, std::string, double, int> > g = q;
    bool NeedService = false;
    while (!g.empty())
          {
            tuple <int, double, std::string, double, int> Tuple = g.front();

            double MaxWaitTime = get<1>(Tuple);
            if (MaxWaitTime<=5)
            {
                ToServeTuple = Tuple;
                NeedService = true;
                break;
            }


            g.pop();

          }
    if (NeedService == false)
    {
        ToServeTuple = make_tuple(0, 0,"No Service Needed", 0,0);
    }


    return ToServeTuple;

}


tuple <int, double, std::string, double, int> VehilceToFirstServe(queue<tuple <int, double, std::string, double, int> > q)
{
    tuple <int, double, std::string, double, int> ToServeTuple;
    queue<tuple <int, double, std::string, double, int> > g = q;
    ToServeTuple = g.front();
    //std::cout<< "Vehicle giving for serve - " << get<0>(ToServeTuple) << std::endl;
    return ToServeTuple;
}


void TraCIDemoRSU11p::ResourceAlllocation()
{
    int VehicleID;
    std::string MessageFromVehilceToServe;
    tuple <int, double, std::string, double, int> TupleHigh;
    tuple <int, double, std::string, double, int> TupleMid;
    tuple <int, double, std::string, double, int> TupleLow;


    if (QHigh.empty()==false){
        TupleHigh = VehilceToFirstServe(QHigh);
        MessageFromVehilceToServe = get<2>(TupleHigh);
        //std::cout<< "vehicle to serve from function - "<< get<0>(TupleHigh) <<std::endl;
        if (MessageFromVehilceToServe != "No Service Needed" && get<0>(TupleHigh)!=0 )
        {
            VehicleID = get<0>(TupleHigh);
            if (RSUBusy == false)
            {
                std::cout<<"~~~~~~~~Vehicle to Service now - "<< VehicleID <<std::endl;
                std::cout<< getParentModule()->getFullPath() << std::endl;
                std::cout<<"~~~~~~~~~~~RSU will be busy for "<< get<4>(TupleHigh) << " FROM TIME - " <<simTime()<<std::endl;
                RSUBusyTime = get<4>(TupleHigh);
                RSUBusy = true;
                std::string RSUName = getParentModule()->getFullPath();
                std::string DeleteFromOther = RSUName + " " + std::to_string(VehicleID);
                //DeleteFromOtherRSUNotification->setName(DeleteFromOther.c_str());
                //scheduleAt(simTime()+uniform(0.01, 0.5), DeleteFromOtherRSUNotification);
                //handleSelfMsg(DeleteFromOtherRSUNotification);

                RSUStatusChangeMessage->setName(RSUName.c_str());
                //std::cout<<get<0>(TupleHigh)<<get<1>(TupleHigh)<<get<2>(TupleHigh)<<get<3>(TupleHigh)<<get<4>(TupleHigh)<<std::endl;
                QHigh = DeletefromQ(QHigh, VehicleID);
                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

//////////////// Which tuple has lower deadline, otherwise, it will always take from higher queue. lower queue will be abandoned.
        }
        }
    if (QMid.empty()==false){
        TupleMid = VehilceToFirstServe(QMid);
        MessageFromVehilceToServe = get<2>(TupleMid);
        //std::cout<< "vehicle to serve from function - "<< get<0>(TupleMid) <<std::endl;
        if (MessageFromVehilceToServe != "No Service Needed" && get<0>(TupleMid)!=0)
        {
            VehicleID = get<0>(TupleMid);
            if (RSUBusy == false)
            {
                std::cout<<"~~~~~~~~~~~Vehicle to Service now - "<< VehicleID <<std::endl;
                std::cout<< getParentModule() << std::endl;
                std::cout<<"~~~~~~~~~~~RSU will be busy for "<< get<4>(TupleMid) << " FROM TIME - " <<simTime()<<std::endl;
                RSUBusyTime = get<4>(TupleMid);
                RSUBusy = true;
                std::string RSUName = getParentModule()->getFullPath();
                RSUStatusChangeMessage->setName(RSUName.c_str());
                //std::cout<<get<0>(TupleMid)<<get<1>(TupleMid)<<get<2>(TupleMid)<<get<3>(TupleMid)<<get<4>(TupleMid)<<std::endl;
                QMid = DeletefromQ(QMid, VehicleID);
                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

        }
        }
    if (QLow.empty()==false){
        TupleLow = VehilceToFirstServe(QLow);
        MessageFromVehilceToServe = get<2>(TupleLow);
        //std::cout<< "vehicle to serve from function - "<< get<0>(TupleLow) <<std::endl;
        if (MessageFromVehilceToServe != "No Service Needed" && get<0>(TupleLow)!=0)
        {
            VehicleID = get<0>(TupleLow);
            if (RSUBusy == false)
            {
                std::cout<<"~~~~~~~~~~Vehicle to Service now - "<< VehicleID <<std::endl;
                std::cout<< getParentModule() << std::endl;
                std::cout<<"~~~~~~~~~~~RSU will be busy for "<< get<4>(TupleLow) << " FROM TIME - " <<simTime()<<std::endl;
                RSUBusyTime = get<4>(TupleLow);
                RSUBusy = true;
                std::string RSUName = getParentModule()->getFullPath();
                RSUStatusChangeMessage->setName(RSUName.c_str());
                std::cout<<get<0>(TupleLow)<<get<1>(TupleLow)<<get<2>(TupleLow)<<get<3>(TupleLow)<<get<3>(TupleLow)<<std::endl;
                QLow = DeletefromQ(QLow, VehicleID);
                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

        }
        }
}


void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{


    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    if (wsm->getKind()==QUEUE_HANDLING || wsm->getKind()==STATUS_CHANGE || wsm->getKind()==DELETE_REQUEST_FROM_RSU)
    {
        return;
    }


    RecievedMessageRSU++;


    /*bool RSUSelfMsg = wsm->getRSUSelfMessage();
    if (RSUSelfMsg == true )
    {
        std::cout<< "Self Message from RSUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU" << std::endl;
        return;
    }
*/
    mac = FindModule<Mac1609_4*>::findSubModule(getParentModule());
    //std::cout<< "time took to reach RSU for " <<wsm->getId() <<"is " <<simTime()<< std::endl;
    //std::cout<< "RSU got message form: " << wsm->getNodeID() << std::endl;
    //std::cout << "Euclidian Distance form RSU and "<< wsm->getNodeID() << " is: "<<sqrt(pow((wsm->getNodeMobilityCoord().x - RSUMobilityX), 2) + pow((wsm->getNodeMobilityCoord().y - RSUMobilityY), 2)) << std::endl
    //std::cout<<"Number of hops for "<< wsm->getId() << " is " <<wsm->getHopCounter()<< std::endl;



    //this rsu repeats the received traffic update in 2 seconds plus some random delay
    //sendDelayedDown(wsm->dup(), 0.2 + uniform(0.01, 0.2));
    // We dont need RSU sending the message for now


    // Finding the RSU Co-ordinate.





    BaseMobility *baseMob;
    baseMob = FindModule<BaseMobility*>::findSubModule(getParentModule());
    Coord rsuCoord = baseMob->getPositionAt(simTime().dbl());
    //std::cout<< "RSU : " << rsuCoord << std::endl;

    Coord vhCoord = wsm->getNodeMobilityCoord();
    //std::cout<< "VH : " << vhCoord << std::endl;

    Coord vhNextCoord = wsm->getNodeMobilityNextCoord();

    int VehicleID = wsm->getNodeID();
    //std::cout<<"Message Reached at << " << simTime() <<std::endl;
    std::cout<<"wsm id: "<< wsm->getId()<< "  Vehicle ID:  "<< VehicleID<<std::endl;
    //std::cout<<"wsm TaskDeadline: "<< wsm->getTaskDeadline()<< "  ServiceTime:  "<< wsm->getServiceTime()<<std::endl;
    float DistancewithRSU = sqrt(pow((vhCoord.x - rsuCoord.x), 2) + pow((vhCoord.y - rsuCoord.y), 2));
    //std::cout<< "Distance : " << DistancewithRSU << std::endl;

    float NextDistancewithRSU = sqrt(pow((vhNextCoord.x  - rsuCoord.x), 2) + pow((vhNextCoord.y - rsuCoord.y), 2));

    float connectionStr = check_and_cast<DeciderResult80211*>(check_and_cast<PhyToMacControlInfo*>(wsm -> getControlInfo()) -> getDeciderResult()) -> getRecvPower_dBm();
    //std::cout<< "connectionStr : " << connectionStr << std::endl;

    //std::cout<< "SenderNodeSpeed : " << wsm->getSenderNodeSpeed() << std::endl;

    bool InRSURange = NextDestinationinRSURange(NextDistancewithRSU, RSUmaxRange);

    bool DistanceType = DefineDistance(DistancewithRSU, RSUmaxRange);
    bool ConnectionType = DefineConnection(connectionStr);


    int SubPriority;
    SubPriority = SubPriotizationFunction(DistanceType, ConnectionType, InRSURange);
    //std::cout<<"SubPriority: " << SubPriority <<std::endl;
    int ST = wsm->getServiceTime();
    int InitDeadline = wsm->getTaskDeadline();
    //double ActualDeadLine = InitDeadline - SimTimetoDoble(simTime());  ---> This is for real time scenario, for result analysis case, we will not decrease simtime.
    double ActualDeadLine = InitDeadline;



    // For calcualtion of threshold time
    if (ActualDeadLine > MaxDeadline)
    {
        MaxDeadline = ActualDeadLine;
    }


    simtime_t MessageInitTime = wsm->getMessageInitTime();
    double MsgInitTime = SimTimetoDoble(MessageInitTime);
    double TimetoReachRSU = SimTimetoDoble(simTime()) - SimTimetoDoble(MessageInitTime) ;
    //std::cout<< "Time took for message to reach : " << TimetoReachRSU << " and hop count = "<< wsm->getHopCounter()<< std::endl;



    // Priority Function


    int SP = SubPriority;
    double Deadline = ActualDeadLine;


    double MaxWaitTimeDouble = Deadline - ST;

    int MaxWaitTime = (int)round(MaxWaitTimeDouble);

    //std::cout<<"MaxWaitTime "<< MaxWaitTime <<std::endl;

    //std::cout<<"InitDeadline "<< InitDeadline << " Actual Deadline: " << Deadline<< " ST: " << ST <<std::endl;

    if (MaxWaitTime < ST)
    {
        std::cout<<"Vehicle- "<<VehicleID<< " ST: " <<ST << " WT: " << MaxWaitTime << "Deadline: " << Deadline<<std::endl;
        std::cout<<"Vehicle- "<<VehicleID<<" 's Request Deadline is already gone, no point now!!!"<<std::endl;
    }
    else if (MaxWaitTime == ST)
    {
        std::cout<<"Vehicle- "<<VehicleID<<" 's Request needs to be Processed now ASAP!!!"<<std::endl;
    }
    else if (MaxWaitTime > ST)
    {

         std::cout<<"Vehicle- "<<VehicleID<<" 's Request Will be put to a queue. "<<std::endl;
         tuple <int, double, std::string, double, int> Task;
         std::string RequestedResource = "RequestedResource";
         tuple <int, double, std::string, double, int> Q;
         Q = make_tuple(VehicleID, MaxWaitTime,RequestedResource, MsgInitTime, ST);
         if (get<0>(Q) != VehicleID)
         {
             //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             std::cout<<"Changing" << std::endl;
             std::get<0>(Q) = VehicleID;
             //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
         }

         if (MaxWaitTime <= MaxDeadline * DeadLinePercetageThreshold)
         {
             std::cout<<" Higher Priority. "<<std::endl;
             //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             if (SP==1)
             {
                 if (QHigh.empty()==false)
                 {
                 bool inQ = checkinQ(QHigh, VehicleID);
                 if (QMid.empty()==false){
                 QMid = DeletefromQ(QMid, VehicleID);
                 QMid = sortQ(QMid);}
                 if (QLow.empty()==false){
                 QLow = DeletefromQ(QLow, VehicleID);
                 QLow = sortQ(QLow);}
                 //std::cout<<" Check value 1: "<<inQ<<std::endl;
                 if (inQ == false){

                     QHigh.push(Q);
                     QHigh = sortQ(QHigh);
                 }
                 else {
                     //std::cout<<" Higher Priority. 1"<<std::endl;
                     QHigh = replaceinQ(QHigh, Q);
                     QHigh = sortQ(QHigh);
                 }
                 }
                 else {
                     QHigh.push(Q);
                 }
             }
             else if (SP==2)
             {
                 if (QMid.empty()==true)
                 {
                     QMid.push(Q);
                 }
                 else{
                 bool inQ = checkinQ(QMid, VehicleID);
                 if (QHigh.empty()==false){
                 QHigh = DeletefromQ(QHigh, VehicleID);
                 QHigh = sortQ(QHigh);}
                 if (QLow.empty()){
                 QLow = DeletefromQ(QLow, VehicleID);
                 QLow = sortQ(QLow);}
                 //std::cout<<" Check value 2: "<<inQ<<std::endl;
                  if (inQ == false){

                      QMid.push(Q);
                      QMid = sortQ(QMid);
                  }
                  else {
                      //std::cout<<" Higher Priority. 2"<<std::endl;
                      QMid = replaceinQ(QMid, Q);
                      QMid = sortQ(QMid);
                  }
                 }
             }
             else if (SP==3)
             {
                 if (QLow.empty()==false){
                 bool inQ = checkinQ(QLow, VehicleID);
                 if (QMid.empty()==false){
                 QMid = DeletefromQ(QMid, VehicleID);
                 QMid = sortQ(QMid);}
                 if (QHigh.empty()==false){
                 QHigh = DeletefromQ(QHigh, VehicleID);
                 QHigh = sortQ(QHigh);}
                 //std::cout<<" Check value 3: "<<inQ<<std::endl;
                  if (inQ == false){
                      QLow.push(Q);
                      QLow = sortQ(QLow);
                  }
                  else {
                      //std::cout<<" Higher Priority. 3"<<std::endl;
                      QLow = replaceinQ(QLow, Q);
                      QLow = sortQ(QLow);
                  }
                 }
                 else{
                     QLow.push(Q);
                 }
             }
         }
         else if (MaxWaitTime > MaxDeadline * DeadLinePercetageThreshold)
         {
             std::cout<<" Lower Priority. "<<std::endl;
             //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             if (SP==1)
              {

                  if (QHigh.size()!= HighPriorityThreshold)
                  {
                      if (QHigh.empty()==false){
                      if (QMid.empty()==false){
                      QMid = DeletefromQ(QMid, VehicleID);
                      QMid = sortQ(QMid);}
                      if (QLow.empty()==false){
                      QLow = DeletefromQ(QLow, VehicleID);
                      QLow = sortQ(QLow);}
                      bool inQ = checkinQ(QHigh, VehicleID);

                      if (inQ == false)
                      {
                          std::cout<<"Did not find in queue" <<std::endl;
                          QHigh.push(Q);
                          QHigh = sortQ(QHigh);
                      }
                      else {
                          std::cout<<"Found in queue" <<std::endl;
                          QHigh = replaceinQ(QHigh, Q);
                          QHigh = sortQ(QHigh);
                      }
                      }
                      else {
                          QHigh.push(Q);
                      }
                  }
                  else {
                  if (QMid.empty()==false){
                  if (QHigh.empty()==false){
                  QHigh = DeletefromQ(QHigh, VehicleID);
                  QHigh = sortQ(QHigh);}
                  if (QLow.empty()==false){
                  QLow = DeletefromQ(QLow, VehicleID);
                  QLow = sortQ(QLow);}
                  bool inQ = checkinQ(QMid, VehicleID);
                    if (inQ == false)
                    {
                        QMid.push(Q);
                        QMid = sortQ(QMid);
                    }
                    else {
                        QMid = replaceinQ(QMid, Q);
                        QMid = sortQ(QMid);
                    }
                  }
                  else {
                      QMid.push(Q);
                  }
                  }
              }
              else if (SP==2)
              {

                  if (QMid.size()!= HighPriorityThreshold)
                    {
                      if (QMid.empty()==false){
                      if (QHigh.empty()==false){
                      QHigh = DeletefromQ(QHigh, VehicleID);
                      QHigh = sortQ(QHigh);}
                      if (QLow.empty()==false){
                      QLow = DeletefromQ(QLow, VehicleID);
                      QLow = sortQ(QLow);}

                      bool inQ = checkinQ(QMid, VehicleID);
                      if (inQ == false)
                      {
                          QMid.push(Q);
                          QMid = sortQ(QMid);
                      }
                      else {
                          QMid = replaceinQ(QMid, Q);
                          QMid = sortQ(QMid);
                      }
                      }
                      else {
                          QMid.push(Q);
                      }
                    }
                    else {
                        if (QLow.empty()==false){
                          if (QHigh.empty()==false){
                          QHigh = DeletefromQ(QHigh, VehicleID);
                          QHigh = sortQ(QHigh);}
                          if (QMid.empty()==false){
                          QMid = DeletefromQ(QMid, VehicleID);
                          QMid = sortQ(QMid);}

                          bool inQ = checkinQ(QLow, VehicleID);
                          if (inQ == false)
                          {
                              QLow.push(Q);
                              QLow = sortQ(QLow);
                          }
                          else {
                              QLow = replaceinQ(QLow, Q);
                              QLow = sortQ(QLow);
                          }
                        }
                        else {
                            QLow.push(Q);
                        }
                    }
                    }
              else if (SP==3)
              {
                  if (QLow.empty()==false){
                  if (QHigh.empty()==false){
                  QHigh = DeletefromQ(QHigh, VehicleID);
                  QHigh = sortQ(QHigh);}
                  if (QMid.empty()==false){
                  QMid = DeletefromQ(QMid, VehicleID);
                  QMid = sortQ(QMid);}

                  bool inQ = checkinQ(QLow, VehicleID);
                if (inQ == false)
                {
                    QLow.push(Q);
                    QLow = sortQ(QLow);
                }
                else {
                    QLow = replaceinQ(QLow, Q);
                    QLow = sortQ(QLow);
                }
                }
              }
         }
    }

    /*if (RunThread == true)
    {
        RunThread = false;
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        wsm->setRSUSelfMessage(true);
        std::cout<<"Self message initiated "<<std::endl;
        //scheduleAt(simTime() + 5, wsm);

        //sendDown(wsm);
    }*/



    std::cout<<"ID: " << getParentModule()->getFullPath()<<std::endl;
    std::cout<<"QHigh: ";
    showq(QHigh);
    std::cout<<"QMid: ";
    showq(QMid);
    std::cout<<"QLow: ";
    showq(QLow);
    std::cout<<std::endl;



}







void TraCIDemoRSU11p::handleSelfMsg(cMessage* msg)
{

    //std::cout<< "MESSAGE TYPE::: " << msg->getKind() << " from RSU ::::" << getParentModule()->getFullPath() << std::endl;

    if (msg->getKind() == QUEUE_HANDLING)
    {
        //std::cout<<"QUEUE_HANDLING" << std::endl;
        //std::cout<<"ID: " << getParentModule()->getFullPath()<<std::endl;
        QueueHandling();
        ResourceAlllocation();
        scheduleAt(simTime()+5+uniform(0.05, 1), QueueHandlingMessage);
    }
    if (msg->getKind() == STATUS_CHANGE)
    {
        //std::cout<< "STATUS_CHANGE Start at "<< simTime() << std::endl;
        //std::cout<< msg->getName()<<std::endl;
        std::string RSUNAME = msg->getName();
        //std::cout<< RSUNAME<<std::endl;
        if (RSUNAME == "Initialize")
        {
            std::cout<<"STATUS_CHANGE_RETURN"<<std::endl;
            return;
        }
        if (RSUNAME == getParentModule()->getFullPath())
        {
        RSUBusy=false;
        RSUBusyTime=0;
        std::cout<<RSUNAME<< " Is free now!!!!!!!!!!!!!!! at " <<simTime()<<std::endl;

        }
        //std::cout<< "STATUS_CHANGE End"<<std::endl;
    }

    if (msg->getKind() == DELETE_REQUEST_FROM_RSU)
    {

        std::string MessageBody = msg->getName();

        if (MessageBody == "Initialize")
        {
        std::cout<<"DELETE_REQUEST_FROM_RSU_RETURN"<<std::endl;
        return;
        }


        std::string RSUNAME = MessageBody.substr(0, MessageBody.find(" "));
        std::string VehicleIDStr = MessageBody.substr(MessageBody.find(" ") + 1);
        int VehicleID = std::stoi(VehicleIDStr);
        std::cout << "DELETE REQUEST FROM OTHER RSU >>> " <<RSUNAME << " " << getParentModule()->getFullPath() << " " << VehicleID << std::endl;
        scheduleAt(simTime()+5+uniform(0.05, 1), msg);
        //if (RSUNAME != getParentModule()->getFullPath())





    }
}



void TraCIDemoRSU11p::finish(){
    //std::cout<<"Finished "<< std::endl;
    //printlist(NodeWithMessage);
    DemoBaseApplLayer::finish();

    double MessageCount = MessageCountRSU;
    double RecievedMessage = RecievedMessageRSU;

    std::cout<<"ID: " << getParentModule()->getFullPath()<<std::endl;
    std::cout<<"RecievedMessage: " << RecievedMessageRSU<<std::endl;



        std::cout<<"QHigh: ";
        showq(QHigh);
        std::cout<<"QMid: ";
        showq(QMid);
        std::cout<<"QLow: ";
        showq(QLow);
        std::cout<<std::endl;




}
