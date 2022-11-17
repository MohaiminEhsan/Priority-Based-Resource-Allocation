#!/bin/bash

#   Description 
#   This script will automatically run the veins code using omnetpp N number of times with different defined simulation time

#   Version: 1.0.0
#   Date: 12/08/2022

#   Options:
#       -n              Number of times the simulation will run
#       -h              User Description
#       -times          n number of simulation times (Comma separated)
#       -p              Run pythons scipt at the end of simulation to provide the analysis data for graph

#   Requirement:
#           The Autobuild.sh has to be in side Veins Project Folder
#           The omnetpp path has to provided in the "OmnetppPath" variable


#   Version:
#       1.0.0       Intial Version Creation
#
#   <--BEGIN

#
#   Variables
#

######################################################################
# Will Change Based on User
OmnetppPath="/home/mohaimin/Downloads/veins/omnetpp/bin"
#######################################################################


VeinsProjectPath="$(pwd)"
VeinsExamplePath="$VeinsProjectPath/examples/veins"
FCFCResultAnalysisFilePath="$VeinsExamplePath/ResultAnalysisFCFS.txt"
OutputFilePath="$VeinsExamplePath/ResutlAnalysis"
OutputFileforDataCollectionPattern="OutputForRun"
NumberofArguments="$#"
SimulationNumerFlag="-n"
SimulationTimeFlag="-times"
HelpFlag="-h"
PythonScriptFlag="-p"
NumberofSimulations=""
SimuationTimes=""
declare -a SimulationTimesArray
ArrayLength=""
BooleanSimulationNumber=false
BooleanSimulationTimes=false
BooleanPython=false
SimTimeLimit=""
SimTimeLimitPattern="sim-time-limit"
OmnetIniFilePath="$VeinsExamplePath/omnetpp.ini"
SimTimeLimitOld=""
SimTimeLimitNew=""
SimTimeLimitReplaceCmd=""
SimulationOrder=0
PythonFilePath="$VeinsExamplePath/FCFS.py"




#   Print Help
function help(){
    echo ""
    echo "********************************************************************************************************"
    echo "Options:"
    echo "          -n      Number of times the simulation will run"
    echo "          -h      User Description"
    echo "          -times  n number of simulation times (Comma separated) <Must be in seconds>"
    echo "          -p      Run pythons scipt at the end of simulation to provide the analysis data for graph"
    echo "Usage:"
    echo "          ./AutoRun.sh -n <Value> -times <Time1,Time2,Time3....> -p"
    echo ""
    echo "********************************************************************************************************"
}



function Initialize(){
    export PATH=$OmnetppPath:$PATH
    cd $VeinsProjectPath
    ./configure
    if [ $? -ne 0 ]; then
        echo "ERROR FOUND!!!!"
        exit
    fi
    cd $VeinsProjectPath
    make
    if [ $? -ne 0 ]; then
        echo "ERROR FOUND!!!!"
        exit
    fi
}



# Run OmnetIni Veins Code
function RunVeinsCode(){

    rm -rf $FCFCResultAnalysisFilePath
	
    SimulationOrderNumber=$1
    
    echo "####################### Simulation Run Number: $SimulationOrderNumber #######################"
    cd $VeinsExamplePath
    #RunCmd="./run -u Cmdenv -f omnetpp.ini -f veins"

    OutputFile="$OutputFilePath/"$OutputFileforDataCollectionPattern"_"$SimulationOrderNumber".txt"

    #RunCmd="opp_run -r 0 -m -u Cmdenv -c WithChannelSwitching -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini > $OutputFile"
    #echo $RunCmd

      
    
    if [ ! -d "$OutputFilePath" ]; then
        mkdir $OutputFilePath
    fi

    if test -f "$OutputFile"; then
        rm -rf $OutputFile
        touch $OutputFile
    else
        touch $OutputFile
    fi
    opp_run -r 0 -m -u Cmdenv -c WithChannelSwitching -n .:../../src/veins --image-path=../../images -l ../../src/veins omnetpp.ini > $OutputFile
    #$RunCmd
    if [ $? -ne 0 ]; then
        echo "ERROR FOUND!!!!"
        exit
    fi
    #Clean Up the output text file (Depends on user)
    sed -i '/undisposed object:/d' $OutputFile
	    
    if [ "$BooleanPython" == true ];then
    	RunPythonCode $SimulationOrderNumber
    fi
}



function RunPythonCode(){
	echo "$PythonFilePath"
	if [ -f "$PythonFilePath" ]; then
		echo "Python Script will run now"
        	python3 $PythonFilePath $1
        	echo "Python Script run end now"
    	fi
    	
}



#
#   Main
#

echo ""
echo "***********************************[AutoRun.sh:Begin]******************************************"


