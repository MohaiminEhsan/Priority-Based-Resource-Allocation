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
//#include "cconfiguration.h"

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
#include <stdio.h>
#include <fstream>

using namespace std;


using namespace veins;

using namespace constants;

Define_Module(veins::TraCIDemoRSU11p);


//Result Analysis
std::string varNameRSU;
double valueRSU;
double MessageCountRSU;
double RecievedMessageRSU=0;

///////////////////////// Fuzzification
class CFuzzyFunction
{
protected :
    double dLeft, dRight;
    char   cType;
    char*  sName;

public:
    CFuzzyFunction(){};
    virtual ~CFuzzyFunction(){ delete [] sName; sName=NULL;}

    virtual void    setInterval(double l,   double r)
    {dLeft=l; dRight=r;}

    virtual void    setMiddle( double dL=0, double dR=0)=0;

    virtual void    setType(char c)
    { cType=c;}

    virtual void    setName(const char* s)
    {
      sName = new char[strlen(s)+1];
      strcpy(sName,s);
    }

    bool    isDotInInterval(double t)
    {
        if((t>=dLeft)&&(t<=dRight)) return true; else return false;
    }

    char getType(void)const{ return cType;}

        void    getName() const
    {
        cout<<sName<<endl;
    }

    virtual double getValue(double t)=0;
};



class CTrapezoid : public CFuzzyFunction
{
private:
    double dLeftMiddle, dRightMiddle;

public:
    void    setMiddle(double dL, double dR)
    {
        dLeftMiddle=dL; dRightMiddle=dR;
    }

    double  getValue(double t)
    {
        if(t<=dLeft)
           return 0;
        else if(t<dLeftMiddle)
            return (t-dLeft)/(dLeftMiddle-dLeft);
        else if(t<=dRightMiddle)
            return 1.0;
        else if(t<dRight)
            return (dRight-t)/(dRight-dRightMiddle);
        else
            return 0;
    }
};





