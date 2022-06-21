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


#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>
#include <map>
#include <vector>
#include <iterator>
#include <list>

using namespace std;


using namespace veins;

using namespace constants;

Define_Module(veins::TraCIDemoRSU11p);



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


void showq(queue<tuple<int, double, std::string> > gq)
{
    queue<tuple<int, double, std::string> > g = gq;
    while (!g.empty()) {
        tuple<int, double, std::string> Tuple = g.front();
        std::cout << '\t' << get<0>(Tuple) << '\t' << get<1>(Tuple) <<'\t' << get<2>(Tuple);
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



bool checkinQ(queue<tuple<int, double, std::string> > q, int id)
{

    std::cout<<"Checked started!!" << std::endl;
    bool flag = false;
    queue<tuple<int, double, std::string> > g = q;
    while (!g.empty())
      {
        // Each element of the priority
        // queue is a tuple itself
        tuple<int, double, std::string> Tuple = g.front();
        if (get<0>(Tuple) == id)
        {
            flag = true;
        }
        g.pop();
      }
    //std::cout<<"Checked !!" << std::endl;
    return flag;
}


queue<tuple<int, double, std::string>> replaceinQ(queue<tuple<int, double, std::string> > q, tuple<int, double, std::string> T)
{
    std::cout<<"Replace started!!" << std::endl;
    queue<tuple<int, double, std::string> > g = q;
    queue<tuple<int, double, std::string> > gnew = q;
    int id = get<0>(T);

    while (!g.empty())
          {
            tuple<int, double, std::string> Tuple = g.front();
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


queue<tuple<int, double, std::string>> DeletefromQ(queue<tuple<int, double, std::string> > q, int id)
{
    std::cout<<"Delete Function"<<std::endl;
    queue<tuple<int, double, std::string> > g = q;
    queue<tuple<int, double, std::string> > gnew = q;
    while (!g.empty())
      {
        tuple<int, double, std::string> Tuple = g.front();
        if (get<0>(Tuple) != id)
        {
            gnew.push(g.front());
        }
        g.pop();

      }
    //std::cout<<"Deleted!!" << std::endl;

    return gnew;

}



int minIndex(queue<tuple<int, double, std::string> > q, int sortedIndex)
{
    int min_index = -1;
    int min_val = INT_MAX;
    int n = q.size();
    for (int i=0; i<n; i++)
    {
        tuple<int, double, std::string> T = q.front();

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


queue<tuple<int, double, std::string> > insertMinToRear(queue<tuple<int, double, std::string> > q, int min_index)
{

    int min_val;
    tuple<int, double, std::string> Temp;
    int n = q.size();
    for (int i = 0; i < n; i++)
    {
        tuple<int, double, std::string> T = q.front();
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



queue<tuple<int, double, std::string> > sortQ(queue<tuple<int, double, std::string> > q)
{

    for (int i = 1; i <= q.size(); i++)
    {
        int min_index = minIndex(q, q.size() - i);
        q = insertMinToRear(q, min_index);
    }


    //std::cout<<"Sorting done!!!!!!!!!!!!!!!!!!"<<std::endl;

    return q;
}




void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
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
    double ActualDeadLine = InitDeadline - SimTimetoDoble(simTime());


    // For calcualtion of threshold time
    if (ActualDeadLine > MaxDeadline)
    {
        MaxDeadline = ActualDeadLine;
    }


    simtime_t MessageInitTime = wsm->getMessageInitTime();
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
        std::cout<<"Vehicle- "<<VehicleID<<" 's Request Deadline is already gone, no point now!!!"<<std::endl;
    }
    else if (MaxWaitTime == ST)
    {
        std::cout<<"Vehicle- "<<VehicleID<<" 's Request needs to be Processed now ASAP!!!"<<std::endl;
    }
    else if (MaxWaitTime > ST)
    {

         std::cout<<"Vehicle- "<<VehicleID<<" 's Request Will be put to a queue. "<<std::endl;
         tuple <int, double, std::string> Task;
         std::string RequestedResource = "RequestedResource";
         tuple <char, double, std::string> Q;
         Q = make_tuple(VehicleID, MaxWaitTime,RequestedResource);
         if (get<0>(Q) != VehicleID)
         {
             std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             std::cout<<"Changing" << std::endl;
             std::get<0>(Q) = VehicleID;
             std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
         }

         if (MaxWaitTime <= MaxDeadline * DeadLinePercetageThreshold)
         {
             std::cout<<" Higher Priority. "<<std::endl;
             std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
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
             std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
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
                          QHigh.push(Q);
                          QHigh = sortQ(QHigh);
                      }
                      else {
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


    std::cout<<"ID: " << getParentModule()<<std::endl;
    std::cout<<"QHigh: ";
    showq(QHigh);
    std::cout<<"QMid: ";
    showq(QMid);
    std::cout<<"QLow: ";
    showq(QLow);
    std::cout<<std::endl;


}


void TraCIDemoRSU11p::finish(){
    //std::cout<<"Finished "<< std::endl;
    //printlist(NodeWithMessage);
    DemoBaseApplLayer::finish();

}