Initialize


echo "Total Arguments Provided: $NumberofArguments"

#   1 Argument - (-h)
if [ $NumberofArguments -eq 1 ]; then
    Argument=$1
    if [ "$Argument" == "-h" ]; then
        help
        exit
    fi
fi


#   2 Arguments - (-n)
if [ $NumberofArguments -eq 2 ]; then
    Argument=$1
    if [ "$Argument" == "$SimulationNumerFlag" ]; then
        NumberofSimulations=$2
        echo "Number of Simulations Runs: $NumberofSimulations"
        BooleanSimulationNumber=true
    else
        help
        exit
    fi
fi

# 3 Arguments - (-n + -p)
if [ $NumberofArguments -eq 3 ]; then
    Argument=$1
    if [ "$Argument" == "$SimulationNumerFlag" ]; then
        NumberofSimulations=$2
        echo "Number of Simulations Runs: $NumberofSimulations"
        BooleanSimulationNumber=true
    else
        help
        exit
    fi

    Argument=$3
    if [ "$Argument" == "$PythonScriptFlag" ]; then
        echo "Python Script will run with this flag"
        BooleanPython=true
    else
        help
        exit
    fi
fi


# 4 Arguments - (-n + -times)
if [ $NumberofArguments -eq 4 ]; then
    Argument=$1
    if [ "$Argument" == "$SimulationNumerFlag" ]; then
        NumberofSimulations=$2
        echo "Number of Simulations Runs: $NumberofSimulations"
        BooleanSimulationNumber=true
    else
        help
        exit
    fi

    Argument=$3
    if [ "$Argument" == "$SimulationTimeFlag" ]; then
        SimuationTimes=$4
        echo "Simulation Times Provided: $SimuationTimes"
        IFS=',' read -ra SimulationTimesArray <<< "$SimuationTimes"
        ArrayLength=${#SimulationTimesArray[@]}
        if [ $ArrayLength != $NumberofSimulations ]; then
            echo "########### Number of Simulation Times does not match the number of times provided########"
            echo "Numer of Simulation Time: $NumberofSimulations"
            echo "Numer of Times provided: $ArrayLength"
            exit
        fi
        BooleanSimulationTimes=true
    else
        help
        exit
    fi
fi


# 5 Arguments - (-n + -times + -p)
if [ $NumberofArguments -eq 5 ]; then
    Argument=$1
    if [ "$Argument" == "$SimulationNumerFlag" ]; then
        NumberofSimulations=$2
        echo "Number of Simulations Runs: $NumberofSimulations"
        BooleanSimulationNumber=true
    else
        help
        exit
    fi

    Argument=$3
    if [ "$Argument" == "$SimulationTimeFlag" ]; then
        SimuationTimes=$4
        echo "Simulation Times Provided: $SimuationTimes"
        IFS=',' read -ra SimulationTimesArray <<< "$SimuationTimes"
        ArrayLength=${#SimulationTimesArray[@]}
        if [ $ArrayLength != $NumberofSimulations ]; then
            echo "########### Number of Simulation Times does not match the number of times provided########"
            echo "Numer of Simulation Time: $NumberofSimulations"
            echo "Numer of Times provided: $ArrayLength"
            exit
        fi
        BooleanSimulationTimes=true
    else
        help
        exit
    fi

    Argument=$5
    if [ "$Argument" == "$PythonScriptFlag" ]; then
        echo "Python Script will run with this flag"
        BooleanPython=true
    else
        help
        exit
    fi
fi


if [ "$BooleanSimulationNumber" == true ] && [ "$BooleanSimulationTimes" == true ]; then
    for i in "${SimulationTimesArray[@]}"
    do
        SimulationOrder=$((SimulationOrder+1))
        SimTimeLimit="$i"
        GetSimTimeLimitOldCmd="grep -w '$SimTimeLimitPattern' '$OmnetIniFilePath'"
        SimTimeLimitOld=$(eval "$GetSimTimeLimitOldCmd")
        echo "Old Sim Time is $SimTimeLimitOld"
        SimTimeLimitNew="$SimTimeLimitPattern = "$SimTimeLimit"s"
        echo "Will Replace with $SimTimeLimitNew"
        SimTimeLimitReplaceCmd="sed -i 's/$SimTimeLimitOld/$SimTimeLimitNew/g' $OmnetIniFilePath"
        eval "$SimTimeLimitReplaceCmd"

        RunVeinsCode $SimulationOrder

    done
fi
if [ "$BooleanSimulationNumber" == true ] && [ "$BooleanSimulationTimes" == false ]; then
    for i in {1..$(seq $NumberofSimulations)}; do
        SimulationOrder=$((SimulationOrder+1))
        RunVeinsCode $SimulationOrder
    done
fi




