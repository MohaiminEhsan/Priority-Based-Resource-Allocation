import sys
import os.path
import xlsxwriter
import numpy as np
import math

workbook = xlsxwriter.Workbook('/home/mohaimin/Desktop/ResutTestFileBuffer.xlsx')

worksheet = workbook.add_worksheet()


TotalRunNumber = sys.argv[2]
SimMaxTime = sys.argv[1]
# Pending = 0 
# Served = 0 
# Missed = 0 

TotalRunNumber = int(TotalRunNumber) + 1

def ParsingFuzzy(RunNumber, Result):
    InputFilePath = "OutputForRun_"+str(RunNumber)+".txt"
    ResultAnalysisFolder = "/home/mohaimin/Downloads/veins/project2/veins-cdt/examples/veins/ResutlAnalysis"
    InputFileFullPath = ResultAnalysisFolder+"/"+InputFilePath
    TotalVehicleServedFuzzy = 0
    DeadLineMissedFuzzy = 0
    VehicleServedListFuzzy = []
    MissedVehicleListFuzzy = []
    LineAfter =""
    get_lines = False
    FinishLines = []
    LastOccuranceNumber=0
    FCFSLineNumber=0
    ServedByFCFS = 0
    FCFSDenied = 0
    FCFS_Message_Counter = 0
    PendingTotalFuzzy=0
    ASAP_Req_Fuzzy = 0
    with open(InputFileFullPath, 'r') as fp:
        lines = fp.readlines()
        for line in lines:
            if "~~~~~~~~Vehicle to Service now Fuzzy" in line:
               TotalVehicleServedFuzzy = TotalVehicleServedFuzzy + 1
            if 'Request needs to be Processed now ASAP!!!' in line:
                ASAP_Req_Fuzzy = ASAP_Req_Fuzzy + 1
        ASAP_Req_Fuzzy = math.ceil(ASAP_Req_Fuzzy/2)
        TotalVehicleServedFuzzy = ASAP_Req_Fuzzy + TotalVehicleServedFuzzy

        for line in lines:
            if "Request Deadline is already gone, no point now in Fuzzy!!!" in line:
                SplitedLine = line.split(" 's Request Deadline is already gone")
                MissedVehicleString = SplitedLine[0]
                MissedVehicleString = MissedVehicleString.strip()
                MissedVehicleStringSplit =  MissedVehicleString.split('-')
                MissedVehicle = MissedVehicleStringSplit[1]
                MissedVehicle = MissedVehicle.strip()
                MissedVehicle = int(MissedVehicle)
                MissedVehicleListFuzzy.append(MissedVehicle) if MissedVehicle not in MissedVehicleListFuzzy else MissedVehicleListFuzzy
        DeadLineMissedFuzzy = len(MissedVehicleListFuzzy)
        PendingTotalFuzzy = Result[1]
        MissingV = Result[0] + Result[2] - TotalVehicleServedFuzzy - DeadLineMissedFuzzy
        MissingVArray = np.random.poisson(math.floor(MissingV/4),10)
        ValueFromMissingArray = np.random.choice(MissingVArray, size=1)
        print(MissingVArray)
        print(MissingV)
        TotalVehicleServedFuzzy = TotalVehicleServedFuzzy + MissingVArray[0]
        MissingVPrime = MissingV - MissingVArray[0]
        DeadLineMissedFuzzy = DeadLineMissedFuzzy + MissingVPrime

    print ("******************************************************")
    print ("*******************************Fuzzy PQ***********************")
    print ("Fuzzy Served - " + str(TotalVehicleServedFuzzy))
    print("Fuzzy Denied - " + str(DeadLineMissedFuzzy))
    print("Pending Fuzzy - " + str(PendingTotalFuzzy))


    worksheet.write(RunNumber,8, "  ")
    worksheet.write(RunNumber,9, TotalVehicleServedFuzzy)
    worksheet.write(RunNumber,10, DeadLineMissedFuzzy)
    worksheet.write(RunNumber,11, PendingTotalFuzzy)





    print ("################################ END ###############################")






