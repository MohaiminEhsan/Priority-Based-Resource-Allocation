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

#include <veins/modules/application/traci/Registry.h>
#include <veins/modules/application/traci/CommonVars.h>
#include "veins/modules/application/traci/TraCIDemo11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/mlr.h"


#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <random>

using namespace std;
using namespace veins;


Define_Module(veins::TraCIDemo11p);
TraCICommandInterface* traci;
TraCIScenarioManager* scenario;

Registry registry;
int NodeRange = 400;
int inRangeMsgSent = 0;
int wsmSent = 0;
int hop = 4;
int vCount = 1000;

std::vector<std::tuple<int, int, int>> vehicleLocation;
std::map<int, std::tuple<int, int>> vehicleResource;


double linearDistance(Coord& a,  Coord& b) {
    Coord dist(a - b);
    return dist.length();
}


using Point = std::pair<double, double>;
constexpr auto eps = 1e-14;

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

double sq(double x) {
    return x * x;
}

std::vector<Point> intersects(const Point &p1, const Point &p2, const Point &cp, double r, bool segment) {
    std::vector<Point> res;
    auto x0 = cp.first;
    auto y0 = cp.second;
    auto x1 = p1.first;
    auto y1 = p1.second;
    auto x2 = p2.first;
    auto y2 = p2.second;
    auto A = y2 - y1;
    auto B = x1 - x2;
    auto C = x2 * y1 - x1 * y2;
    auto a = sq(A) + sq(B);
    double b, c;
    bool bnz = true;
    if (abs(B) >= eps) {
        b = 2 * (A * C + A * B * y0 - sq(B) * x0);
        c = sq(C) + 2 * B * C * y0 - sq(B) * (sq(r) - sq(x0) - sq(y0));
    } else {
        b = 2 * (B * C + A * B * x0 - sq(A) * y0);
        c = sq(C) + 2 * A * C * x0 - sq(A) * (sq(r) - sq(x0) - sq(y0));
        bnz = false;
    }
    auto d = sq(b) - 4 * a * c; // discriminant
    if (d < 0) {
        return res;
    }

    // checks whether a point is within a segment
    auto within = [x1, y1, x2, y2](double x, double y) {
        auto d1 = sqrt(sq(x2 - x1) + sq(y2 - y1));  // distance between end-points
        auto d2 = sqrt(sq(x - x1) + sq(y - y1));    // distance from point to one end
        auto d3 = sqrt(sq(x2 - x) + sq(y2 - y));    // distance from point to other end
        auto delta = d1 - d2 - d3;
        return abs(delta) < eps;                    // true if delta is less than a small tolerance
    };

    auto fx = [A, B, C](double x) {
        return -(A * x + C) / B;
    };

    auto fy = [A, B, C](double y) {
        return -(B * y + C) / A;
    };

    auto rxy = [segment, &res, within](double x, double y) {
        if(!isnan(x) && !isnan(y)){ // removing NaN
            if (!segment || within(x, y)) {
                res.push_back(std::make_pair(x, y));
            }
        }

    };

    

    double x, y;
    if (d == 0.0) {
        // line is tangent to circle, so just one intersect at most
        if (bnz) {
            x = -b / (2 * a);
            y = fx(x);
            rxy(x, y);
        } else {
            y = -b / (2 * a);
            x = fy(y);
            rxy(x, y);
        }
    } else {
        // two intersects at most
        d = sqrt(d);
        if (bnz) {
            x = (-b + d) / (2 * a);
            y = fx(x);
            rxy(x, y);
            x = (-b - d) / (2 * a);
            y = fx(x);
            rxy(x, y);
        } else {
            y = (-b + d) / (2 * a);
            x = fy(y);
            rxy(x, y);
            y = (-b - d) / (2 * a);
            x = fy(y);
            rxy(x, y);
        }
    }
    return res;
}


double calculateVelocity(Coord& a,  Coord& b, double latestTime, double oldestTime){
    Coord dist(a - b);
    return dist.length()/(latestTime-oldestTime);
}


