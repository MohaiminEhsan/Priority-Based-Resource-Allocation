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


#include "TraCIDemoRSU11p.h"

#include <veins/modules/application/traci/Registry.h>
#include <veins/modules/application/traci/CommonVars.h>
#include "veins/base/utils/Coord.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"





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
#include <stack>
#include <queue>


using namespace veins;
using namespace std;


Define_Module(veins::TraCIDemoRSU11p);

Registry edge;
int rsuRange = 400;
int rangeThreshold = 4;
std::map<int, double> alpha;
int inRangeMsgRcv = 0;
int wsmReceived = 0;
int numberVehicles = 3000;
int appID = 0;

int rsuList[6] = {14,19,24,29,34,39};


std::map<int, std::map<int , int>> bandWidth {
    {14, {{19,rand() % 20 + 0},{24,rand() % 20 + 0},{29,rand() % 20 + 0},{34,rand() % 20 + 0},{39,rand() % 20 + 0}} },
    {19, {{14,rand() % 20 + 0},{24,rand() % 20 + 0},{29,rand() % 20 + 0},{34,rand() % 20 + 0},{39,rand() % 20 + 0}} },
    {24, {{19,rand() % 20 + 0}, {14,rand() % 20 + 0},{29,rand() % 20 + 0},{34,rand() % 20 + 0},{39,rand() % 20 + 0}} },
    {29, {{19,rand() % 20 + 0},{24,rand() % 20 + 0},{14,rand() % 20 + 0},{34,rand() % 20 + 0},{39,rand() % 20 + 0}} },
    {34, {{19,rand() % 20 + 0},{24,rand() % 20 + 0},{29,rand() % 20 + 0},{14,rand() % 20 + 0},{39,rand() % 20 + 0}} },
    {39, {{19,rand() % 20 + 0},{24,rand() % 20 + 0},{29,rand() % 20 + 0},{34,rand() % 20 + 0},{14,rand() % 20 + 0}} },
};

double rMatrix [6] [6] = {
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
};

double fMatrix [6] [6] = {
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
        {100, 100, 100, 100, 100, 100},
};


//double rMatrix [6] [6] = {
//        {-1, -1, -1, -1, 100, 100 },
//        {100, 100, -1, -1, 100, 100},
//        {100, 100, -1, 100, 100, 100 },
//        {100, 100, -1, -1, 100, 100 },
//        {100, -1, -1, -1, 100, 100 },
//        {100, 100, -1, -1, 100, 100},
//
//};

double qMatrix [6] [6] = {
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
};


double qfMatrix [6] [6] = {
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
};




double gamma1 = 0.75;
double qAlpha = .95;


// processing, storage, bandwidth
double influence [3] = {0.5, 0.30, 0.20};



//double distance(Coord& a,  Coord& b) {
//    Coord dist(a - b);
//    return dist.length();
//}

//void updateAlpha(int rsuID, int simulationTime){
//
//    std::cout << "Update alpha called for rsu:  "  << rsuID << endl;
//    std::cout << "Previous Alpha: " << alpha[rsuID] << endl;
//
//    double actualVolume = edge.inRangeVehicle[rsuID][simulationTime].size();
//    double predictedVolume = edge.predictedVolume[rsuID][simulationTime];
//
//    if(predictedVolume !=0 && actualVolume !=0){
//        alpha[rsuID] = actualVolume/ predictedVolume;
//    }
//
//    std::cout<< "Actual Volume: " << actualVolume <<endl;
//    std::cout<< "Predicted Volume: " << predictedVolume << endl ;
//    std::cout << "Division: " << actualVolume/ predictedVolume << endl;
//    std::cout<< "Updated Alpha: " << alpha[rsuID]<< endl;
//    std::cout<< "------------------" << endl;
//
//}



/////////////////////////////// Fuzzification Start

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



int FuzzyAlgo()
{
    double dmid=rsuRange/2;
    double dlend=(0.6666*rsuRange);
    double drstart=(0.3333*rsuRange);
    CFuzzyFunction *FuzzySet[2];
    FuzzySet[0] = new CTrapezoid;
    FuzzySet[1] = new CTrapezoid;
    //FuzzySet[2] = new CTrapezoid;

    FuzzySet[0]->setInterval(1,rsuRange);
    FuzzySet[0]->setMiddle(1,dlend);
    //FuzzySet[0]->setType('r');
    FuzzySet[0]->setName("good_distance");

    FuzzySet[1]->setInterval(dmid,rsuRange);
    FuzzySet[1]->setMiddle(drstart,rsuRange);
    //FuzzySet[1]->setType('r');
    FuzzySet[1]->setName("bad_distance");
    return 0;
}


float DefineDistanceFuzzy(float Distance)
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

    //std::cout<<"SPD: " <<SP<<std::endl;
    return SP;
}


double TimeMeanCalculation()
{
    double mean;
    std::list<double> TimeList;
    std::string line;
    std::string FileName("Time.txt");
    ofstream FileOut;
    std::ifstream FileIn("Time.txt");
    FileOut.open(FileName, std::ios_base::app);
    if (FileIn.is_open())
        {

        while(std::getline(FileIn, line))
        {
            TimeList.push_back(std::stoi(line));
        }
        FileIn.close();
        }
    double avg = 0;
    std::list<double>::const_iterator it;
    for(it = TimeList.begin(); it != TimeList.end(); it++) avg += *it;
    avg /= TimeList.size();

    mean = avg;
    //std::cout<< "TIME MEAN : " << mean<<std::endl;
    return mean;
}


double DefineTimeFuzzy(int time)
{
    double mean;
    std::list<double> TimeList;
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

    double avg = 0;
    std::list<double>::const_iterator it;
    for(it = TimeList.begin(); it != TimeList.end(); it++) avg += *it;
    avg /= TimeList.size();

    mean = avg;
    //std::cout<< "TIME MEAN : " << mean<<std::endl;
    return mean;

}


float DefineConnectionFuzzy(float connectionStr)
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

//        std::cout<<"SPC: " <<SP<<std::endl;

        FileOut.close();

        return SP;

}


double SimTimetoDoble (simtime_t time)
{
    std::string Time = time.str();
    double TimeDouble = ::atof(Time.c_str());
    return TimeDouble;
}