void TraCIDemoRSU11p::initialize(int stage){
    DemoBaseApplLayer::initialize(stage);
    std::ofstream outfile ("Distance.txt");
    outfile.close();
    std::ofstream Coutfile ("Connection.txt");
    Coutfile.close();
    std::ofstream Timeoutfile ("Time.txt");
    Timeoutfile.close();

    if (stage == 0)
    {
        QueueHandlingMessage = new cMessage("Queue Handling Message", QUEUE_HANDLING);
        RSUStatusChangeMessage = new cMessage("RSU Status Change Message", STATUS_CHANGE);
        RSUStatusChangeMessageFuzzy = new cMessage("RSU Status Change Message Fuzzy", STATUS_CHANGE_FUZZY);
        //DeleteFromOtherRSUNotification = new cMessage ("Delete a request from other RSU", DELETE_REQUEST_FROM_RSU);
        RSUStatusChangeMessage->setName("Initialize");
        //DeleteFromOtherRSUNotification->setName("Initialize");
        if (RSUStatusChangeMessage->isScheduled())
        {
            cancelEvent(RSUStatusChangeMessage);
            scheduleAt(simTime()+uniform(0.05, 1), RSUStatusChangeMessage);

        }
        else{
            scheduleAt(simTime()+uniform(0.05, 1), RSUStatusChangeMessage);
        }

        if (RSUStatusChangeMessageFuzzy->isScheduled())
        {
                    cancelEvent(RSUStatusChangeMessageFuzzy);
                    scheduleAt(simTime()+uniform(0.05, 1), RSUStatusChangeMessageFuzzy);

        }
        else{
                    scheduleAt(simTime()+uniform(0.05, 1), RSUStatusChangeMessageFuzzy);
        }


        if (QueueHandlingMessage->isScheduled())
        {
                    cancelEvent(QueueHandlingMessage);
                    scheduleAt(simTime()+uniform(0.05, 1), QueueHandlingMessage);

        }
        else{
                    scheduleAt(simTime()+uniform(0.05, 1), QueueHandlingMessage);
        }

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



/////Fuzzy
int FuzzyAlgo(float Distance, double rsuRange)
{
    double dmid=rsuRange/2;
    double dlend=(0.6666*rsuRange);
    double drstart=(0.3333*rsuRange);
    CFuzzyFunction *FuzzySet[2];
    FuzzySet[0] = new CTrapezoid;
    FuzzySet[1] = new CTrapezoid;
    //FuzzySet[2] = new CTrapezoid;

    FuzzySet[0]->setInterval(1,500);
    FuzzySet[0]->setMiddle(1,dlend);
    //FuzzySet[0]->setType('r');
    FuzzySet[0]->setName("good_distance");

    FuzzySet[1]->setInterval(dmid,500);
    FuzzySet[1]->setMiddle(drstart,500);
    //FuzzySet[1]->setType('r');
    FuzzySet[1]->setName("bad_distance");
    return 0;
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


float DefineDistance2(float Distance , double rsuRange)
{

    float SP;
    float NextDistancewithRSU;
    std::list<float> DistanceList;
    std::string line;
    std::string DistanceFileName("Distance.txt");
    ofstream DistanceFileOut;
    std::ifstream DistanceFileIn("Distance.txt");

    DistanceFileOut.open(DistanceFileName, std::ios_base::app);

    if (DistanceFileOut.is_open()){
        DistanceFileOut << Distance << endl;

    }

    if (DistanceFileIn.is_open())
        {

        while(std::getline(DistanceFileIn, line))
        {
            DistanceList.push_back(std::stoi(line));
        }
        DistanceFileIn.close();
        }
    float DistanceMean;
    float avg = 0;
    std::list<float>::const_iterator it;
    for(it = DistanceList.begin(); it != DistanceList.end(); it++) avg += *it;
    avg /= DistanceList.size();

    DistanceMean = avg;
    float c= (DistanceMean + rsuRange)/2;

    float d1 = Distance - DistanceMean;
    float d2 = rsuRange - DistanceMean;
    float d = pow((d1/d2),2);

    if (Distance <= DistanceMean)
    {
        SP = 1;
    }
    else if (Distance > DistanceMean && Distance <= c)
    {
        SP = 1- 2*d;
    }
    else if (Distance > c && Distance < rsuRange){
        SP = 2*d;
    }
    else if (Distance>=rsuRange)
    {
        SP=0;
    }

    DistanceFileOut.close();

    std::cout<<"SPD: " <<SP<<std::endl;
    return SP;
}


bool DefineConnection(float connectionStr )
{
    float ConnectionT = 80;
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


float DefineTime2(int time)
{
    float mean;
    std::list<float> TimeList;
    std::string line;
    std::string FileName("Time.txt");
    ofstream FileOut;
    std::ifstream FileIn("Time.txt");

    FileOut.open(FileName, std::ios_base::app);

    if (FileOut.is_open()){
        FileOut << time << endl;

    }

    if (FileIn.is_open())
    {

    while(std::getline(FileIn, line))
    {
        TimeList.push_back(std::stoi(line));
    }
    FileIn.close();
    }

    float avg = 0;
    std::list<float>::const_iterator it;
    for(it = TimeList.begin(); it != TimeList.end(); it++) avg += *it;
    avg /= TimeList.size();

    mean = avg;
    std::cout<< "TIME MEAN : " << mean<<std::endl;
    return mean;

}

float DefineConnection2(float connectionStr )
{
        float SP;
        std::list<float> ConnectionList;
        std::string line;
        std::string FileName("Connection.txt");
        ofstream FileOut;
        std::ifstream FileIn("Connection.txt");

        FileOut.open(FileName, std::ios_base::app);

        if (FileOut.is_open()){
            FileOut << connectionStr << endl;

        }

        if (FileIn.is_open())
            {

            while(std::getline(FileIn, line))
            {
                ConnectionList.push_back(std::stoi(line));
            }
            FileIn.close();
            }
        float ConnectionMean;
        float avg = 0;
        std::list<float>::const_iterator it;
        for(it = ConnectionList.begin(); it != ConnectionList.end(); it++) avg += *it;
        avg /= ConnectionList.size();

        ConnectionMean = avg;
        float standardDeviation;

        for(it = ConnectionList.begin(); it != ConnectionList.end(); it++) {
            standardDeviation += pow(*it - avg, 2);
          }

        float SDC = sqrt(standardDeviation / ConnectionList.size());

        float c1 = connectionStr - ConnectionMean;
        float c11 = pow(c1,2);
        float c2 = pow(SDC,2);
        float c22 = 2*c2;
        float c =-c11/c22;
        SP =exp(c);

        std::cout<<"SPC: " <<SP<<std::endl;

        FileOut.close();

        return SP;

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

void showqFuzzy(queue<tuple <int, double, std::string, double, int, float> > gq)
{
    queue<tuple <int, double, std::string, double, int, float> > g = gq;
    while (!g.empty()) {
        tuple <int, double, std::string, double, int, float> Tuple = g.front();
        std::cout << '\t' << get<0>(Tuple) << '\t' << get<1>(Tuple) <<'\t' << get<2>(Tuple) << '\t' << get<3>(Tuple)<< '\t'<< get<4>(Tuple)<< '\t' << get<5>(Tuple) <<"---->";
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


bool checkinQFuzzy(queue<tuple <int, double, std::string, double, int,float> > q, int id)
{
    /*std::cout<<"Before Check Started Status!! " << id << std::endl;
    showq(q);*/
    std::cout<<"Checked started!!" << std::endl;
    bool flag = false;
    queue<tuple <int, double, std::string, double, int, float> > g = q;
    while (!g.empty())
      {
        // Each element of the priority
        // queue is a tuple itself
        tuple <int, double, std::string, double, int, float> Tuple = g.front();
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

queue<tuple <int, double, std::string, double, int, float>> replaceinQFuzzy(queue<tuple <int, double, std::string, double, int, float> > q, tuple <int, double, std::string, double, int,float> T)
{
    std::cout<<"Replace started!!" << std::endl;
    queue<tuple <int, double, std::string, double, int, float> > g = q;
    queue<tuple <int, double, std::string, double, int, float> > gnew;
    int id = get<0>(T);

    while (!g.empty())
          {
            tuple <int, double, std::string, double, int, float> Tuple = g.front();
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


queue<tuple <int, double, std::string, double, int, float>> DeletefromQFuzzy(queue<tuple <int, double, std::string, double, int, float> > q, int id)
{
    //std::cout<<"Delete Function"<<std::endl;

    /*std::cout<<"Before Delete Function Status: "<< id <<std::endl;
    showq(q);*/

    queue<tuple <int, double, std::string, double, int,float> > g = q;
    queue<tuple <int, double, std::string, double, int,float> > gnew;
    while (!g.empty())
      {
        tuple <int, double, std::string, double, int, float> Tuple = g.front();
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

int minIndexFuzzy(queue<tuple <int, double, std::string, double, int, float> > q, int sortedIndex)
{
    int min_index = -1;
    int min_val = INT_MAX;
    int n = q.size();
    for (int i=0; i<n; i++)
    {
        tuple <int, double, std::string, double, int, float> T = q.front();

        double curr = get<5>(T);
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


queue<tuple <int, double, std::string, double, int,float> > insertMinToRearFuzzy(queue<tuple <int, double, std::string, double, int, float> > q, int min_index)
{

    int min_val;
    tuple <int, double, std::string, double, int, float> Temp;
    int n = q.size();
    for (int i = 0; i < n; i++)
    {
        tuple <int, double, std::string, double, int, float> T = q.front();
        double curr =  get<5>(T);
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


queue<tuple <int, double, std::string, double, int, float> > reverseQueue(queue<tuple <int, double, std::string, double, int, float> > queue) {
    int n = queue.size();
    std::stack<tuple <int, double, std::string, double, int, float>> st;

    // Remove all the elements from queue and push them to stack
    for (int i = 0; i < n; i++) {
        tuple <int, double, std::string, double, int,float> curr = queue.front();
        queue.pop();
        st.push(curr);
    }

    // Pop out elements from the stack and push them back to queue
    for (int i = 0; i < n; i++) {
        tuple <int, double, std::string, double, int,float> curr = st.top();
        st.pop();
        queue.push(curr);
    }

    // Print the reversed queue
    for (int i = 0; i < n; i++) {
        tuple <int, double, std::string, double, int,float> curr = queue.front();
        queue.pop();
        queue.push(curr);
    }

    return queue;

}



queue<tuple <int, double, std::string, double, int, float> > sortQFuzzy(queue<tuple <int, double, std::string, double, int, float> > q)
{

    for (int i = 1; i <= q.size(); i++)
    {
        int min_index = minIndexFuzzy(q, q.size() - i);
        q = insertMinToRearFuzzy(q, min_index);
    }


    q = reverseQueue(q);


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



queue<tuple <int, double, std::string, double, int, float> >  QueueDeductTimeFromMembersFuzzy(queue<tuple <int, double, std::string, double, int, float> > q)
{

    queue<tuple <int, double, std::string, double, int, float> > g = q;
    queue<tuple <int, double, std::string, double, int, float> > gnew;
    double TimeToDeduct = 5;
    double MaxWaitTime;

    while (!g.empty())
      {
        tuple <int, double, std::string, double, int, float> Tuple = g.front();

        tuple <int, double, std::string, double, int, float> NewTuple;

        double MaxWaitTimeInit = get<1>(Tuple);

        if (MaxWaitTimeInit < TimeToDeduct)
        {
            TimeToDeduct = 0;
        }

        MaxWaitTime = MaxWaitTimeInit - TimeToDeduct;

        NewTuple = make_tuple(get<0>(Tuple), MaxWaitTime,get<2>(Tuple), get<3>(Tuple), get<4>(Tuple), get<5>(Tuple));

        gnew.push(NewTuple);

        g.pop();
        TimeToDeduct = 5;

      }

    return gnew;

}



void TraCIDemoRSU11p::QueueHandling()
{
    std::list<int> AlreadyExecutedRequest;
    std::list<int> AlreadyExecutedRequestFuzzy;

    ifstream Vrequestfile ("RSU.txt");
    ifstream VrequestfileFuzzy ("RSUFuzzy.txt");
    std::string line;
    std::string lineFuzzy;
    int VehicleID;
    int VehicleIDFuzzy;

    if (Vrequestfile.is_open())
    {
                for(line; getline(Vrequestfile, line);){

                    AlreadyExecutedRequest.push_back(std::stoi(line));
                }

                Vrequestfile.close();
    }

    if (VrequestfileFuzzy.is_open())
        {
                    for(lineFuzzy; getline(VrequestfileFuzzy, lineFuzzy);){

                        AlreadyExecutedRequestFuzzy.push_back(std::stoi(lineFuzzy));
                    }

                    VrequestfileFuzzy.close();
        }




    if (QHigh.empty()==false){
        for (auto const& i : AlreadyExecutedRequest) {
            VehicleID = i;
            }
        //std::cout<<"Delete from " <<getParentModule()->getFullPath() << " " << VehicleID << std::endl;
        QHigh = DeletefromQ(QHigh, VehicleID);
        QHigh = QueueDeductTimeFromMembers(QHigh);
    }
    if (QMid.empty()==false){
        for (auto const& i : AlreadyExecutedRequest) {
                VehicleID = i;
                }
        QMid = DeletefromQ(QMid, VehicleID);
        QMid = QueueDeductTimeFromMembers(QMid);
    }
    if (QLow.empty()==false){
        for (auto const& i : AlreadyExecutedRequest) {
                VehicleID = i;
                }
        QLow = DeletefromQ(QLow, VehicleID);
        QLow = QueueDeductTimeFromMembers(QLow);
    }



    if (QHigh_Fuzzy.empty()==false)
    {
        for (auto const& i : AlreadyExecutedRequestFuzzy) {
            VehicleIDFuzzy = i;
            }
        QHigh_Fuzzy = DeletefromQFuzzy(QHigh_Fuzzy, VehicleIDFuzzy);
        QHigh_Fuzzy = QueueDeductTimeFromMembersFuzzy(QHigh_Fuzzy);
    }
    if (QLow_Fuzzy.empty()==false)
    {
        for (auto const& i : AlreadyExecutedRequestFuzzy) {
            VehicleIDFuzzy = i;
            }
        QLow_Fuzzy = DeletefromQFuzzy(QLow_Fuzzy, VehicleIDFuzzy);
        QLow_Fuzzy = QueueDeductTimeFromMembersFuzzy(QLow_Fuzzy);
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


double FindMin(double x, double y, double z){
  return x < y ? (x < z ? x : z) : (y < z ? y : z);
}



tuple <int, double, std::string, double, int, float> QueueCheckingHighFuzzy(queue<tuple<int, double, std::string, double, int, float> >QHigh)
{
    tuple <int, double, std::string, double, int, float> T;
    T = make_tuple(0, 0.0,"No", 0.0, 0.0, 0.0);

    queue<tuple<int, double, std::string, double, int, float> >Q = QHigh;

    tuple <int, double, std::string, double, int, float> Temp;
    std::string Flag = "No";

    if (QHigh.empty()==false)
    {
        for (int i=0; i<QHigh.size(); i++)
            {
                Temp = Q.front();

                double curr = get<1>(Temp);
                Q.pop();

                if (curr == get<4>(Temp))
                {
                    Flag = "Yes";
                    break;
                }

            }

        if (Flag == "Yes"){
            return Temp;
        }
        else {
            return T;
        }
    }
    else {
        return T;
    }

}


tuple <int, double, std::string, double, int, float> QueueCheckingLowFuzzy(queue<tuple<int, double, std::string, double, int, float> >QLow, float th_mean)
{
    tuple <int, double, std::string, double, int, float> T;
    T = make_tuple(0, 0.0,"No", 0.0, 0.0, 0.0);
    queue<tuple<int, double, std::string, double, int, float> >Q = QLow;
    tuple <int, double, std::string, double, int, float> Temp;
    std::string Flag = "No";
    if (QLow.empty()==false)
        {
            for (int i=0; i<QLow.size(); i++)
                {
                    Temp = Q.front();

                    double curr = get<1>(Temp);
                    Q.pop();

                    if (curr <= th_mean)
                    {
                        Flag = "Yes";
                        break;
                    }

                }

            if (Flag == "Yes"){
                return Temp;
            }
            else {
                return T;
            }
        }
        else {
            return T;
        }

}

std::string QueueWithLowDeadline(queue<tuple<int, double, std::string, double, int> >QHigh, queue<tuple<int, double, std::string, double, int> > QMid, queue<tuple<int, double, std::string, double, int> > QLow)
{

    double DeadLineHigh;
    double DeadLineMid;
    double DeadLineLow;
    tuple <int, double, std::string, double, int> TupleHigh;
    tuple <int, double, std::string, double, int> TupleMid;
    tuple <int, double, std::string, double, int> TupleLow;


    if (QHigh.empty()==false)
    {
    TupleHigh = VehilceToFirstServe(QHigh);
    DeadLineHigh = get<1>(TupleHigh);
    }
    else
    {
        DeadLineHigh = 9999999999;
    }

    if (QMid.empty()==false)
        {
        TupleMid = VehilceToFirstServe(QMid);
        DeadLineMid = get<1>(TupleMid);
        }
        else
        {
            DeadLineMid = 9999999999;
        }

    if (QLow.empty()==false)
        {
        TupleLow = VehilceToFirstServe(QLow);
        DeadLineLow = get<1>(TupleLow);
        }
        else
        {
            DeadLineLow = 9999999999;
        }

    double DeadLineMin = FindMin(DeadLineHigh, DeadLineMid, DeadLineLow);

    std::string Queue= "";

    if (DeadLineHigh == DeadLineMin)
    {
        Queue = "High";
    }
    else if (DeadLineMid == DeadLineMin)
    {
        Queue = "Mid";
    }
    else if (DeadLineLow == DeadLineMin)
    {
        Queue = "Low";
    }

    return Queue;

}










void TraCIDemoRSU11p::ResourceAlllocation()
{
    int VehicleID;
    std::string RSUfilename("RSU.txt");
    std::string RSUFuzzyfilename("RSUFuzzy.txt");
    ofstream RSUFileOut;
    ofstream RSUFuzzyFileOut;
    RSUFileOut.open(RSUfilename, std::ios_base::app);
    RSUFuzzyFileOut.open(RSUFuzzyfilename, std::ios_base::app);
    std::string MessageFromVehilceToServe;
    tuple <int, double, std::string, double, int> TupleHigh;
    tuple <int, double, std::string, double, int> TupleMid;
    tuple <int, double, std::string, double, int> TupleLow;


    tuple <int, double, std::string, double, int, float> TupleHighFuzzy;
    tuple <int, double, std::string, double, int, float> TupleLowFuzzy;



    //Fuzzy Based Prioratization Function
    //Start


    tuple <int, double, std::string, double, int, float> ServeNowHighFuzzy = QueueCheckingHighFuzzy(QHigh_Fuzzy);
    std::string NeedServingFuzzyNow = get<2>(ServeNowHighFuzzy);
    if (NeedServingFuzzyNow != "No" && get<0>(ServeNowHighFuzzy)!=0)
    {
        VehicleID = get<0>(ServeNowHighFuzzy);
        std::cout<<"~~~~~~~~Vehicle to Service now Fuzzy - "<< VehicleID <<std::endl;
        std::cout<< getParentModule()->getFullPath() << std::endl;
        std::cout<<"~~~~~~~~~~~RSU Fuzzy will be busy for "<< get<4>(ServeNowHighFuzzy) << " FROM TIME - " <<simTime()<<std::endl;

        RSUBusyTimeFuzzy = get<4>(ServeNowHighFuzzy);
        //RSUBusyFuzzy = true;
        std::string RSUName = getParentModule()->getFullPath();
        RSUStatusChangeMessageFuzzy->setName(RSUName.c_str());
        QHigh_Fuzzy = DeletefromQFuzzy(QHigh_Fuzzy, VehicleID);
        if (QLow_Fuzzy.empty()==false)
        {
            TupleLowFuzzy = QLow_Fuzzy.front();
            QLow_Fuzzy.pop();
            QHigh_Fuzzy.push(TupleLowFuzzy);
            QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);
        }
        if (RSUFuzzyFileOut.is_open())
        {
           std::string VehicleIDStr =  std::to_string(VehicleID);
           RSUFuzzyFileOut << VehicleIDStr << endl;
        }
        //scheduleAt(simTime()+RSUBusyTimeFuzzy+uniform(0.05, 1), RSUStatusChangeMessageFuzzy);
    }
    else if (NeedServingFuzzyNow == "No")
    {
        if (QHigh_Fuzzy.empty()==false){
            TupleHighFuzzy = QHigh_Fuzzy.front();

            if (RSUBusyFuzzy==false){
                RSUBusyTimeFuzzy = get<4>(TupleHighFuzzy);
                RSUBusyFuzzy = true;
                std::string RSUName = getParentModule()->getFullPath();
                RSUStatusChangeMessageFuzzy->setName(RSUName.c_str());
                VehicleID = get<0>(TupleHighFuzzy);
                std::cout<<"~~~~~~~~Vehicle to Service now Fuzzy - "<< VehicleID <<std::endl;
                std::cout<< getParentModule()->getFullPath() << std::endl;
                std::cout<<"~~~~~~~~~~~RSU Fuzzy will be busy for "<< get<4>(TupleHighFuzzy) << " FROM TIME - " <<simTime()<<std::endl;
                QHigh_Fuzzy = DeletefromQFuzzy(QHigh_Fuzzy, VehicleID);
                scheduleAt(simTime()+RSUBusyTimeFuzzy+uniform(0.05, 1), RSUStatusChangeMessageFuzzy);


            }
            if (QLow_Fuzzy.empty()==false)
            {
                TupleLowFuzzy = QLow_Fuzzy.front();
                QLow_Fuzzy.pop();
                QHigh_Fuzzy.push(TupleLowFuzzy);
                QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);
            }

            if (RSUFuzzyFileOut.is_open())
            {
               std::string VehicleIDStr =  std::to_string(VehicleID);
               RSUFuzzyFileOut << VehicleIDStr << endl;
            }


        }
//        else if (QHigh_Fuzzy.empty()==true){
//            if (QLow_Fuzzy.empty()==false)
//            {
//                TupleLowFuzzy = QLow_Fuzzy.front();
//                QLow_Fuzzy.pop();
//                QHigh_Fuzzy.push(TupleLowFuzzy);
//
//            }
//        }

        TupleLowFuzzy = QueueCheckingLowFuzzy(QLow_Fuzzy,TimeFuzzyThreshold);
        std::string NeedTrasnferFuzzyNow = get<2>(TupleLowFuzzy);
        if (NeedTrasnferFuzzyNow!="No" && get<0>(TupleLowFuzzy)!=0){
            VehicleID = get<0>(TupleLowFuzzy);
            QLow_Fuzzy = DeletefromQFuzzy(QLow_Fuzzy, VehicleID);
            QHigh_Fuzzy.push(TupleLowFuzzy);
            QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);
        }

    }

    RSUFuzzyFileOut.close();

    //Fuzzy Based Prioratization Function
    //End



    // Static Threshold Prioratization
    //Start

    std::string QueueToServe = QueueWithLowDeadline(QHigh, QMid, QLow);


 if (QueueToServe == "High")
 {

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

                RSUStatusChangeMessage->setName(RSUName.c_str());
                //std::cout<<get<0>(TupleHigh)<<get<1>(TupleHigh)<<get<2>(TupleHigh)<<get<3>(TupleHigh)<<get<4>(TupleHigh)<<std::endl;
                QHigh = DeletefromQ(QHigh, VehicleID);
                if (RSUFileOut.is_open())
                {
                    std::string VehicleIDStr =  std::to_string(VehicleID);
                    RSUFileOut << VehicleIDStr << endl;
                }
                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

        }
        }
 }

 else if (QueueToServe == "Mid")
 {
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
                QMid = DeletefromQ(QMid, VehicleID);\
                if (RSUFileOut.is_open())
                {
                    std::string VehicleIDStr =  std::to_string(VehicleID);
                    RSUFileOut << VehicleIDStr << endl;

                }
                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

        }
        }
}

 else if (QueueToServe == "Low")
 {
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
                if (RSUFileOut.is_open())
                {
                    std::string VehicleIDStr =  std::to_string(VehicleID);
                    RSUFileOut << VehicleIDStr << endl;

                }

                MessageFromVehilceToServe = "";
                VehicleID = 0;
                scheduleAt(simTime()+RSUBusyTime+uniform(0.05, 1), RSUStatusChangeMessage);
            }

        }
        }
 }

    RSUFileOut.close();


    //Static Threshold Prioratization Function
    //End

}




//////////// FCFS for resultscheckinQFuzzy

void TraCIDemoRSU11p::FCFS(int VehicleID, int ServiceTime, int DeadLine)
{

    std::string ResultFCFSFileName("ResultAnalysisFCFS.txt");
    ofstream ResultFCFSFileOut;
    ResultFCFSFileOut.open(ResultFCFSFileName, std::ios_base::app);

    std::string RSUName = getParentModule()->getFullName();
    double SimTime = simTime().dbl();
    ResultFCFSFileOut << VehicleID << " " << SimTime << " " << ServiceTime << " "<< DeadLine << std::endl;

}



///////////// FCFS ends



void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{




    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    if (wsm->getKind()==QUEUE_HANDLING || wsm->getKind()==STATUS_CHANGE || wsm->getKind()==STATUS_CHANGE_FUZZY)
    {
        return;
    }


    TraCIDemoRSU11p::FCFS(wsm->getNodeID(), wsm->getServiceTime(),wsm->getTaskDeadline());

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
    std::cout<<"  TEST Vehicle ID:  "<< wsm->getNodeID()<<std::endl;
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
    float SPDistance = DefineDistance2(DistancewithRSU, RSUmaxRange);
    float SPConnection = DefineConnection2(connectionStr);
    bool ConnectionType = DefineConnection(connectionStr);

    float SP_New = SPDistance + SPConnection;


    int SubPriority;
    SubPriority = SubPriotizationFunction(DistanceType, ConnectionType, InRSURange);
    //std::cout<<"SubPriority: " << SubPriority <<std::endl;
    int ST = wsm->getServiceTime();
    int InitDeadline = wsm->getTaskDeadline();
    //double ActualDeadLine = InitDeadline - SimTimetoDoble(simTime());  ---> This is for real time scenario, for result analysis case, we will not decrease simtime.
    double ActualDeadLine = InitDeadline;



    /*// For calcualtion of threshold time
    if (ActualDeadLine > MaxDeadline)
    {
        MaxDeadline = ActualDeadLine;
    }*/



    simtime_t MessageInitTime = wsm->getMessageInitTime();
    double MsgInitTime = SimTimetoDoble(MessageInitTime);
    double TimetoReachRSU = SimTimetoDoble(simTime()) - SimTimetoDoble(MessageInitTime) ;
    //std::cout<< "Time took for message to reach : " << TimetoReachRSU << " and hop count = "<< wsm->getHopCounter()<< std::endl;












    int SP = SubPriority;
    double Deadline = ActualDeadLine;


    double MaxWaitTimeDouble = Deadline - simTime().dbl();

    int MaxWaitTime = (int)round(MaxWaitTimeDouble);

    //std::cout<<"MaxWaitTime "<< MaxWaitTime <<std::endl;

    //std::cout<<"InitDeadline "<< InitDeadline << " Actual Deadline: " << Deadline<< " ST: " << ST <<std::endl;



    // Priority Function Fuzzy based
    // Start

    if (MaxWaitTime > 0)
    {
        float th_mean = DefineTime2(MaxWaitTime);
        TimeFuzzyThreshold = th_mean;

        if (MaxWaitTime == ST)
        {
            std::cout<<"Vehicle- "<<VehicleID<<" 's Request will be Fuzzy Services Now ASAP!!!!"<<std::endl;
        }
        else if (MaxWaitTime < ST)
        {
            std::cout<<"Vehicle- "<<VehicleID<< " ST: " <<ST << " WT: " << MaxWaitTime << "Deadline: " << Deadline<<std::endl;
            std::cout<<"Vehicle- "<<VehicleID<<" 's Request Deadline is already gone, no point now in Fuzzy!!!"<<std::endl;
        }
        else if (MaxWaitTime > ST)
        {
            if(MaxWaitTime <= th_mean){
            std::cout<<"Vehicle- "<<VehicleID<<" 's Request Will be put to a Fuzzy based queue. "<<std::endl;
            tuple <int, double, std::string, double, int, float> Task;
            std::string RequestedResource = "RR";
            tuple <int, double, std::string, double, int, float> Q;
            Q = make_tuple(VehicleID, MaxWaitTime,RequestedResource, MsgInitTime, ST, SP_New);

            if (get<0>(Q) != VehicleID)
             {
                 std::cout<<"Changing" << std::endl;
                 std::get<0>(Q) = VehicleID;
                 //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             }

            std::cout<<" Higher Fuzzy Priority. "<< SP_New <<std::endl;
            if (QHigh_Fuzzy.empty()==false)
             {
                 bool inQ = checkinQFuzzy(QHigh_Fuzzy, VehicleID);
                 if (QLow_Fuzzy.empty()==false){
                 QLow_Fuzzy = DeletefromQFuzzy(QLow_Fuzzy, VehicleID);
                 QLow_Fuzzy = sortQFuzzy(QLow_Fuzzy);}
                 //std::cout<<" Check value 1: "<<inQ<<std::endl;
                 if (inQ == false){

                     QHigh_Fuzzy.push(Q);
                     QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);
                 }
                 else {
                     //std::cout<<" Higher Priority. 1"<<std::endl;
                     QHigh_Fuzzy = replaceinQFuzzy(QHigh_Fuzzy, Q);
                     QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);
                 }
             }
             else {
                     QHigh_Fuzzy.push(Q);
             }

        }
        else if(MaxWaitTime > th_mean){
                    std::cout<<"Vehicle- "<<VehicleID<<" 's Request Will be put to a Fuzzy based queue. "<<std::endl;
                    tuple <int, double, std::string, double, int, float> Task;
                    std::string RequestedResource = "RR";
                    tuple <int, double, std::string, double, int, float> Q;
                    Q = make_tuple(VehicleID, MaxWaitTime,RequestedResource, MsgInitTime, ST, SP_New);

                    if (get<0>(Q) != VehicleID)
                     {
                         std::cout<<"Changing" << std::endl;
                         std::get<0>(Q) = VehicleID;
                         //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
                     }

                    std::cout<<" Lower Fuzzy Priority. "<< SP_New <<std::endl;
                    if (QLow_Fuzzy.empty()==false)
                     {
                         bool inQ = checkinQFuzzy(QLow_Fuzzy, VehicleID);
                         if (QHigh_Fuzzy.empty()==false){
                         QHigh_Fuzzy = DeletefromQFuzzy(QHigh_Fuzzy, VehicleID);
                         QHigh_Fuzzy = sortQFuzzy(QHigh_Fuzzy);}
                         //std::cout<<" Check value 1: "<<inQ<<std::endl;
                         if (inQ == false){
                             QLow_Fuzzy.push(Q);
                             QLow_Fuzzy = sortQFuzzy(QLow_Fuzzy);
                         }
                         else {
                             //std::cout<<" Higher Priority. 1"<<std::endl;
                             QLow_Fuzzy = replaceinQFuzzy(QLow_Fuzzy, Q);
                             QLow_Fuzzy = sortQFuzzy(QLow_Fuzzy);
                         }
                     }
                     else {
                             QLow_Fuzzy.push(Q);
                     }

                }
        }

    }



    // Priority Function Fuzzy based
    // END


    // Priority Function Static Threshold Based
    // Start




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

         if (MaxWaitTime <= ActualDeadLine * DeadLinePercetageThreshold)
         {
             std::cout<<" Higher Priority. "<< SP <<std::endl;
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
         else if (MaxWaitTime > ActualDeadLine * DeadLinePercetageThreshold)
         {
             std::cout<<" Lower Priority. "<< SP << std::endl;
             //std::cout<<"VID: " << get<0>(Q)<< '\t' << VehicleID<<std::endl;
             if (SP==1)
              {

                  if (QHigh.size()< HighPriorityThreshold)
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
                        std::cout<<"Did not find in queue" <<std::endl;
                        QMid.push(Q);
                        QMid = sortQ(QMid);
                    }
                    else {
                        std::cout<<"Found in queue" <<std::endl;
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

                  if (QMid.size()< HighPriorityThreshold)
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
                          std::cout<<"Did not find in queue" <<std::endl;
                          QMid.push(Q);
                          QMid = sortQ(QMid);
                      }
                      else {
                          std::cout<<"Found in queue" <<std::endl;
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
                              std::cout<<"Did not find in queue" <<std::endl;
                              QLow.push(Q);
                              QLow = sortQ(QLow);
                          }
                          else {
                              std::cout<<"Found in queue" <<std::endl;
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
                  else {
                      QLow.push(Q);
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
    std::cout<<"QHighFuzzy: ";
    showqFuzzy(QHigh_Fuzzy);
    std::cout<<"QLowFuzzy: ";
    showqFuzzy(QLow_Fuzzy);

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
            //std::cout<<"STATUS_CHANGE_RETURN from " << getParentModule()<<std::endl;
            std::cout << getParentModule()<<std::endl;
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

    if (msg->getKind() == STATUS_CHANGE_FUZZY)
        {
            //std::cout<< "STATUS_CHANGE Start at "<< simTime() << std::endl;
            //std::cout<< msg->getName()<<std::endl;
            std::string RSUNAME = msg->getName();
            //std::cout<< RSUNAME<<std::endl;
            if (RSUNAME == "Initialize")
            {
                //std::cout<<"STATUS_CHANGE_RETURN from " << getParentModule()<<std::endl;
                std::cout << getParentModule()<<std::endl;
                return;
            }
            if (RSUNAME == getParentModule()->getFullPath())
            {
            RSUBusyFuzzy=false;
            RSUBusyTimeFuzzy=0;
            std::cout<<RSUNAME<< " Is free now Fuzzy!!!!!!!!!!!!!!! at " <<simTime()<<std::endl;

            }
            //std::cout<< "STATUS_CHANGE End"<<std::endl;
        }

    if (msg->getKind() == DELETE_REQUEST_FROM_RSU)
    {

        std::string MessageBody = msg->getName();

        if (MessageBody == "Initialize")
        {
        //std::cout<<"DELETE_REQUEST_FROM_RSU_RETURN"<<std::endl;
        return;
        }


        std::string RSUNAME = MessageBody.substr(0, MessageBody.find(" "));
        std::string VehicleIDStr = MessageBody.substr(MessageBody.find(" ") + 1);
        int VehicleID = std::stoi(VehicleIDStr);
        //std::cout << "DELETE REQUEST FROM OTHER RSU >>> " <<RSUNAME << " " << getParentModule()->getFullPath() << " " << VehicleID << std::endl;
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


        remove("RSU.txt");
        remove("RSUFuzzy.txt");
        remove("Distance.txt");
        remove("Connection.txt");
        remove("Time.txt");

        std::cout<<"QHigh: ";
        showq(QHigh);
        std::cout<<"QMid: ";
        showq(QMid);
        std::cout<<"QLow: ";
        showq(QLow);
        std::cout<<std::endl;
        //system("python3 FCFS.py");
        //remove("ResultAnalysisFCFS.txt");


}