// Fires for every car loading.
void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        inRange = new cMessage("inRange");
        scheduleAt(simTime() + 1, inRange);
        stage++;
    }
    std::ofstream outfile ("Requester.txt");
    outfile.close();

}


tuple<int, double> findNearestEdge(Coord vehicleCoord){

    double lowest=35000; // set to max map size.
    int nearest;
    double runningDistance;

    for (auto const& rsu : registry.rsuRegistry)
    {
        Coord dist(vehicleCoord - rsu.second);
        runningDistance = abs( sqrt( pow(vehicleCoord.x - rsu.second.x,2) + pow(vehicleCoord.y - rsu.second.y, 2) ) - NodeRange );

        if(runningDistance < lowest){
            lowest= runningDistance;
            nearest = rsu.first;
        }
    }

    return make_tuple(nearest, lowest);
}


///// For Fuzzy based priority model Start

int RandomDeadLineGenerate(double initTime)
{
    int SimMaxTime = 1000;
    int min = int (initTime);
    int max = SimMaxTime;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range
    int random = distr(gen); // generate numbers
    return random;
}


// A quick way to split strings separated via spaces.
std::string simple_tokenizer(string s)
{
    stringstream ss(s);
    std::string word;
    std::string word1;
    ss >> word;
    ss >> word1;
    return word1;
}


double PoissonDistributionNumberGenerator(double AverageNumber)
{
    // seed the RNG
    std::random_device rd; // uniformly-distributed integer random number generator
    std::mt19937 rng (rd ()); // mt19937: Pseudo-random number generation
    //double lamda = 1 / AverageNumber;
    std::exponential_distribution<double> exp (AverageNumber);
    double backofftime;
    backofftime=  exp.operator() (rng);// generates the next random number in the distribution

    //return backofftime;
    return AverageNumber;
}


bool RequesterSelection(int Vid)
{
    std::list<int> VList;
    std::string line;
    std::string VFileName("Requester.txt");
    ofstream VFileOut;
    std::ifstream VFileIn("Requester.txt");

    VFileOut.open(VFileName, std::ios_base::app);
    if (VFileIn.is_open())
    {

    while(std::getline(VFileIn, line))
    {
        VList.push_back(std::stoi(line));
    }
    VFileIn.close();
    }

    bool flag = false;
    std::list<int>::const_iterator it;
    for(it = VList.begin(); it != VList.end(); it++)
    {
        if (Vid == *it)
        {
            flag = true;
            break;
        }
    }
    if (flag == true)
    {
        return false;
    }
    else{
//        int RequestCounter = 0;
//        int TotalVehilceSoFar = VList.size();
        int ReqFinalResult = 0;
//        std::list<std::string>::const_iterator it;
//        for(it = VList.begin(); it != VList.end(); it++)
//        {
//            std::string ReqResult = simple_tokenizer(*it);
//            int reqRes = stoi(ReqResult);
//            if (reqRes == 1){
//                RequestCounter++;
//            }
//        }
        double testerValue = PoissonDistributionNumberGenerator(1.00);
        if (testerValue >= 1.00)
        {
            ReqFinalResult = 1;
        }else{
            ReqFinalResult = 0;
        }
        if (VFileOut.is_open()){
            VFileOut << Vid << endl;
        }
        if (ReqFinalResult == 1)
        {
            return true;
        }
        else if (ReqFinalResult == 0)
        {
            return false;
        }
    }

}


///// For Fuzzy based priority model End