bool checkinQ(queue<tuple <int, int,float> > q, int id)
{
    /*std::cout<<"Before Check Started Status!! " << id << std::endl;
    showq(q);*/
   // std::cout<<"Checked started!!" << std::endl;
    bool flag = false;
    queue<tuple <int, int, float> > g = q;
    while (!g.empty())
      {
        // Each element of the priority
        // queue is a tuple itself
        tuple <int,  int, float> Tuple = g.front();
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

queue<tuple <int,  int, float>> replaceinQ(queue<tuple <int,  int, float> > q, tuple <int, int,float> T)
{
    //std::cout<<"Replace started!!" << std::endl;
    queue<tuple <int,  int, float> > g = q;
    queue<tuple <int,  int, float> > gnew;
    int id = get<0>(T);

    while (!g.empty())
          {
            tuple <int,  int, float> Tuple = g.front();
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

    //std::cout<<"Replaced!!" << std::endl;
}


void showQ(queue<tuple <int, int, float> > gq)
{
    queue<tuple <int, int, float> > g = gq;
    while (!g.empty()) {
        tuple <int, int, float> Tuple = g.front();
        std::cout << '\t' << get<0>(Tuple) << '\t' << get<1>(Tuple) <<'\t' << get<2>(Tuple) << '\t'  <<"---->";
        g.pop();
    }
    std::cout<<std::endl;
}


queue<tuple <int,  int, float>> DeletefromQ(queue<tuple <int,  int, float> > q, int id)
{
    //std::cout<<"Delete Function"<<std::endl;

    /*std::cout<<"Before Delete Function Status: "<< id <<std::endl;
    showq(q);*/

    queue<tuple <int, int,float> > g = q;
    queue<tuple <int, int,float> > gnew;
    while (!g.empty())
      {
        tuple <int,  int, float> Tuple = g.front();
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


/// Sorting

int minIndex(queue<tuple <int,  int, float> > q, int sortedIndex)
{
    int min_index = -1;
    double min_val = INT_MAX;
    int n = q.size();
    for (int i=0; i<n; i++)
    {
        tuple <int,  int, float> T = q.front();

        double curr = get<2>(T);
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
queue<tuple <int, int,float> > insertMinToRear(queue<tuple <int,  int, float> > q, int min_index)
{

    double min_val;
    tuple <int,  int, float> Temp;
    int n = q.size();
    for (int i = 0; i < n; i++)
    {
        tuple <int,  int, float> T = q.front();
        double curr =  get<2>(T);
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

queue<tuple <int,  int, float> > reverseQueue(queue<tuple <int,  int, float> > queue) {
    int n = queue.size();
    stack<tuple <int,  int, float>> st;

    // Remove all the elements from queue and push them to stack
    for (int i = 0; i < n; i++) {
        tuple <int, int,float> curr = queue.front();
        queue.pop();
        st.push(curr);
    }

    // Pop out elements from the stack and push them back to queue
    for (int i = 0; i < n; i++) {
        tuple <int, int,float> curr = st.top();
        st.pop();
        queue.push(curr);
    }

    // Print the reversed queue
    for (int i = 0; i < n; i++) {
        tuple <int, int,float> curr = queue.front();
        queue.pop();
        queue.push(curr);
    }

    return queue;

}



queue<tuple <int,  int, float> > sortQ(queue<tuple <int,  int, float> > q)
{

    for (int i = 1; i <= q.size(); i++)
    {
        int min_index = minIndex(q, q.size() - i);
        q = insertMinToRear(q, min_index);
    }


    q = reverseQueue(q);

    return q;
}


queue<tuple <int,  int, float> >  QueueDeductTimeFromMembers(queue<tuple <int,  int, float> > q)
{

    queue<tuple <int,  int, float> > g = q;
    queue<tuple <int,  int, float> > gnew;
    double TimeToDeduct = 2;
    double MaxWaitTime;

    while (!g.empty())
      {
        tuple <int,  int, float> Tuple = g.front();

        tuple <int,  int, float> NewTuple;

        double MaxWaitTimeInit = get<1>(Tuple);

        if (MaxWaitTimeInit < TimeToDeduct)
        {
            TimeToDeduct = 0;
        }

        MaxWaitTime = MaxWaitTimeInit - TimeToDeduct;

        NewTuple = make_tuple(get<0>(Tuple), MaxWaitTime,get<2>(Tuple));

        gnew.push(NewTuple);

        g.pop();
        TimeToDeduct = 2;

      }

    return gnew;

}

float EuclidianDistanceCalculation(Coord Rsu, Coord Vehicle){

    double RsuX = Rsu.x;
    double RsuY = Rsu.y;
    double VehicleX = Vehicle.x;
    double VehicleY = Vehicle.y;
    float distance = sqrt(pow((RsuX - VehicleX),2) + pow((RsuY - VehicleY),2));
    return distance;


}


/////////////////////////////// Fuzzification End


void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{

    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    float connectionValue = check_and_cast<DeciderResult80211*>(check_and_cast<PhyToMacControlInfo*>(wsm -> getControlInfo()) -> getDeciderResult()) -> getRecvPower_dBm();


    if(wsm->getInRange() == true){
        int vehicleID = wsm->getSenderAddress();
        int messageTime = wsm->getMessageTime();
        //std::cout << "Vehicle in Range: " << messageTime << ". at: " << myId << ". Vehicle ID: " << vehicleID << endl;
        inRangeMsgRcv++;
//        wsmReceived++;
//        std::cout << "Total Message Received: " << inRangeMsgRcv  << endl ;
//        std::cout << "---------------------------------"<< endl;
        edge.inRangeVehicle[myId][messageTime].insert(edge.inRangeVehicle[myId][messageTime].begin(), vehicleID);

    }


    // If wsm is from vehicle and targeted to this RSU
    if(wsm->getSenderType() == 1 && wsm->getTargetAddress() == myId){


        int vehicleID = wsm->getSenderAddress();
        int messageTime = wsm->getMessageTime();


        // Finding the RSU Co-ordinate.
        BaseMobility *baseMob;
        baseMob = FindModule<BaseMobility*>::findSubModule(getParentModule());
        Coord rsuCoord = baseMob->getPositionAt(simTime().dbl());

        Coord vehicleCoord = wsm->getSenderPositionRLDCO();
        int lastKnown = get<0>(edge.msgRegistry[myId][vehicleID]);

        float distance = EuclidianDistanceCalculation(rsuCoord, vehicleCoord);

        // is this a new message
        if(lastKnown != messageTime && lastKnown < messageTime ){

            wsmReceived++;

            //Retrieving WSM information
            float vehicleSpeed = wsm->getSenderSpeedRLDCO();
            int hopCount = wsm->getHopCountRLDCO();
            double dwellDistance = wsm->getDwellDistance();
            int dwellTime = wsm->getDwellTime();
            int timeToReach = wsm->getTimeToReach();
            int dwellStart = messageTime + timeToReach;
            int dwellEnd = dwellStart + dwellTime;
            int availableResource = wsm->getAvailableResource();
            int availableStorage = wsm->getAvailableStorage();
            double MessageInitTime = wsm->getMessageInitTime();
            int RequestDeadline = wsm->getRequestDeadline();
            bool Requester = wsm->getRequester();
//            std::cout<<"Time: " << wsm->getMessageInitTime()<<endl;
//            std::cout << availableStorage << endl;
//            std::cout << "---------------"<< endl;
//
//            Coord dist(vehicleCoord - rsuCoord);
//            if(dist.length() > rsuRange){
//                std::cout<< dist.length() << endl;
//                std::cout<< "RSU Received this message" << endl;
//                std::cout<< "Dwell Time: " << dwellTime << endl;
//                std::cout<< "Time to Reach: " << timeToReach<< endl;
//                std::cout<< "Dwell Start: " << dwellStart << endl;
//                std::cout<< "Dwell End: " << dwellEnd << endl;
//                std::cout << "---------------" << endl;
//            }

            std::tuple<int, double, int, int, int, int, float, float, double, int, bool> vehicleData (messageTime, dwellDistance, dwellStart, dwellEnd, availableResource, availableStorage, distance, connectionValue, MessageInitTime, RequestDeadline, Requester);
            edge.msgRegistry[myId][vehicleID] = vehicleData;

        } // message doesnt exists in database

    }

}


// New Changes
void TraCIDemoRSU11p::initialize(int stage) {

    DemoBaseApplLayer::initialize(stage);

    std::ofstream outfile ("Distance.txt");
    outfile.close();
    std::ofstream Coutfile ("Connection.txt");
    Coutfile.close();
    std::ofstream Timeoutfile ("Time.txt");
    Timeoutfile.close();

    if (stage == 0) {

        broadcast_event = new cMessage("boradcast_event");
        scheduleAt(simTime() + 5, broadcast_event);

        request_event = new cMessage("request_event");
        scheduleAt(simTime() + 1, request_event);

        dwellTime_event = new cMessage("dwellTime_event");
        scheduleAt(simTime() + 1, dwellTime_event);

        vehicle_event = new cMessage("vehicle_event");
        scheduleAt(simTime() + 1, vehicle_event);

        traffic_flow_event = new cMessage("traffic_flow_event");
        scheduleAt(simTime()+5, traffic_flow_event);

        rl_event = new cMessage("rl_event");
        scheduleAt(simTime()+2, rl_event);

        app_status = new cMessage("app_status");
        scheduleAt(simTime()+2, app_status);

        fuzzy_event = new cMessage("fuzzy_event");
        scheduleAt(simTime()+1, fuzzy_event);

        fuzzy_request_event = new cMessage("fuzzy_request_event");
//        scheduleAt(simTime()+1, fuzzy_request_event);

        queue_handling_event = new cMessage("queue_handling_event");
        scheduleAt(simTime()+2, queue_handling_event);

        fuzzy_allocation_event = new cMessage("fuzzy_allocation_event");
//        scheduleAt(simTime()+20, fuzzy_allocation_event);

        rl_fz_event = new cMessage("rl_fz_event");
        scheduleAt(simTime()+2, rl_fz_event);

        stage++;

    }

}

//New Changes
void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}


int findIndex(int rsuID){
    int index;
    for(int i=0; i<6; i++){
        if(rsuList[i]== rsuID){
            index = i;
            break;
        }
    }
    return index;
}

std::vector<int> available_actions(int state){

    std::vector<int> av_act;

    for (int i = 0; i < 6; i++){
       if(rMatrix[state][i] >= 0){
            av_act.push_back(i);
        }
    }

    return av_act;
}

std::vector<int> available_actionsF(int state){

    std::vector<int> av_act;

    for (int i = 0; i < 6; i++){
       if(fMatrix[state][i] >= 0){
            av_act.push_back(i);
        }
    }

    return av_act;
}


void updateQMatrix(int current_state, int action){
    double max_value = 0;
    std::vector<int> available_acts = available_actions(action);

    for(int j=0; j< available_acts.size(); j++){
        if(qMatrix[action][available_acts.at(j)] >= max_value)
        max_value = qMatrix[action][available_acts.at(j)];
    }

    qMatrix[current_state][action] = ((1-qAlpha)*qMatrix[current_state][action]) +  (qAlpha * (rMatrix[current_state][action] + (gamma1* max_value)));
}

void updateQFMatrix(int current_state, int action){
    double max_value = 0;
    std::vector<int> available_acts = available_actionsF(action);

    for(int j=0; j< available_acts.size(); j++){
        if(qfMatrix[action][available_acts.at(j)] >= max_value)
        max_value = qfMatrix[action][available_acts.at(j)];
    }

    qfMatrix[current_state][action] = ((1-qAlpha)*qfMatrix[current_state][action]) +  (qAlpha * (fMatrix[current_state][action] + (gamma1* max_value)));
}


int chooseFogContRL(int appID, int simulationTime, int originRSU, int resourceReq, int storageReq, int appStart, int appEnd){

    std::vector<std::tuple<int, int, int, int, double>> probFog;
    int score;
    int highestScore=0;
    int chosenFog;

    for(int i=0; i<6; i++){
        int resourcePoolCont = 0 ;
        int storagePoolCont = 0;
        if(rsuList[i]!=originRSU ){
            for (auto const& vehicle : edge.msgRegistry[rsuList[i]]){
                if(appStart >= get<2>(vehicle.second) && appEnd <= get<3>(vehicle.second) ){
                    resourcePoolCont = resourcePoolCont + get<4>(vehicle.second);
                    storagePoolCont = storagePoolCont + get<5>(vehicle.second);
                }
            }

            if(resourcePoolCont >= resourceReq && storagePoolCont >= storageReq){
                score = (resourcePoolCont * influence[0]) + ( storagePoolCont * influence[1] ) + ( bandWidth[originRSU][rsuList[i]]  * influence[2]);
                if(score >= highestScore) {
                    highestScore= score;
                    //chosenFog = rsuList[i];
                }
                std::tuple<int, int, int, int, double> probDetails (rsuList[i], resourcePoolCont, storagePoolCont, bandWidth[originRSU][rsuList[i]], score);
                probFog.push_back(probDetails);
            }

        } // if not origin
    }

    if(probFog.size()!=0){ // ALL the RL Magic Here
        std::cout << "RL Enabled Probable Fogs: " << endl ;
        double max_value = 0;
        int state = findIndex(originRSU);
        int max_index;
        int action;
        for(auto itr : probFog){
            std::cout << get<0>(itr) << ", Resource: " << get<1>(itr) << ", Storage: " << get<2>(itr) << ", Bandwidth: " << get<3>(itr) << ", Score: " << get<4>(itr)
                    << endl;
            action = findIndex(get<0>(itr));

            if(qMatrix[state][action] >= max_value){
                max_value = qMatrix[state][action];
                max_index = action;
            }

        }

        std::cout << "-------------------------" << endl;
        return rsuList[action];
    }

    else{

        return 0;
    }

}


int chooseFogCont(int appID, int simulationTime, int originRSU, int resourceReq, int storageReq, int appStart, int appEnd){

    //std::vector<int> probFog;
    //std::cout << "Choose Fog Continuous Called." << endl ;
    std::vector<std::tuple<int, int, int, int, double>> probFog;
    int score;
    int highestScore=0;
    int chosenFog;

    for(int i=0; i<6; i++){
        int resourcePoolCont = 0 ;
        int storagePoolCont = 0;
        if(rsuList[i]!=originRSU ){
            for (auto const& vehicle : edge.msgRegistry[rsuList[i]]){
                if(appStart >= get<2>(vehicle.second) && appEnd <= get<3>(vehicle.second) ){
                    resourcePoolCont = resourcePoolCont + get<4>(vehicle.second);
                    storagePoolCont = storagePoolCont + get<5>(vehicle.second);
                }
            }

            if(resourcePoolCont >= resourceReq && storagePoolCont >= storageReq){
                score = (resourcePoolCont * influence[0]) + ( storagePoolCont * influence[1] ) + ( bandWidth[originRSU][rsuList[i]]  * influence[2]);
                if(score >= highestScore) {
                    highestScore= score;
                    chosenFog = rsuList[i];
                }
                std::tuple<int, int, int, int, double> probDetails (rsuList[i], resourcePoolCont, storagePoolCont, bandWidth[originRSU][rsuList[i]], score);
                probFog.push_back(probDetails);
            }

        } // if not origin
    }

    if(probFog.size()!=0){
//        for(auto itr : probFog){
//
//        }
        return chosenFog;
    }

    else{
        return 0;
    }

}

int chooseFogDist(int appID, int simulationTime, int originRSU, int resourceReq, int storageReq, int appStart, int appEnd){

    std::vector<std::tuple<int, int, int, int, double>> probFog;
    //std::cout << "Choose Fog Disrupted Called." << endl ;

    int score;
    int highestScore=0;
    int chosenFog;

    for(int i=0; i<6; i++){

        int lowest = 9999;
        int lowestResourceDist = 9999;
        int lowestStorageDist = 9999;

        if(rsuList[i]!=originRSU ){

            for (int j = appStart ; j <= appEnd ; j++){
                int currentTotal = 0;
                int resourcePoolDist=0;
                int storagePoolDist = 0;
                for (auto const& vehicle : edge.msgRegistry[rsuList[i]]){
                    if(j >= get<2>(vehicle.second) || j <= get<3>(vehicle.second) ){
                        currentTotal++;
                        resourcePoolDist = resourcePoolDist + get<4>(vehicle.second);
                        storagePoolDist = storagePoolDist + get<5>(vehicle.second);
                    }
                }

                if(currentTotal <= lowest){
                   lowest = currentTotal;
                   lowestResourceDist = resourcePoolDist;
                   lowestStorageDist = storagePoolDist;
                }
            }

            if(lowestResourceDist >= resourceReq && lowestStorageDist >= storageReq){
                score = (lowestResourceDist * influence[0]) + ( lowestStorageDist * influence[1] ) + ( bandWidth[originRSU][rsuList[i]]  * influence[2]);
                if(score >= highestScore){
                    highestScore = score;
                    chosenFog = rsuList[i];
                }
                std::tuple<int, int, int, int, double> probDetails (rsuList[i], lowestResourceDist, lowestStorageDist, bandWidth[originRSU][rsuList[i]], score );
                probFog.push_back(probDetails);
            }
        } // if not origin

    }

    if(probFog.size()!=0){
        //std::cout << "Pr0obable Fogs: " << endl ;
//        for(auto itr : probFog){
//           // std::cout << get<0>(itr) << ", Resource: " << get<1>(itr) << ", Storage: " << get<2>(itr) << ", Bandwidth: " << get<3>(itr) << ", Score: " << get<4>(itr)
//                    //<< endl;
//        }
        return chosenFog;
    }

    else{
        return 0;
    }

}


int appStatusRL(int id){

    if(get<0>(edge.appRegistryRL[id]) == 0){ // Continuous
        int resourcePoolCont=0;
        int storagePoolCont=0;
        std::vector<int> vehiclesAvailable;

        for (auto const& vehicle : vehicleLocation){

            if(get<2>(vehicle) == get<5>(edge.appRegistryRL[id])){
                if(get<1>(vehicle) >= get<2>(edge.appRegistryRL[id])  && get<1>(vehicle) <= get<3>(edge.appRegistryRL[id])){
                    vehiclesAvailable.push_back(get<0>(vehicle));
                }

            }
        }

        sort( vehiclesAvailable.begin(),  vehiclesAvailable.end() );
        vehiclesAvailable.erase( unique(  vehiclesAvailable.begin(),  vehiclesAvailable.end() ),  vehiclesAvailable.end() );


        for (auto const& vehicle : vehiclesAvailable){
            resourcePoolCont += get<0>(vehicleResource[vehicle]);
            storagePoolCont += get<1>(vehicleResource[vehicle]);
        }

        if(resourcePoolCont >= get<6>(edge.appRegistryRL[id]) && storagePoolCont >= get<7>(edge.appRegistryRL[id])){

            if (get<8>(edge.appRegistryRL[id]) + 50 >= get<9>(edge.appRegistryRL[id]))
            {
                return 7;
            }
            else {
                return 5;
            }

        }
        else{
            return 5;
        }

    }

    else{ // Disruptive
        if (get<8>(edge.appRegistryRL[id]) + 50 >= get<9>(edge.appRegistryRL[id]))
        {
            return 7;
        }
        else {
            return 5;
        }

    }
}




int FZRLFunc(int id){

    double sp = 0;
    int counter = 0;
    int st = 0;


    double sp_ref = get<10>(edge.appRegistryFZ[id]);
    int st_ref = get<9>(edge.appRegistryFZ[id]);

    for (auto const& FZ : edge.appRegistryFZ){

        sp = sp + get<10>(FZ.second);
        st = st + get<9>(FZ.second);

        counter++;
    }
    sp = sp/counter;
    st = st/counter;

    if (sp_ref>=sp && st_ref<=st)
    {
        return 7;
    }
    else{
        return 5;
    }


}

int appStatusFZ(int id){

    if(get<0>(edge.appRegistryFZ[id]) == 0){ // Continuous
        int resourcePoolCont=0;
        int storagePoolCont=0;
        std::vector<int> vehiclesAvailable;

        for (auto const& vehicle : vehicleLocation){

            if(get<2>(vehicle) == get<5>(edge.appRegistryFZ[id])){
                if(get<1>(vehicle) >= get<2>(edge.appRegistryFZ[id])  && get<1>(vehicle) <= get<3>(edge.appRegistryFZ[id])){
                    vehiclesAvailable.push_back(get<0>(vehicle));
                }

            }
        }

        sort( vehiclesAvailable.begin(),  vehiclesAvailable.end() );
        vehiclesAvailable.erase( unique(  vehiclesAvailable.begin(),  vehiclesAvailable.end() ),  vehiclesAvailable.end() );


        for (auto const& vehicle : vehiclesAvailable){
            resourcePoolCont += get<0>(vehicleResource[vehicle]);
            storagePoolCont += get<1>(vehicleResource[vehicle]);
        }

        if(resourcePoolCont >= get<6>(edge.appRegistryFZ[id]) && storagePoolCont >= get<7>(edge.appRegistryFZ[id])){

            if (get<8>(edge.appRegistryFZ[id]) + 50 >= get<9>(edge.appRegistryFZ[id]))
            {
                return 7;
            }
            else {
                return FZRLFunc(id);
            }

        }
        else{
            return FZRLFunc(id);
        }

    }

    else{ // Disruptive
        if (get<8>(edge.appRegistryFZ[id]) + 50 >= get<9>(edge.appRegistryFZ[id]))
        {
            return 7;
        }
        else {
            return FZRLFunc(id);
        }

    }
}





int appStatus(int id){

    if(get<0>(edge.appRegistry[id]) == 0){
        //std::cout<< "App needs continuous resource" << endl;
        int resourcePoolCont=0;
        int storagePoolCont=0;
        std::vector<int> vehiclesAvailable;

        for (auto const& vehicle : vehicleLocation){

            if(get<2>(vehicle) == get<5>(edge.appRegistry[id])){

                //std::cout << get<2>(vehicle) << " " << get<5>(edge.appRegistry[id]) <<" " << get<1>(vehicle) << " "
                                                            //<<get<2>(edge.appRegistry[id]) << " " << get<3>(edge.appRegistry[id])
                                                            //<< endl;
                if(get<1>(vehicle) >= get<2>(edge.appRegistry[id])  && get<1>(vehicle) <= get<3>(edge.appRegistry[id])){
                    vehiclesAvailable.push_back(get<0>(vehicle));
                }

            }
        }

        sort( vehiclesAvailable.begin(),  vehiclesAvailable.end() );
        vehiclesAvailable.erase( unique(  vehiclesAvailable.begin(),  vehiclesAvailable.end() ),  vehiclesAvailable.end() );

        //std::cout << "Actual Vehicles Available: " << vehiclesAvailable.size() << endl;

        for (auto const& vehicle : vehiclesAvailable){
            resourcePoolCont += get<0>(vehicleResource[vehicle]);
            storagePoolCont += get<1>(vehicleResource[vehicle]);
        }

        //std::cout<< "Resource: " << resourcePoolCont << ", Storage: "<< storagePoolCont << endl;

        if(resourcePoolCont >= get<6>(edge.appRegistry[id]) && storagePoolCont >= get<7>(edge.appRegistry[id])){
            //std::cout<< "Assigned and Served." << endl;

            if (get<8>(edge.appRegistry[id]) + 50 >= get<9>(edge.appRegistry[id]))
            {
                return 7;
            }
            else {
                return 5;
            }
        }
        else{
            //std::cout<< "Assigned and Failed." << endl;
            return 5;
        }

    }
    else{
        //std::cout<< "App needs Disrupted resource" << endl;
        if (get<8>(edge.appRegistry[id]) + 50 >= get<9>(edge.appRegistry[id]))
        {
            return 7;
        }
        else {
            return 5;
        }
    }
}


int RandomNumberGenerator(int min, int max)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range

    int random = distr(gen); // generate numbers
    return random;
}


tuple <int, int, float> QueueCheckingHighFuzzy(queue<tuple<int, int, float> >QHigh)
{
    tuple <int, int, float> T;
    T = make_tuple(0, 0.0, 0.0);

    queue<tuple<int, int, float> >Q = QHigh;

    tuple <int, int, float> Temp;
    std::string Flag = "No";

    if (QHigh.empty()==false)
    {
        for (int i=0; i<QHigh.size(); i++)
            {
                Temp = Q.front();
                double curr = get<1>(Temp);
                Q.pop();

                if (curr < 5)
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

tuple <int, int, float> QueueCheckingLowFuzzy(queue<tuple<int, int, float> >QLow)
{
    tuple <int, int, float> T;
    T = make_tuple(0, 0.0, 0.0);

    queue<tuple<int, int, float> >Q = QLow;

    tuple <int, int, float> Temp;
    std::string Flag = "No";

    if (QLow.empty()==false)
    {
        for (int i=0; i<QLow.size(); i++)
            {
                Temp = Q.front();
                double curr = get<1>(Temp);
                Q.pop();
                double th_mean = TimeMeanCalculation();
                if (curr < th_mean)
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




int RandomDeadLineGenerateTime(double Time)
{
    int SimMaxTime = 1000;
    int min = int (Time);
    int max = SimMaxTime;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range
    int random = distr(gen); // generate numbers
    return random;
}





void TraCIDemoRSU11p::handleSelfMsg(cMessage* msg)
{

//    std::cout<<"In Handle Self Message" << std::endl;

    int simulationTime = (int) simTime().dbl();


    if (msg == fuzzy_event){

        float Distance;
        float Connection;


        for (auto const& vehicle : edge.msgRegistry[myId])
        {
            bool Requester = get<10>(vehicle.second);
            if (Requester == true)
            {
                Distance = get<6>(vehicle.second);
                Connection = get<7>(vehicle.second);
                int MessageInitTime = get<8>(vehicle.second);
                int RequestDeadline = get<9>(vehicle.second);
                if (Distance > rsuRange)
                {
                    continue;
                }
                float PriorityValue = DefineDistanceFuzzy(Distance) + DefineConnectionFuzzy(Connection);

                bool flag = false;

                for (auto const& Request : edge.FuzzyRequestRegistry)
                {
                    if (Request.first == vehicle.first){
                        flag = true;
                    }
                }

                if (flag == false)
                {
                    int RequestStatus = 0;
                    std::tuple<float, int, int, int> FuzzyRequestData (PriorityValue, MessageInitTime, RequestDeadline, RequestStatus);
                    edge.FuzzyRequestRegistry[vehicle.first] = FuzzyRequestData;
                }

            }

        }

        scheduleAt(simTime() + 1 , fuzzy_event);


    }


    if (msg==fuzzy_request_event){
        for (auto const& Request : edge.FuzzyRequestRegistry)
            {
                int RequestStatus = get<3>(Request.second);
                int Vid = Request.first;
                float PriorityValue = get<0>(Request.second);
                int MsgInitTime = get<1>(Request.second);
                double RequestDeadline = get<2>(Request.second);
                if (RequestStatus == 0) // Status 0 = Request not Assigned
                {
                    double TimeLeft = RequestDeadline - simTime().dbl();
                    int ServiceTime = RandomNumberGenerator(0, int(TimeLeft));
                    int MaxWaitTime = int(TimeLeft) - ServiceTime;
                    tuple <int, int, float> Q;
                    Q = make_tuple(Request.first, MaxWaitTime, PriorityValue);
                    if (MaxWaitTime <5){
                        std::cout<< Request.first << " will be executed Right NOW!!!!!!! at: " << simTime().dbl()<< " MessageInit Time - "<< MsgInitTime << " with deadline - 0" << endl;
                        RequestStatus = 99; // Status 99 = Done;
                        std::tuple<float, int, int, int> FuzzyRequestData (PriorityValue, MsgInitTime, RequestDeadline, RequestStatus);
                        edge.FuzzyRequestRegistry.erase(Vid);
                        edge.FuzzyRequestRegistry[Vid] = FuzzyRequestData;
                    }
                    else{
                        RequestStatus = 1; //Status 1 = Assigned to Queue
                        double mean_time = DefineTimeFuzzy(MaxWaitTime);
                        int th_mean = int(mean_time);
                        if (MaxWaitTime <= th_mean){
                            if (QHigh.empty()==false)
                            {
                                bool inQ = checkinQ(QHigh, Vid);
                                if (QLow.empty()==false){
                                    QLow = DeletefromQ(QLow, Vid);
                                    QLow= sortQ(QLow);
                                    }
                                if (inQ == false){
                                    QHigh.push(Q);
                                    QHigh = sortQ(QHigh);
                                }else {
                                    QHigh = replaceinQ(QHigh, Q);
                                    QHigh = sortQ(QHigh);

                                }


                            }
                            else {
                                QHigh.push(Q);
                            }
                        } else if (MaxWaitTime > th_mean)
                        {
                            if (QLow.empty()==false)
                                {
                                    bool inQ = checkinQ(QLow, Vid);
                                    if (QHigh.empty()==false){
                                        QHigh = DeletefromQ(QHigh, Vid);
                                        QHigh= sortQ(QHigh);
                                        }
                                    if (inQ == false){
                                        QLow.push(Q);
                                        QLow = sortQ(QLow);
                                    }else {
                                        QLow = replaceinQ(QLow, Q);
                                        QLow = sortQ(QLow);

                                    }


                                }
                            else {
                                QLow.push(Q);
                            }

                        }
                        std::tuple<float, int, int, int> FuzzyRequestData (PriorityValue, MsgInitTime, RequestDeadline, RequestStatus);
                        edge.FuzzyRequestRegistry.erase(Vid);
                        edge.FuzzyRequestRegistry[Vid] = FuzzyRequestData;
                    }

                }


            }
            scheduleAt(simTime() + 1 , fuzzy_request_event);

    }

    if (msg == queue_handling_event)
    {
        if (QLow.empty()==false)
        {
            QLow = QueueDeductTimeFromMembers(QLow);
        }
        if (QHigh.empty()==false)
        {
            QHigh = QueueDeductTimeFromMembers(QHigh);
        }

        for (auto const& AHP : edge.appRegistry)
        {
            int temp1 = get<8>(AHP.second) - 2;
            int temp2 = AHP.first;
            std::tuple<int, int, int, int, int, int, int, int, int,int, int> appData (get<0>(AHP.second), get<1>(AHP.second), get<2>(AHP.second), get<3>(AHP.second),  get<4>(AHP.second), get<5>(AHP.second),  get<6>(AHP.second), get<7>(AHP.second), temp2,get<9>(AHP.second), get<10>(AHP.second) );
            edge.appRegistry.erase(temp2);
            edge.appRegistry[temp2] = appData;
        }

        for (auto const& RL : edge.appRegistryRL)
        {
            int temp1 = get<8>(RL.second) - 2;
            int temp2 = RL.first;
            std::tuple<int, int, int, int, int, int, int, int, int,int, int> appDataRL (get<0>(RL.second), get<1>(RL.second), get<2>(RL.second), get<3>(RL.second),  get<4>(RL.second), get<5>(RL.second),  get<6>(RL.second), get<7>(RL.second), temp2,get<9>(RL.second), get<10>(RL.second) );
            edge.appRegistryRL.erase(temp2);
            edge.appRegistryRL[temp2] = appDataRL;
        }

        for (auto const& FZ : edge.appRegistryFZ)
        {
            int temp1 = get<8>(FZ.second) - 2;
            int temp2 = FZ.first;
            std::tuple<int, int, int, int, int, int, int, int, int,int, double, int> appDataFZ (get<0>(FZ.second), get<1>(FZ.second), get<2>(FZ.second), get<3>(FZ.second),  get<4>(FZ.second), get<5>(FZ.second),  get<6>(FZ.second), get<7>(FZ.second), temp2,get<9>(FZ.second), get<10>(FZ.second), get<11>(FZ.second) );
            edge.appRegistryFZ.erase(temp2);
            edge.appRegistryFZ[temp2] = appDataFZ;
        }

        scheduleAt(simTime() + 2, queue_handling_event);
    }

    if (msg == fuzzy_allocation_event)
    {
        tuple <int, int, float> TupleHigh;
        tuple <int, int, float> TupleLow;
        std::string MessageFromVehilceToServe;
        tuple <int, int, float> ServeNowHighFuzzy = QueueCheckingHighFuzzy(QHigh);
        tuple <int, int, float> ServeNowLowFuzzy = QueueCheckingLowFuzzy(QHigh);

        if (get<0>(ServeNowHighFuzzy)!=0)
        {
            std::cout<< get<0>(ServeNowHighFuzzy) << " will be executed Right NOW!!!!!!! at: " << simTime().dbl() << " with deadline - " << get<1>(ServeNowHighFuzzy) << " SP- " << get<2>(ServeNowHighFuzzy)<<endl;
            QHigh = DeletefromQ(QHigh, get<0>(ServeNowHighFuzzy));

            if (QLow.empty()==false)
            {
                TupleLow = QLow.front();
                QLow.pop();
                QHigh.push(TupleLow);
                QHigh = sortQ(QHigh);
            }

        }
        else if (get<0>(ServeNowHighFuzzy)==0)
        {
            if (QHigh.empty()==false){
                TupleHigh = QHigh.front();
                std::cout<< get<0>(TupleHigh) << " will be executed Right NOW!!!!!!! at: " << simTime().dbl() << " with deadline - " << get<1>(TupleHigh)<< " SP- " << get<2>(TupleHigh)<<endl;
                QHigh = DeletefromQ(QHigh,get<0>(TupleHigh));
                if (QLow.empty()==false)
                {
                    TupleLow = QLow.front();
                    QLow.pop();
                    QHigh.push(TupleLow);
                    QHigh = sortQ(QHigh);
                }


            }
        }
        TupleLow = QueueCheckingLowFuzzy(QLow);
        if (get<0>(TupleLow)!=0)
        {
            QLow=DeletefromQ(QLow, get<0>(TupleLow));
            QHigh.push(TupleLow);
            QHigh = sortQ(QHigh);
        }
        scheduleAt(simTime() + 20, fuzzy_allocation_event);

    }


    if(msg == broadcast_event){

        BaseMobility *baseMob;
        baseMob = FindModule<BaseMobility*>::findSubModule(getParentModule());

        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setSenderType(0);
        wsm->setSenderAddress(myId);
        wsm->setSenderPositionRLDCO(baseMob->getPositionAt(simTime().dbl()));
        wsm->setHopCountRLDCO(0);
        sendDelayedDown(wsm, uniform(1,3));

        if(simTime().dbl() < 100)
        scheduleAt(simTime() + 5, msg);

    }

    if(msg== request_event){ // Event is an application request


        if( simulationTime > 149 &&  simulationTime % 50 == 0 && simulationTime != 1000){


            int resourcePoolCont=0;
            int storagePoolCont = 0;
            int status = 0; // 0 - Initiated. 1 - Assigned, 2 - Denied  5 -> Failed. 7 -> Success
            int servedRSU=99;
            double appStart = simulationTime + 0; // Previously was 25
            int executionTime = 5;
            int appEnd = appStart + executionTime;
            int chosenFog;
            int RequestDeadlineF;
            int RDeadline = RandomDeadLineGenerateTime(simTime().dbl());
            int STime = RandomNumberGenerator(1, 0.25*RDeadline);
            int resourceReq = rand() % 10 + 10;
            int storageReq = rand() % 10 + 10;

            //int appType = rand() % 2; // 0 - Continuous connectivity, 1 - Disrupted connectivity
            int appType = 0 ;

//            predictLog.open("results/phase2/ahp/applications.csv",  ios::out | ios::app);
//                       predictLog << appID << ", " << appType << ", " << simulationTime <<", "
//                               <<  appStart << ", " <<  appEnd << ", " <<  myId << ", "
//                               <<  chosenFog << ", " <<  resourceReq << ", " <<  storageReq
//                               << ", " <<  0 << ", " <<  status << "\n" ;
//                       predictLog.close();

            //Application --> resource, deadline
//            Don't remove this section. 0
//            std::map<int, std::tuple<int,int>> applications;
//            int appCount = rand() % 10 + 0;
//
//            for(int i=0; i< appCount ; i++ ){
//               std::tuple<int, int> appData (rand() % 10 + 10, rand() % 10 + 10 );
//               applications[i] = appData;
//            }
//             If we want to implement multiple application requests and choosing algorithm.


            int vehicleCountThrough = 0;
            //int volume = 0;

            for (auto const& vehicle : edge.msgRegistry[myId]){
                if(appStart >= get<2>(vehicle.second) && appEnd <= get<3>(vehicle.second) ){
                    vehicleCountThrough++;
                    resourcePoolCont = resourcePoolCont + get<4>(vehicle.second);
                    storagePoolCont = storagePoolCont + get<5>(vehicle.second);

                }
            }

            int lowest = 9999;
            int disrupted = 0;
            int lowestResourceDist = 9999;
            int lowestStorageDist = 9999;

            for (int j = appStart ; j <= appEnd ; j++){
                int currentTotal = 0;
                int resourcePoolDist=0;
                int storagePoolDist=0;
                for (auto const& vehicle : edge.msgRegistry[myId]){
                    if(j >= get<2>(vehicle.second) || j <= get<3>(vehicle.second) ){
                        currentTotal++;
                        resourcePoolDist = resourcePoolDist + get<4>(vehicle.second);
                        storagePoolDist = storagePoolDist + get<5>(vehicle.second);
                    }
                }

                if(currentTotal <= lowest){
                   lowest=currentTotal;
                   lowestResourceDist = resourcePoolDist;
                   lowestStorageDist = storagePoolDist;
                }
            }

//            std::cout << "App Id: " << appID <<endl;
//            std::cout << "App Request on RSU: " << myId <<endl;
//            std::cout << "Application request at: " << simulationTime <<endl;
//            std::cout << "App type: " << appType <<endl;
//            std::cout << "Computation: " << resourceReq << " Storage: " << storageReq <<endl;
//            std::cout << "Continuous Conn: " << vehicleCountThrough <<endl;
//            std::cout << "Continuous Conn Computation: " << resourcePoolCont << " Storage: "<< storagePoolCont <<endl;
//            std::cout << "Disrupted Conn: " << lowest <<endl ;
//            std::cout << "Disrupted Conn Resource: " << lowestResourceDist << " Storage: " << lowestStorageDist << endl ;



            /*************************
             * Simple AHP Based Decision
             ***************************/

            if((appType== 0 && resourceReq <= resourcePoolCont && storageReq<= storagePoolCont) || (appType== 1 && resourceReq <= lowestResourceDist && storageReq <= lowestStorageDist)){ // Continuous/Disrupted and Served
               chosenFog = myId;
               status = 1; // Assigned
            }

            else if(appType==0){
                chosenFog = chooseFogCont(appID,  simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }

            else if(appType==1){
                chosenFog = chooseFogDist(appID, simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }





            // Inserting into application registry. // ID, type, time, rsu, servedRSU, resource, storage, deadline, status
            std::tuple<int, int, int, int, int, int, int, int, int, int, int> appData (appType, simulationTime, appStart, appEnd,  myId, chosenFog,  resourceReq, storageReq, RDeadline, STime, status );
            edge.appRegistry[appID] = appData;

            //Logging Applications for AHP
//            predictLog.open("results/phase2/ahp/applications.csv",  ios::out | ios::app);
//            predictLog << appID << ", " << appType << ", " << simulationTime <<", "
//                    <<  appStart << ", " <<  appEnd << ", " <<  myId << ", "
//                    <<  chosenFog << ", " <<  resourceReq << ", " <<  storageReq
//                    << ", " <<  0 << ", " <<  status << "\n" ;
//            predictLog.close();
            /*************************
            * Simple AHP Based Decision Ends
            ***************************/



            /***********
             *  RL Implemented Decision
             ****************/

            if((appType== 0 && resourceReq <= resourcePoolCont && storageReq<= storagePoolCont) || (appType== 1 && resourceReq <= lowestResourceDist && storageReq <= lowestStorageDist)){ // Continuous/Disrupted and Served
               chosenFog = myId;
               status = 1; // Assigned
            }

            else if(appType==0){
                chosenFog = chooseFogContRL(appID,  simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }

            else if(appType==1){
                chosenFog = chooseFogDist(appID, simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }

            // Inserting into application registry. // ID, type, time, rsu, servedRSU, resource, storage, deadline, status
            std::tuple<int, int, int, int, int, int, int, int, int,int, int> appDataRL (appType, simulationTime, appStart, appEnd,  myId, chosenFog,  resourceReq, storageReq, RDeadline,STime, status );
            edge.appRegistryRL[appID] = appDataRL;

//            //Logging Applications for RL
//            predictLog.open("results/phase2/rl/applications.csv",  ios::out | ios::app);
//            predictLog << appID << ", " << appType << ", " << simulationTime <<", "
//                    <<  appStart << ", " <<  appEnd << ", " <<  myId << ", "
//                    <<  chosenFog << ", " <<  resourceReq << ", " <<  storageReq
//                    << ", " <<  0 << ", " <<  status << "\n" ;
//            predictLog.close();

             /***********
             *  RL Implemented Decision Ends
             ****************/




            /***********
             *  RL Fuzzy Implemented Decision
             ****************/

            if((appType== 0 && resourceReq <= resourcePoolCont && storageReq<= storagePoolCont) || (appType== 1 && resourceReq <= lowestResourceDist && storageReq <= lowestStorageDist)){ // Continuous/Disrupted and Served
               chosenFog = myId;
               status = 1; // Assigned
            }

            else if(appType==0){
                chosenFog = chooseFogContRL(appID,  simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }

            else if(appType==1){
                chosenFog = chooseFogDist(appID, simulationTime, myId, resourceReq, storageReq, appStart, appEnd);
                if(chosenFog==0){
                    status = 2;
                }
                else{
                    status = 1;
                }
            }

            double SP=0;
            int counterFZ = 0;

            for (auto const& Request : edge.FuzzyRequestRegistry)
            {
                SP = SP + get<0>(Request.second);
                counterFZ++;
            }
            SP = SP/counterFZ;

//            std::cout<< "Average SP is - " << SP<<endl;


            // Inserting into application registry. // ID, type, time, rsu, servedRSU, resource, storage, deadline, status
            std::tuple<int, int, int, int, int, int, int, int, int, int, double, int > appDataFZ (appType, simulationTime, appStart, appEnd,  myId, chosenFog,  resourceReq, storageReq, RDeadline, STime,SP, status );
            edge.appRegistryFZ[appID] = appDataFZ;

//            std::cout<< "Average edge SP is - " << get<10>(edge.appRegistryFZ[appID])<<endl;


//            //Logging Applications for RL
//            predictLog.open("results/phase2/rl/applications.csv",  ios::out | ios::app);
//            predictLog << appID << ", " << appType << ", " << simulationTime <<", "
//                    <<  appStart << ", " <<  appEnd << ", " <<  myId << ", "
//                    <<  chosenFog << ", " <<  resourceReq << ", " <<  storageReq
//                    << ", " <<  0 << ", " <<  status << "\n" ;
//            predictLog.close();

             /***********
             *  RL Fuzzy Implemented Decision Ends
             ****************/









            appID++;

//            std::cout << "Application Status: " << status <<endl;
//            std::cout << "-----------------------" <<endl ;
//
//
//             Logging predictions
//            predictLog.open("results/dis2/predictions_random_25_05_linear_1000_400m_hop4_disrupted.csv",  ios::out | ios::app);
//            predictLog << myId << ", " << appStart << "," << appEnd << ","<< lowest/6 << "\n";
//            predictLog.close();
//
//
//             Logging volume in rsu database
//            edge.predictedVolume[myId][simulationTime] = volume;

        }

        scheduleAt(simTime() + 1 , request_event);

    }

    if(msg == dwellTime_event){


        if( simulationTime % 51 == 0 &&  simulationTime != 1000 && simulationTime > 200){

            int nVc = 3;
            int dwellTime;

            for(dwellTime = 0; dwellTime < 1000 - simulationTime; dwellTime++ ){

                int vehicleCount = 0;
                int volume = 0;

                for (auto const& vehicle : edge.msgRegistry[myId]){
                    if( (simulationTime + dwellTime) >= get<2>(vehicle.second) &&  (simulationTime + dwellTime) <= get<3>(vehicle.second) ){
                        vehicleCount++;
                    }
                }

                if(vehicleCount < nVc)
                    break;
            }

            //            std::cout << "Dwell time estimation at: " << simulationTime << "s. At RSU: " << myId << ". Dwell Time: " << dwellTime<<  endl ;
            //            std::cout << "--------------------------" << endl ;
            //
            //             Logging predictions
            //            predictLog.open("results/dis2/dwellTime_random_25_05_linear_1000_400m_hop4_disrupted.csv",  ios::out | ios::app);
            //            predictLog << myId << ", " << simulationTime << "," << dwellTime*3 << "\n";
            //            predictLog.close();
            //
            //             Logging to RSU database
            //
            //            edge.dtPrediction[myId][simulationTime] = dwellTime;
            //
            //            int vehicleCount = traci->get

        }

        scheduleAt(simTime() + 1 , dwellTime_event);
    }


    if(msg == rl_event){ // All the RL magic here. Updating R matrix and Q matrix.

        if( (simulationTime > 149 && myId == 14) &&  (simulationTime == 156 || simulationTime == 206 || simulationTime == 256 || simulationTime == 306
                || simulationTime == 356 || simulationTime == 406 || simulationTime == 456 || simulationTime == 506 || simulationTime == 556 || simulationTime == 606
                || simulationTime == 656 || simulationTime == 706 || simulationTime == 756 || simulationTime == 806 || simulationTime == 856  || simulationTime == 906
                || simulationTime == 956)){
//        if(simulationTime > 149 && myId == 14){

            // All the RL Stuff
//            std::cout<< "################################" << endl;
//            std::cout<< "This is an RL Training Event" << endl;

            int RLoriginFog;
            int RLchosenFog;
            int serveStatus;


            for(int i=appID-1; i >= appID-6; i--){

                // Check if app was assigned
                if(std::get<10>(edge.appRegistryRL[i])== 1){
                    RLoriginFog = std::get<4>(edge.appRegistryRL[i]);
                    RLchosenFog = std::get<5>(edge.appRegistryRL[i]);
//                    std::cout<< "Type: " << std::get<0>(edge.appRegistryRL[i]) << ", Status: 1, " << "Origin: " << RLoriginFog << ", Chosen Fog: " << RLchosenFog
//                            << ", Resource Req: " << std::get<6>(edge.appRegistryRL[i]) << ", Storage Req: " << std::get<7>(edge.appRegistryRL[i])
//                            << endl;

                    serveStatus = appStatusRL(i);
                    //appStatus(i);

                    if(serveStatus == 5){ //Failed
                        int originIndex = findIndex(RLoriginFog);
                        int chosenIndex = findIndex(RLchosenFog);
                        //std::cout<< "Assigned and Failed. Decreasing reward for: " << RLoriginFog << ", " <<RLchosenFog << ", ["<< originIndex << chosenIndex << "]"  <<  endl;
                        //rMatrix[originIndex][chosenIndex] = rMatrix[originIndex][chosenIndex] - 20 ;
                        rMatrix[originIndex][chosenIndex] = -1;
                        updateQMatrix(originIndex, chosenIndex);
                    }

                    else if(serveStatus == 7){ //Served
                        int originIndex = findIndex(RLoriginFog);
                        int chosenIndex = findIndex(RLchosenFog);
                        //std::cout<< "Assigned and Served. Increasing reward for: " << RLoriginFog << ", " <<RLchosenFog << ", ["<< originIndex << chosenIndex << "]"  <<  endl;
                        //rMatrix[originIndex][chosenIndex] = rMatrix[originIndex][chosenIndex] + 20 ;
                        rMatrix[originIndex][chosenIndex] = 100;
                        updateQMatrix(originIndex, chosenIndex);
                    }


                    //Logging Applications for RL
                    predictLog.open("results/phase2/applications_rl_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                    predictLog << i << ", " << std::get<0>(edge.appRegistryRL[i]) << ", " << std::get<1>(edge.appRegistryRL[i]) <<", "
                           <<  std::get<2>(edge.appRegistryRL[i]) << ", " <<  std::get<3>(edge.appRegistryRL[i]) << ", " <<  std::get<4>(edge.appRegistryRL[i]) << ", "
                           <<  std::get<5>(edge.appRegistryRL[i]) << ", " <<  std::get<6>(edge.appRegistryRL[i]) << ", " <<  std::get<7>(edge.appRegistryRL[i])
                           << ", " <<  std::get<8>(edge.appRegistryRL[i]) << ", " <<  std::get<9>(edge.appRegistryRL[i])<< ", " <<  serveStatus << "\n" ;
                    predictLog.close();

                // Changing Status so that it does not comeback to this again
                    int temp1 = serveStatus;
                    int temp2 = i;
                    std::tuple<int, int, int, int, int, int, int, int, int,int, int> appData (std::get<0>(edge.appRegistryRL[i]), std::get<1>(edge.appRegistryRL[i]), std::get<2>(edge.appRegistryRL[i]), std::get<3>(edge.appRegistryRL[i]), std::get<4>(edge.appRegistryRL[i]), std::get<5>(edge.appRegistryRL[i]), std::get<6>(edge.appRegistryRL[i]),std::get<7>(edge.appRegistryRL[i]), std::get<8>(edge.appRegistryRL[i]),std::get<9>(edge.appRegistryRL[i]), serveStatus );
                    edge.appRegistryRL.erase(temp2);
                    edge.appRegistryRL[temp2] = appData;


                }// App was assigned

                else{ // app was denied

                    predictLog.open("results/phase2/applications_rl_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                    predictLog << i << ", " << std::get<0>(edge.appRegistryRL[i]) << ", " << std::get<1>(edge.appRegistryRL[i]) <<", "
                           <<  std::get<2>(edge.appRegistryRL[i]) << ", " <<  std::get<3>(edge.appRegistryRL[i]) << ", " <<  std::get<4>(edge.appRegistryRL[i]) << ", "
                           <<  std::get<5>(edge.appRegistryRL[i]) << ", " <<  std::get<6>(edge.appRegistryRL[i]) << ", " <<  std::get<7>(edge.appRegistryRL[i])
                           << ", " <<  std::get<8>(edge.appRegistryRL[i]) << ", " <<  std::get<9>(edge.appRegistryRL[i])<< ", " <<  2 << "\n" ;
                    predictLog.close();

                } // App was denied

            }


            //std::cout << vehicleLocation.size() << endl;

            //std::cout<< "###############################" << endl;

        }

        scheduleAt(simTime() + 2 , rl_event);
    }


    ///////////////////////// RL Fuzzy Event HEre Start

    if(msg == rl_fz_event){ // All the RL magic here. Updating R matrix and Q matrix.

        if( (simulationTime > 149 && myId == 14) &&  (simulationTime == 156 || simulationTime == 206 || simulationTime == 256 || simulationTime == 306
                            || simulationTime == 356 || simulationTime == 406 || simulationTime == 456 || simulationTime == 506 || simulationTime == 556 || simulationTime == 606
                            || simulationTime == 656 || simulationTime == 706 || simulationTime == 756 || simulationTime == 806 || simulationTime == 856  || simulationTime == 906
                            || simulationTime == 956)){

//        if(simulationTime > 149 && myId == 14){

                        // All the RL Stuff
            //            std::cout<< "################################" << endl;
            //            std::cout<< "This is an RL Training Event" << endl;

                        int RLoriginFog;
                        int RLchosenFog;
                        int serveStatus;



                        for(int i=appID-1; i >= appID-6; i--){

                            // Check if app was assigned
                            if(std::get<11>(edge.appRegistryFZ[i])== 1){
                                RLoriginFog = std::get<4>(edge.appRegistryFZ[i]);
                                RLchosenFog = std::get<5>(edge.appRegistryFZ[i]);
            //                    std::cout<< "Type: " << std::get<0>(edge.appRegistryFZ[i]) << ", Status: 1, " << "Origin: " << RLoriginFog << ", Chosen Fog: " << RLchosenFog
            //                            << ", Resource Req: " << std::get<6>(edge.appRegistryFZ[i]) << ", Storage Req: " << std::get<7>(edge.appRegistryFZ[i])
            //                            << endl;

                                serveStatus = appStatusFZ(i);
                                //appStatus(i);

                                if(serveStatus == 5){ //Failed
                                    int originIndex = findIndex(RLoriginFog);
                                    int chosenIndex = findIndex(RLchosenFog);
                                    //std::cout<< "Assigned and Failed. Decreasing reward for: " << RLoriginFog << ", " <<RLchosenFog << ", ["<< originIndex << chosenIndex << "]"  <<  endl;
                                    //rMatrix[originIndex][chosenIndex] = rMatrix[originIndex][chosenIndex] - 20 ;
                                    fMatrix[originIndex][chosenIndex] = -1;
                                    updateQFMatrix(originIndex, chosenIndex);
                                }

                                else if(serveStatus == 7){ //Served
                                    int originIndex = findIndex(RLoriginFog);
                                    int chosenIndex = findIndex(RLchosenFog);
                                    //std::cout<< "Assigned and Served. Increasing reward for: " << RLoriginFog << ", " <<RLchosenFog << ", ["<< originIndex << chosenIndex << "]"  <<  endl;
                                    //rMatrix[originIndex][chosenIndex] = rMatrix[originIndex][chosenIndex] + 20 ;
                                    fMatrix[originIndex][chosenIndex] = 100;
                                    updateQFMatrix(originIndex, chosenIndex);
                                }


                                //Logging Applications for RL
                                predictLog.open("results/phase2/applications_fz_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                                predictLog << i << ", " << std::get<0>(edge.appRegistryFZ[i]) << ", " << std::get<1>(edge.appRegistryFZ[i]) <<", "
                                       <<  std::get<2>(edge.appRegistryFZ[i]) << ", " <<  std::get<3>(edge.appRegistryFZ[i]) << ", " <<  std::get<4>(edge.appRegistryFZ[i]) << ", "
                                       <<  std::get<5>(edge.appRegistryFZ[i]) << ", " <<  std::get<6>(edge.appRegistryFZ[i]) << ", " <<  std::get<7>(edge.appRegistryFZ[i])
                                       << ", " <<  std::get<8>(edge.appRegistryFZ[i]) << ", " <<  std::get<9>(edge.appRegistryFZ[i])<< ", " <<  std::get<10>(edge.appRegistryFZ[i]) << ", " <<  serveStatus << "\n" ;
                                predictLog.close();

                                // Changing Status so that it does not comeback to this again
                                int temp1 = serveStatus;
                                int temp2 = i;
                                std::tuple<int, int, int, int, int, int, int, int, int,int, double, int> appData (std::get<0>(edge.appRegistryFZ[i]), std::get<1>(edge.appRegistryFZ[i]), std::get<2>(edge.appRegistryFZ[i]), std::get<3>(edge.appRegistryFZ[i]), std::get<4>(edge.appRegistryFZ[i]), std::get<5>(edge.appRegistryFZ[i]), std::get<6>(edge.appRegistryFZ[i]),std::get<7>(edge.appRegistryFZ[i]), std::get<8>(edge.appRegistryFZ[i]),std::get<9>(edge.appRegistryFZ[i]),std::get<10>(edge.appRegistryFZ[i]), serveStatus );
                                edge.appRegistryFZ.erase(temp2);
                                edge.appRegistryFZ[temp2] = appData;



                            } // App was assigned

                            else{ // app was denied

                                predictLog.open("results/phase2/applications_fz_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                                predictLog << i << ", " << std::get<0>(edge.appRegistryFZ[i]) << ", " << std::get<1>(edge.appRegistryFZ[i]) <<", "
                                       <<  std::get<2>(edge.appRegistryFZ[i]) << ", " <<  std::get<3>(edge.appRegistryFZ[i]) << ", " <<  std::get<4>(edge.appRegistryFZ[i]) << ", "
                                       <<  std::get<5>(edge.appRegistryFZ[i]) << ", " <<  std::get<6>(edge.appRegistryFZ[i]) << ", " <<  std::get<7>(edge.appRegistryFZ[i])
                                       << ", " <<  std::get<8>(edge.appRegistryFZ[i]) << ", " <<  std::get<9>(edge.appRegistryFZ[i])<< ", " <<  std::get<10>(edge.appRegistryFZ[i]) << ", "<< 2 << "\n" ;
                                predictLog.close();

                            } // App was denied

                        }


                        //std::cout << vehicleLocation.size() << endl;

                        //std::cout<< "###############################" << endl;

                    }

                    scheduleAt(simTime() + 2 , rl_fz_event);
        }


/////////////////// RL FZ EVent End;


    if(msg == app_status){ // Prediodical app status check and logging

        if( (simulationTime > 149 && myId == 14) &&  (simulationTime == 158 || simulationTime == 208 || simulationTime == 258 || simulationTime == 308
                || simulationTime == 358 || simulationTime == 408 || simulationTime == 458 || simulationTime == 508 || simulationTime == 558 || simulationTime == 608
                || simulationTime == 658 || simulationTime == 708 || simulationTime == 758 || simulationTime == 808 || simulationTime == 858  || simulationTime == 908
                || simulationTime == 958)){

//        if(simulationTime > 149 && myId == 14){

            // All the RL Stuff
//            std::cout<< "################################" << endl;
//            std::cout<< "This is an app logging event" << endl;

            int originFog;
            int chosenFog;
            int serveStatus;


            for(int i=appID-1; i >= appID-6; i--){

                // Check if app was assigned
                if(std::get<10>(edge.appRegistry[i])== 1){
                    originFog = std::get<4>(edge.appRegistry[i]);
                    chosenFog = std::get<5>(edge.appRegistry[i]);
//                    std::cout<< "Type: " << std::get<0>(edge.appRegistry[i]) << ", Status: 1, " << "Origin: " << originFog << ", Chosen Fog: " << chosenFog
//                            << ", Resource Req: " << std::get<6>(edge.appRegistry[i]) << ", Storage Req: " << std::get<7>(edge.appRegistry[i])
//                            << endl;

                    serveStatus = appStatus(i);


                    //Logging Applications for RL
                    predictLog.open("results/phase2/applications_ahp_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                    predictLog << i << ", " << std::get<0>(edge.appRegistry[i]) << ", " << std::get<1>(edge.appRegistry[i]) <<", "
                           <<  std::get<2>(edge.appRegistry[i]) << ", " <<  std::get<3>(edge.appRegistry[i]) << ", " <<  std::get<4>(edge.appRegistry[i]) << ", "
                           <<  std::get<5>(edge.appRegistry[i]) << ", " <<  std::get<6>(edge.appRegistry[i]) << ", " <<  std::get<7>(edge.appRegistry[i])
                           << ", " <<  std::get<8>(edge.appRegistry[i]) << ", " <<  std::get<9>(edge.appRegistry[i]) << ", " << serveStatus << "\n" ;
                    predictLog.close();

                    // Changing Status so that it does not comeback to this again
                    int temp1 = serveStatus;
                    int temp2 = i;
                    std::tuple<int, int, int, int, int, int, int, int, int,int, int> appData (std::get<0>(edge.appRegistry[i]), std::get<1>(edge.appRegistry[i]), std::get<2>(edge.appRegistry[i]), std::get<3>(edge.appRegistry[i]), std::get<4>(edge.appRegistry[i]), std::get<5>(edge.appRegistry[i]), std::get<6>(edge.appRegistry[i]),std::get<7>(edge.appRegistry[i]), std::get<8>(edge.appRegistry[i]),std::get<9>(edge.appRegistry[i]), serveStatus );
                    edge.appRegistry.erase(temp2);
                    edge.appRegistry[temp2] = appData;



                } // App was assigned

                else{ // app was denied

                    predictLog.open("results/phase2/applications_ahp_"+std::to_string(numberVehicles)+".csv",  ios::out | ios::app);
                    predictLog << i << ", " << std::get<0>(edge.appRegistry[i]) << ", " << std::get<1>(edge.appRegistry[i]) <<", "
                           <<  std::get<2>(edge.appRegistry[i]) << ", " <<  std::get<3>(edge.appRegistry[i]) << ", " <<  std::get<4>(edge.appRegistry[i]) << ", "
                           <<  std::get<5>(edge.appRegistry[i]) << ", " <<  std::get<6>(edge.appRegistry[i]) << ", " <<  std::get<7>(edge.appRegistry[i])
                           << ", " <<  std::get<8>(edge.appRegistry[i]) << ", " <<  std::get<9>(edge.appRegistry[i])<< ", " <<  2 << "\n" ;
                    predictLog.close();

                } // App was denied

            }


            //std::cout << vehicleLocation.size() << endl;

            //std::cout<< "###############################" << endl;

        }

        scheduleAt(simTime() + 2 , app_status);
    }




    if(msg == traffic_flow_event && myId == 24){ //Estimating traffic flow

        int simulationTime = (int) simTime().dbl();

        /*
        std::cout<< "Traffic Flow Event at: " << myId << ". Simulation Time: " << simulationTime <<endl;
        double actualVolume = edge.inRangeVehicle[myId][simulationTime].size();

        int* a = &edge.inRangeVehicle[myId][simulationTime-4][0];

        //for(int i=0; i < edge.inRangeVehicle[myId][simulationTime-4].size(); i++)
         //   std::cout << a[i] << ' ';
       // std::cout<< endl;

        sort(edge.inRangeVehicle[myId][simulationTime-4].begin(), edge.inRangeVehicle[myId][simulationTime-4].end());
        sort(edge.inRangeVehicle[myId][simulationTime-3].begin(), edge.inRangeVehicle[myId][simulationTime-3].end());
        sort(edge.inRangeVehicle[myId][simulationTime-2].begin(), edge.inRangeVehicle[myId][simulationTime-2].end());
        sort(edge.inRangeVehicle[myId][simulationTime-1].begin(), edge.inRangeVehicle[myId][simulationTime-1].end());

        for(int i=0; i < edge.inRangeVehicle[myId][simulationTime-4].size(); i++)
                    std::cout << edge.inRangeVehicle[myId][simulationTime-4].at(i) << ' ';

        std::cout<< endl;

        for(int i=0; i < edge.inRangeVehicle[myId][simulationTime-3].size(); i++)
           std::cout << edge.inRangeVehicle[myId][simulationTime-3].at(i) << ' ';

        std::cout<< endl;

        for(int i=0; i < edge.inRangeVehicle[myId][simulationTime-2].size(); i++)
           std::cout << edge.inRangeVehicle[myId][simulationTime-2].at(i) << ' ';

        std::cout<< endl;

        for(int i=0; i < edge.inRangeVehicle[myId][simulationTime-1].size(); i++)
           std::cout << edge.inRangeVehicle[myId][simulationTime-1].at(i) << ' ';

        std::cout<< endl;

        std::cout<< edge.inRangeVehicle[myId][simulationTime-1].size() << endl;
        std::cout<< edge.inRangeVehicle[myId][simulationTime-2].size() << endl;
        std::cout<< "--------------" << endl;

        scheduleAt(simTime() + 10, traffic_flow_event);
        */
    }

}


void TraCIDemoRSU11p:: onBSM(DemoSafetyMessage* bsm)
{

}

void TraCIDemoRSU11p::finish()
{
    DemoBaseApplLayer::finish();
    //std::cout<<"SimTime: " << simTime().dbl() << " myID: " << myId << endl;
    if(simTime().dbl() == 1000 && myId == 24){
        //std::cout << "In Range Received: " << inRangeMsgRcv << endl;
        std::cout << "WSM Received: " << wsmReceived << endl;

        for(int i = 0; i< 6; i++){
            std::cout<< "{";
            for(int j= 0; j< 6; j++){
                std::cout<< rMatrix[i][j] << ", ";
            }
            std::cout<< "},";
            std::cout<< endl;
        }

        for(int i = 0; i< 6; i++){

            for(int j= 0; j< 6; j++){
                std::cout<< qMatrix[i][j] << ", ";
            }
            std::cout<< endl;
        }


//        for (auto const& Request : edge.FuzzyRequestRegistry)
//        {
//           std::cout<< Request.first<<" and Status: "<< get<3>(Request.second)<<endl;
//        }


        remove("RSUFuzzy.txt");
        remove("Distance.txt");
        remove("Connection.txt");
        remove("Time.txt");
        remove("Requester.txt");
        std::cout<<"QHigh:"<<endl;
        showQ(QHigh);
        std::cout<<"QLow:"<<endl;
        showQ(QLow);
    }

    /*for(auto itr : edge.appRegistry){
        cout << std::get<0>(itr) << ", " << std::get<1>(itr)<< ", "
                << std::get<2>(itr)<< ", " << std::get<3>(itr) << ", "
                << std::get<4>(itr)<< ", " << std::get<5>(itr)<< ", "
                << std::get<6>(itr)<<  ", "include <climits> <<std::get<7>(itr) <<  ", " <<std::get<8>(itr) <<  endl ;

        cout << "-------------7: 49
5: 47
2: 6
        -----------" << endl;

    }*/

    //// For Fuzzy Short files that are created throughout the compilation of code


}

