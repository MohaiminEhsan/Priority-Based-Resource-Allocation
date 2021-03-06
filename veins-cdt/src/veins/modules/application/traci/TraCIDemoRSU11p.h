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

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"//added
#include "veins/base/utils/FindModule.h"//added
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"
#include "veins/modules/application/traci/constants.h"
#include "veins/modules/application/traci/TraCIDemo11p.h"

#include "cstring"
#include <iostream>
#include <list>
#include <omnetpp.h>
#include <cstring>
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
#include <bits/stdc++.h>
#include <list>

using namespace std;

namespace veins {

/**
 * Small RSU Demo using 11p
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {
protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void handleSelfMsg(cMessage* msg) override;
    cMessage* QueueHandlingMessage;
    cMessage* RSUStatusChangeMessage;
    cMessage* DeleteFromOtherRSUNotification;
    //void handleSelfMsg(cMessage* msg) override;
    Mac1609_4* mac;
public:
    int PQ_Size = 30;
    int MaxDeadline = 50;
    double DeadLinePercetageThreshold = .60;
    //extern std::list<int> NodeWithMessage;
    int HighPriorityThreshold = 5;
    queue<tuple<int, double, std::string, double, int> > QHigh;
    queue<tuple<int, double, std::string, double, int> > QMid;
    queue<tuple<int, double, std::string, double, int> > QLow;
    double RSUmaxRange = 500;
    void initialize(int stage) override;
    void finish() override;
    void QueueHandling();
    void ResourceAlllocation();
    bool RunThread = true;
    bool RSUBusy = false;
    double RSUBusyTime = 0;


    //void printList()
};

} // namespace veins