// About the vehicle position. Everytime the vehicle changes position this is fired.
void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{

    DemoBaseApplLayer::handlePositionUpdate(obj);

    Coord vehicleCoord = mobility->getPositionAt(simTime().dbl());
    int vehicleID = mobility->getId();
    int simulationTime = (int) simTime().dbl();


    if(((int) simTime().dbl()) % (rand() % 6 + 13) == 0 ){

    //if(((int) simTime().dbl()) % 60 == 0 ){

        // Getting vehicle data

        double vehicleSpeed = mobility->getSpeed();
        double simulationTime = simTime().dbl();
        int availableResource=0;
        int availableStorage=0;


        // Creating tuple of information
        std::tuple<veins::Coord, double, double> vehicleData (vehicleCoord, vehicleSpeed, simulationTime);

        // Adding information into vehicle registry.
        registry.vehicleRegistry[vehicleID].insert(registry.vehicleRegistry[vehicleID].begin(), vehicleData) ;

        // Removing last data
        int size = registry.vehicleRegistry[vehicleID].size();

        //std::cout<<vehicleID << endl;


        if(size > 3){ // If we have at least 5 historic data points to work with

            // Checking if path crosses any RSU range in any way
            for (auto const& rsu : registry.rsuRegistry)
            {
                Coord currentDist(rsu.second - vehicleCoord);

                if(currentDist.length() < NodeRange){ // within range

                    //#include <queue>
                    // Finding the exit point.
                    //

                    //std::cout << "vehicle in range" << endl;

                    Coord oldest = get<0>(registry.vehicleRegistry[vehicleID][size-1]) ;
                    auto cp = std::make_pair(rsu.second.x, rsu.second.y);
                    // Preparing data for x y training for path projection
                    std::vector<std::vector<double>> xCoordTrain;
                    std::vector<double> yCoordTrain;

                    for(int j=0; j < size; j++){
                       Coord current = get<0>(registry.vehicleRegistry[vehicleID][j]);
                       xCoordTrain.push_back({current.x});
                       yCoordTrain.push_back(current.y);
                    }

                    LinearRegression trajectory(xCoordTrain, yCoordTrain, NODEBUG);
                    trajectory.fit();

                    double p1y = trajectory.predict({vehicleCoord.x});
                    double p2y = trajectory.predict({oldest.x});
                    auto p1 = std::make_pair(vehicleCoord.x, p1y);
                    auto p2 = std::make_pair(oldest.x, p2y);

                    std::vector<Point> output = intersects(p1, p2, cp, NodeRange, false);

                    if(!output.empty()) { // We got some intersecting points


                        Coord entry;
                        Coord exit;
                        Coord a(get<0>(output[0]), get<1>(output[0])); // Intersecting point.
                        Coord b(get<0>(output[1]), get<1>(output[1])); // Intersecting point.

                        if(linearDistance(a,oldest) < linearDistance(b, oldest)){
                            entry = a;
                            exit = b;
                        }
                        else{
                            entry = b;
                            exit = a;
                        }


                        //double oldestTime = get<2>(registry.vehicleRegistry[vehicleID][size-1]) ;

                        double totalSpeed = 0;
                        int count = 0;

                        std::vector<std::vector<double>> X;
                        std::vector<double> Y;

                        for(int j=0; j < size; j++){
                            totalSpeed += std::get<1>(registry.vehicleRegistry[vehicleID][j]);
                            count += 1;

                            Coord current = get<0>(registry.vehicleRegistry[vehicleID][j]);
                            Coord next = get<0>(registry.vehicleRegistry[vehicleID][j+1]);

                            if(j < size-1){
                               std::vector<double> xTrain = {linearDistance(current, next), (get<1>(registry.vehicleRegistry[vehicleID][j]) + get<1>(registry.vehicleRegistry[vehicleID][j+1])) / 2 };
                               //std::vector<double> xTrain = {traci->getDistance(current, next, true), (get<1>(registry.vehicleRegistry[vehicleID][j]) + get<1>(registry.vehicleRegistry[vehicleID][j+1])) / 2 };
                                X.push_back(xTrain);
                                Y.push_back(get<2>(registry.vehicleRegistry[vehicleID][j]) - get<2>(registry.vehicleRegistry[vehicleID][j+1]));
                            }
                        }


                        LinearRegression mlr(X, Y, NODEBUG);
                        mlr.fit();

                        double averageSpeed = totalSpeed/count;
                        double dwellDistance = linearDistance(vehicleCoord, exit);
                        //double dwellDistance = traci->getDistance(vehicleCoord, exit, true);

                        int dwellTime = mlr.predict({dwellDistance, averageSpeed});

                        if ( vehicleResource.count(vehicleID) == 0 ) { // Not Present
                            availableResource = rand() % 10 + 0;
                            availableStorage = rand() % 10 + 0;
                            std::tuple<int, int> resourceData (availableResource, availableStorage);
                            vehicleResource[vehicleID] =  resourceData;
                            // Logging Resource
                            //vehicleLog.open("results/phase2/vehicleResource_"+std::to_string(vCount)+".csv",  ios::out | ios::app);
                           // vehicleLog << vehicleID << ", " << availableResource << ", " << availableStorage <<  "\n";
                            //vehicleLog.close();
                        } else {
                            availableResource = get<0>(vehicleResource[vehicleID]);
                            availableStorage = get<1>(vehicleResource[vehicleID]);
                        }

                        //std::cout << availableStorage << endl;

                        // Sending a WSM Message to the nextRSU
                        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                        populateWSM(wsm);
                        wsm->setSenderType(1);
                        wsm->setSenderAddress(vehicleID);
                        wsm->setMessageTime((int)simTime().dbl());
                        wsm->setTargetAddress(rsu.first);
                        wsm->setSenderPositionRLDCO(vehicleCoord);
                        wsm->setExitCoord(exit);
                        wsm->setTimeToReach(0); // Already in range
                        wsm->setDwellDistance(dwellDistance);
                        wsm->setDwellTime(dwellTime);
                        wsm->setHopCountRLDCO(99);

                        wsm->setAvailableResource(availableResource);
                        wsm->setAvailableStorage(availableStorage);
                        double MessageInitTime=simTime().dbl();
                        wsm->setMessageInitTime(MessageInitTime);
                        int RequestDeadLine = RandomDeadLineGenerate(MessageInitTime);
                        wsm->setRequestDeadline(RequestDeadLine);
                        sendDelayedDown(wsm, uniform(0.01,1.0));
                        bool Requester = RequesterSelection(vehicleID);
                        wsm->setRequester(Requester);
                        wsmSent++;

                    }

                } // If within Range


                else{ // If not within range

                    Coord oldest = get<0>(registry.vehicleRegistry[vehicleID][size-1]) ;
                    //          first element is coord          id for map    last tuple
                    Coord initDist(rsu.second - oldest);

                    if(initDist.length() > currentDist.length()){ // vehicle moving towards the RSU

                        if(currentDist.length() < NodeRange* hop){

                            auto cp = std::make_pair(rsu.second.x, rsu.second.y);

                            // Preparing data for x y training for path projection
                            std::vector<std::vector<double>> xCoordTrain;
                            std::vector<double> yCoordTrain;

                            for(int j=0; j < size; j++){
                               Coord current = get<0>(registry.vehicleRegistry[vehicleID][j]);
                               xCoordTrain.push_back({current.x});
                               yCoordTrain.push_back(current.y);
                            }

                            LinearRegression trajectory(xCoordTrain, yCoordTrain, NODEBUG);
                            trajectory.fit();

                            double p1y = trajectory.predict({vehicleCoord.x});
                            double p2y = trajectory.predict({oldest.x});
                            //auto p1 = std::make_pair(vehicleCoord.x, vehicleCoord.y);
                            //auto p2 = std::make_pair(oldest.x, oldest.y);
                            auto p1 = std::make_pair(vehicleCoord.x, p1y);
                            auto p2 = std::make_pair(oldest.x, p2y);


                            std::vector<Point> output = intersects(p1, p2, cp, NodeRange, false);

                            if(!output.empty()) {

                                Coord entry;
                                Coord exit;
                                Coord a(get<0>(output[0]), get<1>(output[0])); // Intersecting point.
                                Coord b(get<0>(output[1]), get<1>(output[1])); // Intersecting point.

                                if(linearDistance(a,vehicleCoord) < linearDistance(b, vehicleCoord)){
                                    entry = a;
                                    exit = b;
                                }
                                else{
                                    entry = b;
                                    exit = a;
                                }

                                double oldestTime = get<2>(registry.vehicleRegistry[vehicleID][size-1]) ;
                                //double velocity = calculateVelocity(vehicleCoord, oldest, simulationTime, oldestTime);

                                double totalSpeed = 0;
                                int count = 0;

                                std::vector<std::vector<double>> X;
                                std::vector<double> Y;

                                for(int j=0; j < size; j++){
                                    totalSpeed += std::get<1>(registry.vehicleRegistry[vehicleID][j]);
                                    count += 1;

                                    Coord current = get<0>(registry.vehicleRegistry[vehicleID][j]);
                                    Coord next = get<0>(registry.vehicleRegistry[vehicleID][j+1]);

                                    if(j < size-1){
                                        std::vector<double> xTrain = {linearDistance(current, next), (get<1>(registry.vehicleRegistry[vehicleID][j]) + get<1>(registry.vehicleRegistry[vehicleID][j+1])) / 2 };
                                        //std::vector<double> xTrain = {traci->getDistance(current, next,true), (get<1>(registry.vehicleRegistry[vehicleID][j]) + get<1>(registry.vehicleRegistry[vehicleID][j+1])) / 2 };
                                        X.push_back(xTrain);
                                        Y.push_back(get<2>(registry.vehicleRegistry[vehicleID][j]) - get<2>(registry.vehicleRegistry[vehicleID][j+1]));
                                    }
                                }


                                LinearRegression mlr(X, Y, NODEBUG);
                                mlr.fit();

                                double averageSpeed = totalSpeed/count;

                                double entryDistance = linearDistance(vehicleCoord, entry);
                                //double entryDistance = traci->getDistance(vehicleCoord, entry, true);

                                double dwellDistance = linearDistance(entry, exit);
                                //double dwellDistance = traci->getDistance(entry, exit, true);

                                int timeToReach = mlr.predict({ entryDistance, averageSpeed  });
                                int dwellTime = mlr.predict({dwellDistance, averageSpeed});


                                /*double drivingDistance;
                                std::cout<< "Calculating driving distance: " <<endl;
                                try{
                                   drivingDistance = traci->getDistance(entry, exit, true);
                                }
                                catch (const std::exception &exc)
                                {
                                    // catch anything thrown within try block that derives from std::exception
                                    std::cerr << exc.what();
                                }
                                */


                                /*
                                std::cout<< averageSpeed << endl;
                                std::cout<< "Entry Distance: " << entryDistance << endl;
                                std::cout << "Time to Reach: " <<  timeToReach << endl;


                                std::cout<<  "Dwell Distance: " << dwellDistance << endl;
                                std::cout<< "Dwell time: " <<  dwellTime << endl;
                                std::cout << "-----------------" << endl;
                                */

                                if ( vehicleResource.count(vehicleID) == 0 ) { // Not Present
                                    availableResource = rand() % 10 + 0;
                                    availableStorage = rand() % 10 + 0;
                                    std::tuple<int, int> resourceData (availableResource, availableStorage);
                                    vehicleResource[vehicleID] =  resourceData;
                                    // Logging Resource
                                    //vehicleLog.open("results/phase2/vehicleResource_"+std::to_string(vCount)+".csv",  ios::out | ios::app);
                                   // vehicleLog << vehicleID << ", " << availableResource << ", " << availableStorage <<  "\n";
                                   // vehicleLog.close();
                                } else {
                                    availableResource = get<0>(vehicleResource[vehicleID]);
                                    availableStorage = get<1>(vehicleResource[vehicleID]);
                                }


                                // Sending a WSM Message to the nextRSU
                                TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                                populateWSM(wsm);
                                wsm->setSenderType(1);
                                wsm->setSenderAddress(vehicleID);
                                wsm->setMessageTime((int)simTime().dbl());
                                wsm->setTargetAddress(rsu.first);
                                wsm->setTargetCoord(rsu.second);
                                wsm->setSenderSpeedRLDCO(vehicleSpeed);
                                wsm->setAverageSpeed(averageSpeed);
                                wsm->setSenderPositionRLDCO(vehicleCoord);
                                wsm->setNeighborChain(std::to_string(vehicleID).c_str());

                                wsm->setEntryCoord(entry);
                                wsm->setExitCoord(exit);
                                wsm->setTimeToReach(timeToReach);
                                wsm->setDwellDistance(dwellDistance);
                                wsm->setDwellTime(dwellTime);
                                wsm->setHopCountRLDCO(0);
                                wsm->setAvailableResource(availableResource);
                                sendDelayedDown(wsm, uniform(0.01,0.09));
                                wsm->setAvailableStorage(availableStorage);
                                double MessageInitTime = simTime().dbl();
                                wsm->setMessageInitTime(MessageInitTime);
                                int RequestDeadLine = RandomDeadLineGenerate(MessageInitTime);
                                wsm->setRequestDeadline(RequestDeadLine);
                                bool Requester = RequesterSelection(vehicleID);
                                wsm->setRequester(Requester);
                                wsmSent++;
                                //std::cout<< "WSM Sent" << endl;
                                //std::cout<< "-------------------------" << endl;

                            } // If intersects

                        }// if within 4000

                    } // if moving towards

                } // if out of range

            } // For each known rsu

            if( size > 15){
               registry.vehicleRegistry[vehicleID].pop_back();
            }

        } // if size > 4

    } // at specific simtime

}



