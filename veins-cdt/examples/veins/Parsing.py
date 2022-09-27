import sys
import os.path

RunNumber = sys.argv[2]
SimMaxTime = sys.argv[1]

InputFilePath = "OutputForRun_"+RunNumber+".txt"
ResultAnalysisFolder = "/home/mohaimin/Downloads/veins/project2/veins-cdt/examples/veins/ResutlAnalysis"
InputFileFullPath = ResultAnalysisFolder+"/"+InputFilePath

TotalVehicleServed = 0
DeadLineMissed = 0
VehicleServedList = []
LineAfter =""
get_lines = False
FinishLines = []
LastOccuranceNumber=0
FCFSLineNumber=0
ServedByFCFS = 0
FCFSDenied = 0
FCFS_Message_Counter = 0
PendingTotal=0

with open(InputFileFullPath, 'r') as fp:
    lines = fp.readlines()
    for num, line in enumerate(lines, 1):
        if 'Message Counter:' in line:
            LastOccuranceNumber = num
    Counter1=0
    for line in lines:
        Counter1 = Counter1 +1
        if "~~~~~~~~Vehicle to Service now" in line:
            TotalVehicleServed = TotalVehicleServed + 1
        if "Request Will be put to a queue" in line:
            SplitedLine=line.split("'s")
            VehicleStringSplit=SplitedLine[0].split("-")
            VehicleString=VehicleStringSplit[1]
            VehicleString=VehicleString.strip()
            VehicleString=float(VehicleString)
            VehicleServedList.append(VehicleString) if VehicleString not in VehicleServedList else VehicleServedList
        if "~~~~~~~~~~~RSU will be busy for" in line:
            SplitedLine = line.split("-")
            TimeToBeAdded = SplitedLine[1]
            TimeToBeAdded = TimeToBeAdded.strip()
            SimTimeNow = float(TimeToBeAdded)
            SplitedLine = line.split("for")
            SplitedSplitedLine = SplitedLine[1].split('FROM')
            TimeNextToBeAdded = SplitedSplitedLine[0]
            TimeNextToBeAdded = TimeNextToBeAdded.strip()
            TimeNextToBeAdded = float(TimeNextToBeAdded)
            SimTimeNow = TimeNextToBeAdded + SimTimeNow
            if SimTimeNow>=float(SimMaxTime):
                break
    BreakLineNumber = Counter1
    Counter3=0
    for line in lines:
        Counter3=Counter3+1
        if Counter3>BreakLineNumber:
            if "~~~~~~~~~~~RSU will be busy for" in line:
                PendingTotal = PendingTotal + 1

        
    for line in lines:
        if "Request Deadline is already gone, no point now!!!" in line:
            DeadLineMissed = DeadLineMissed + 1


with open(InputFileFullPath, 'r') as fp:
    lines = fp.readlines()
    counter = 0
    for line in lines:
        counter = counter + 1
        if (counter>=LastOccuranceNumber):
                FinishLines.append(line)








PendingFromQueue = sum('---->' in s for s in FinishLines)

with open(InputFileFullPath,'r') as fp:
    lines = fp.readlines()

    for num, line in enumerate(lines, 1):
        if 'FCFS*************************' in line:
            FCFSLineNumber = num

    counter = 0
    for line in lines:
        counter = counter + 1
        if (counter>FCFSLineNumber):
            if 'ending on ' in line:
                SplitedLine = line.split('ending on ')
                TimeToBeAdded = SplitedLine[1]
                TimeToBeAdded = TimeToBeAdded.strip()
                SimTimeNow = float(TimeToBeAdded)
                if SimTimeNow<=float(SimMaxTime):
                    ServedByFCFS = ServedByFCFS +1
                elif SimTimeNow>float(SimMaxTime):
                    break
    Counter2=0
    for line in lines:
        Counter2=Counter2+1
        if (counter>FCFSLineNumber):
            if 'So can not fulfil this request!!!' in line:
                FCFSDenied = FCFSDenied + 1


    FCFS_Message_Counter = len(lines) - FCFSLineNumber




if ServedByFCFS==0 and TotalVehicleServed!=0:
    ServedByFCFS = ServedByFCFS + 1


DeadLineMissed = DeadLineMissed/2
PendingFromQueue = PendingFromQueue/2
PendingTotal = PendingTotal + PendingFromQueue
PendingFromFCFS = FCFS_Message_Counter - ServedByFCFS - FCFSDenied


print ("*******************************PQ Based***********************")
print("TotalVehicleServed - " + str(TotalVehicleServed))
print("DeadLineMissed - " + str(DeadLineMissed))
print("VehicleServedList - " )
print(VehicleServedList)
print("PendingTotal - " + str(PendingTotal))
print ("******************************************************")
print ("*******************************FCFS***********************")
print ("ServedByFCFS - " + str(ServedByFCFS))
print("FCFSDenied - " + str(FCFSDenied))
print("PendingFromFCFS - " + str(PendingFromFCFS))

print("FCFS_Message_Counter - " + str(FCFS_Message_Counter))


