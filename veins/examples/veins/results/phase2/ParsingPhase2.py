import sys
import random
import os
import os.path
import numpy as np
import math
import pandas
from pandas import read_csv
import csv
import scipy.stats as st
import statsmodels.stats.api as sms



Times = [1000,1500,2000,2500,3000]
#Times = [200]
NumberofRuns = 30

NumberofRunsInt = int(NumberofRuns)


if(os.path.isfile("/home/mohaimin/Desktop/ResultShort/phase2/wsmData.csv")):
    os.remove("/home/mohaimin/Desktop/ResultShort/phase2/wsmData.csv")



for time in Times:
    WorkingDir = os.getcwd()
    ResultDir = WorkingDir +"/"+ str(time)

    WSMTotalSentList = []
    WSMRcvdList = []
    WSMActualList = []

    for Run in range(1, NumberofRunsInt+1):
        print("in Run: " + str(Run))
        OutputFileName = ResultDir + "/OutputForRun_" + str(Run) + ".txt"

        WSMRcvd = 0
        WSMActual = 0
        WSMTotalSent = 0
        with open(OutputFileName, 'r') as fp:
            lines = fp.readlines()
            for line in lines:
                print ("In line: " + line)
                if "WSM Received:" in line:
                    SplitedLine = line.split(":")
                    WSMRcvdString = SplitedLine[1]
                    WSMRcvdString = WSMRcvdString.strip()
                    WSMRcvd = int(WSMRcvdString)
                if "WSM Sent:" in line:
                    SplitedLine = line.split(":")
                    WSMTotalSntString = SplitedLine[1]
                    WSMTotalSntString = WSMTotalSntString.strip()
                    WSMActual = int(WSMTotalSntString)
                
                if WSMRcvd!=0 and WSMActual!=0:
                    WSMTotalSent = WSMRcvd + WSMActual
        
        WSMTotalSentList.append(WSMTotalSent)
        WSMRcvdList.append(WSMRcvd)
        WSMActualList.append(WSMActual)

        # CI Calculation for WSMTotalSentList
        lb, ub = sms.DescrStatsW(WSMTotalSentList).tconfint_mean()
        WSMTotalSentList_ci = np.mean(WSMTotalSentList)-lb

        # CI Calculation for WSMRcvdList
        lb, ub = sms.DescrStatsW(WSMRcvdList).tconfint_mean()
        WSMRcvdList_ci = np.mean(WSMRcvdList)-lb

        # CI Calculation for WSMActualList
        lb, ub = sms.DescrStatsW(WSMActualList).tconfint_mean()
        WSMActualList_ci = np.mean(WSMActualList)-lb

    
    #Write All Data in CSV file
    with open('/home/mohaimin/Desktop/ResultShort/phase2/wsmData.csv', 'a', encoding='UTF8', newline='') as f:
        writer = csv.writer(f)
        data = [time, np.mean(WSMTotalSentList),np.mean(WSMActualList),np.mean(WSMRcvdList), WSMTotalSentList_ci, WSMActualList_ci, WSMRcvdList_ci]
        writer.writerow(data)
        



        


        

        