void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{

}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{


    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    int hopCount = wsm->getHopCountRLDCO();

    // Ignoring self wsm
    if(wsm->getSenderAddress() != mobility->getId()){

        // If wsm from another car
        if(  wsm->getSenderType() == 1){

            if(hopCount < hop){

                Coord senderCoord = wsm->getSenderPositionRLDCO();
                Coord targetCoord = wsm->getTargetCoord();
                Coord vehicleCoord = mobility->getPositionAt(simTime().dbl());
                //int vehicleID = wsm->getSenderAddress();
                //float vehicleSpeed = wsm->getSenderSpeedRLDCO();
                //if(linearDistance(vehicleCoord, targetCoord ) < linearDistance(senderCoord, targetCoord)){
                if(true){
                    string currentChain = wsm->getNeighborChain();
                    string s = std::to_string(mobility->getId());
                    //if(currentChain.find(s) != string::npos){
                    if(true){
                        string newChain = currentChain +','+ s;
                        wsm->setNeighborChain(newChain.c_str());
                        wsm->setHopCountRLDCO(hopCount+1);
                        sendDelayedDown(wsm->dup(), uniform(.01,0.09));
                        //std::cout << "Flooding this" << endl ;
                    }

                    else{
                        //std::cout << "Already flooded this wsm" << endl ;
                    }

                }

            }

        }

        else{ // If WSM from a RSU

            if(wsm->getInRange() == true ){ // vehicle in range of an rsu

                    /*
                  int rsuID = wsm->getSenderAddress();
                  int vehicleID = mobility->getId();
                  int simulationTime = (int) simTime().dbl();

                  // Logging locations
                  vehicleLog.open("results/vehicleLog_random_25_05.csv",  ios::out | ios::app);
                  vehicleLog << mobility->getId() << ", " << simulationTime << ", " << rsuID <<  "\n";
                  vehicleLog.close();

                  // Sending a WSM Message to the nextRSU
                  TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                  populateWSM(wsm);
                  wsm->setSenderType(1);
                  wsm->setSenderAddress(vehicleID);
                  wsm->setMessageTime(simulationTime);
                  wsm->setTargetAddress(rsuID);
                  wsm->setHopCountRLDCO(99);
                  wsm->setInRange(true);
                  sendDown(wsm);
                  */
            }

            else{ // may not be in range.

                Coord rsuCoord = wsm->getSenderPositionRLDCO();
                int rsuID = wsm->getSenderAddress();

                // Adding to RSU Registry if not already present
                if ( registry.rsuRegistry.find(rsuID) == registry.rsuRegistry.end() ) {
                    registry.rsuRegistry[rsuID] = rsuCoord ;
                }

                if(wsm->getHopCountRLDCO() < hop){
                    wsm->setHopCountRLDCO(hopCount+1);
                    sendDelayedDown(wsm->dup(), uniform(0.01,.09)); // waits delay seconds before sending ? this avoids packet collision. uniform creates a random delay
                }
            }
        }
    }


} // onWSM

