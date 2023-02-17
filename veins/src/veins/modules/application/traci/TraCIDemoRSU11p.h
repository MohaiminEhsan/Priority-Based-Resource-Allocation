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

#pragma once

#include <omnetpp.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <sstream>
#include <typeinfo>
#include <fstream>
#include <utility>
#include <queue>


using namespace omnetpp;
using namespace std;

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"


namespace veins {

/**
 * Small RSU Demo using 11p
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {

public:

    //void initialize(int stage);

    void initialize(int stage) override;
    void finish() override;

    //int stage=0;

    double request_interval_size;
    double request_tolerance_size;
    double rsuXCoord;
    double rsuYCoord;
    queue<tuple<int, int, float> > QLow;
    queue<tuple<int, int, float> > QHigh;



    // Creating the events
    cMessage* request_event;
    //cMessage* request_tolerance_event;
    cMessage* broadcast_event;
    cMessage* vehicle_event;
    cMessage* dwellTime_event;
    cMessage* traffic_flow_event;
    cMessage* rl_event;
    cMessage* app_status;
    cMessage* fuzzy_event;
    cMessage* fuzzy_request_event;
    cMessage* queue_handling_event;
    cMessage* fuzzy_allocation_event;
    cMessage* rl_fz_event;


    fstream predictLog;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onBSM(DemoSafetyMessage* bsm) override;

    void handleSelfMsg(cMessage* msg) override;
    //void handlePositionUpdate(cObject* obj) override;

    int numMsg;
};

} // namespace veins
