// -----------------------------------------------------------------
// File: colognev1000.ned
//
// NOTES: it could be necessary to do the following:
// 1. To include this folder (or an upper one) in the list of
//    .ned folders from the project properties:
//        OMNeT++ > NED Source Folder.
// 2. To specify a 'package' line if there exist same package.ned
//    file in the working directory (or an upper one).
// -----------------------------------------------------------------

package colognev1000.simulations.colognev1000;

import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;

network colognev1000 extends Scenario
{
    parameters:
        @display("bgb=$playgroundSizeX,$playgroundSizeY");

        bool withObstacles = default(true);

    submodules:
        rsu[1]: RSU {
            @display("p=150,140;b=10,10,oval");
        }

    connections allowunconnected:
}