void TraCIDemo11p:: onBSM(DemoSafetyMessage* bsm)
{

}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{

    //DemoBaseApplLayer::handleSelfMsg(msg);

    if(msg == inRange){

        Coord vehicleCoord = mobility->getPositionAt(simTime().dbl());
        int vehicleID = mobility->getId();
        int simulationTime = (int) simTime().dbl();


        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setSenderType(1);
        wsm->setSenderAddress(vehicleID);
        wsm->setMessageTime(simulationTime);
        //wsm->setTargetAddress(rsu.first);
        wsm->setHopCountRLDCO(99);
        wsm->setInRange(true);
        sendDelayedDown(wsm, uniform(0.01, .09));


        if(simulationTime > 149){


            for (auto const& rsu : registry.rsuRegistry)
            {
                Coord currentDist(rsu.second - vehicleCoord);

               if(currentDist.length() < NodeRange){

                   //std::cout<<vehicleID << endl;
                  // Sending a WSM Message to the current rsu
                  /*TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                  populateWSM(wsm);
                  wsm->setSenderType(1);
                  wsm->setSenderAddress(vehicleID);
                  wsm->setMessageTime(simulationTime);
                  wsm->setTargetAddress(rsu.first);
                  wsm->setHopCountRLDCO(99);
                  wsm->setInRange(true);
                  sendDelayedDown(wsm, uniform(0.01, 1.0));
                   */
                  //inRangeMsgSent++;
                  //wsmSent++;

                  // Logging locations
                   //vehicleLog.open("results/phase2/vehicleLog_"+std::to_string(vCount)+".csv",  ios::out | ios::app);
                 //  vehicleLog << vehicleID << ", " << simulationTime << ", " << rsu.first <<  "\n";
                  // vehicleLog.close();
                   std::tuple<int, int, int> locationData (vehicleID, simulationTime, rsu.first);
                   vehicleLocation.push_back(locationData);

                  //break;

                }


            }

        }

        scheduleAt(simTime() + 1, inRange);

    }

}

void TraCIDemo11p::finish()
{
    DemoBaseApplLayer::finish();

    //std::cout << simTime().dbl() << endl ;

    std::cout << myId << endl ;
    std::cout <<"WSM Sent: " << wsmSent << endl;
    if(simTime().dbl() == 1000 && myId == 507){
        std::cout << myId << endl ;
        //std::cout << "In range Message Sent: " << inRangeMsgSent << endl;
        std::cout <<"WSM Sent: " << wsmSent << endl;
        /*
        LinearRegression mlr({
            {13828.2,20908.8, 10.94},
            {13851,20946.5, 14.77 },
            {13926.5,20925.2, 14.09 },
            {13982.3,20894.2, 6.92},
            {14039.6,20865, 14.49}
        },
            {1030, 1025, 1020, 1015, 1010},

            NODEBUG);
        mlr.fit();
        std::cout << mlr.predict({ 14500, 21700, 12.50 }) << endl ;
        std::cout << "----------------------------------" << endl;


        for (auto const& x : registry.rsuRegistry)
        {
            std::cout << x.first << ':' << x.second << std::endl ;
        }*/



    }




    // statistics recording goes here
}