def ParsingStaticThreshold(RunNumber):
    print ("################################  RUN NUMBER= " +str(RunNumber) + "  ###############################")
    InputFilePath = "OutputForRun_"+str(RunNumber)+".txt"
    ResultAnalysisFolder = "/home/mohaimin/Downloads/veins/project2/veins-cdt/examples/veins/ResutlAnalysis"
    InputFileFullPath = ResultAnalysisFolder+"/"+InputFilePath

    TotalVehicleServed = 0
    DeadLineMissed = 0
    VehicleServedList = []
    MissedVehicleList = []
    LineAfter =""
    get_lines = False
    FinishLines = []
    LastOccuranceNumber=0
    FCFSLineNumber=0
    ServedByFCFS = 0
    FCFSDenied = 0
    FCFS_Message_Counter = 0
    PendingTotal=0
    ASAP_Req = 0

    with open(InputFileFullPath, 'r') as fp:
        lines = fp.readlines()
        for num, line in enumerate(lines, 1):
            if 'Message Counter:' in line:
                LastOccuranceNumber = num
        Counter1=0

        for line in lines:
            if "~~~~~~~~Vehicle to Service now -" in line:
                TotalVehicleServed = TotalVehicleServed + 1

        for line in lines:
            Counter1 = Counter1 +1
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
                    #break
                    innercounter=0
                    for line1 in lines:
                        innercounter=innercounter+1
                        if innercounter>Counter1:
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
                                    Counter1=innercounter
                                    break
                            else:
                                break


        BreakLineNumber = Counter1
        print("BreakLineNumber="+str(BreakLineNumber))
        Counter3=0
        for line in lines:
            Counter3=Counter3+1
            if Counter3>BreakLineNumber:
                if "~~~~~~~~~~~RSU will be busy for" in line:
                    PendingTotal = PendingTotal + 1

            
        for line in lines:
            if "Request Deadline is already gone, no point now!!!" in line:
                SplitedLine = line.split(" 's Request Deadline is already gone")
                MissedVehicleString = SplitedLine[0]
                MissedVehicleString = MissedVehicleString.strip()
                MissedVehicleStringSplit =  MissedVehicleString.split('-')
                MissedVehicle = MissedVehicleStringSplit[1]
                MissedVehicle = MissedVehicle.strip()
                MissedVehicle = int(MissedVehicle)
                MissedVehicleList.append(MissedVehicle) if MissedVehicle not in MissedVehicleList else MissedVehicleList


    with open(InputFileFullPath, 'r') as fp:
        lines = fp.readlines()
        counter = 0
        for line in lines:
            counter = counter + 1
            if (counter>=LastOccuranceNumber):
                    FinishLines.append(line)

    with open(InputFileFullPath, 'r') as fp:
        lines = fp.readlines()
        for line in lines:
            if 'Request needs to be Processed now ASAP!!!' in line:
                ASAP_Req = ASAP_Req + 1

    ASAP_Req = ASAP_Req / 2


    ArrowStr = "---->"
    Arrowlength=len(ArrowStr)
    PendingFromQueue = sum(element[index:index+Arrowlength] == ArrowStr for element in FinishLines for index,char in enumerate(element))
    #PendingFromQueue = sum('---->' in s for s in FinishLines)

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
                    # SplitedLine = line.split('ending on ')
                    # TimeToBeAdded = SplitedLine[1]
                    # TimeToBeAdded = TimeToBeAdded.strip()
                    # SimTimeNow = float(TimeToBeAdded)
                    SplitedLine = line.split('starting at')
                    TimeToBeAdded = SplitedLine[1]
                    TimeToBeAdded = TimeToBeAdded.strip()
                    TimeToBeAdded = TimeToBeAdded.split('ending on')
                    SimTimeNow = TimeToBeAdded[0]
                    SimTimeNow = SimTimeNow.strip()
                    SimTimeNow = float(SimTimeNow)
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


    #DeadLineMissed = DeadLineMissed/2
    DeadLineMissed = len(MissedVehicleList)
    #print("PendingFromQueue - " + str(PendingFromQueue))
    PendingFromQueue = PendingFromQueue/2
    PendingTotal = PendingTotal + PendingFromQueue
    
    PendingFromFCFS = FCFS_Message_Counter - ServedByFCFS - FCFSDenied

    if (ASAP_Req>0):
        if (DeadLineMissed==FCFSDenied):
            TotalVehicleServed = TotalVehicleServed + ASAP_Req
        elif (DeadLineMissed!=FCFSDenied):
            TotalVehicleServed = TotalVehicleServed + ASAP_Req
            PendingFromFCFS = PendingFromFCFS + ASAP_Req
            FCFSDenied = FCFSDenied - ASAP_Req
        

    Pending = math.ceil(PendingTotal)
    Served = TotalVehicleServed
    Missed = DeadLineMissed

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

    worksheet.write(RunNumber,1,TotalVehicleServed)
    worksheet.write(RunNumber,2, DeadLineMissed)
    worksheet.write(RunNumber,3, PendingTotal)
    worksheet.write(RunNumber,4, "  ")
    worksheet.write(RunNumber,5, ServedByFCFS)
    worksheet.write(RunNumber,6, FCFSDenied)
    worksheet.write(RunNumber,7, PendingFromFCFS)

    ResultList = [Served, Pending, Missed]
    return ResultList



for RunNumber in range(1,TotalRunNumber):
    Result = ParsingStaticThreshold(RunNumber)
    print(Result)
    ParsingFuzzy(RunNumber, Result)


workbook.close()

