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



NumerofVehicles = sys.argv[1]
NumberofRuns = sys.argv[2]

NumberofRunsInt = int(NumberofRuns)

WorkingDir = os.getcwd()
ResultDir = WorkingDir + "/results/phase2/" + NumerofVehicles

rl_2_list = []
rl_5_list = []
rl_7_list = []

fz_2_list = []
fz_5_list = []
fz_7_list = []

ahp_2_list = []
ahp_5_list = []
ahp_7_list = []

for Run in range(1, NumberofRunsInt+1):
    
    ahp_Status_2 = 0
    ahp_Status_5 = 0
    ahp_Status_7 = 0
    rl_Status_2 = 0
    rl_Status_5 = 0
    rl_Status_7 = 0
    fz_Status_2 = 0
    fz_Status_5 = 0
    fz_Status_7 = 0

    ahp_file = ResultDir + "/" + "applications_ahp_" + NumerofVehicles + "_" + str(Run) +".csv"
    rl_file = ResultDir + "/" + "applications_rl_" + NumerofVehicles + "_" + str(Run) + ".csv"
    fz_file = ResultDir + "/" + "applications_fz_" + NumerofVehicles + "_" + str(Run) + ".csv"
    with open(ahp_file, "r") as file_ahp:
        ahp = csv.reader(file_ahp)
        for row in ahp:
            #print(type(row[11]))
            if int(row[11])==2:
                ahp_Status_2 = ahp_Status_2 + 1
            elif int(row[11])==5:
                ahp_Status_5 = ahp_Status_5 + 1
            elif int(row[11])==7:
                ahp_Status_7 = ahp_Status_7 + 1
    with open(rl_file, "r") as file_rl:
        rl = csv.reader(file_rl)
        for row in rl:
            if int(row[11])==2:
                rl_Status_2 = rl_Status_2 + 1
            elif int(row[11])==5:
                rl_Status_5 = rl_Status_5 + 1
            elif int(row[11])==7:
                rl_Status_7 = rl_Status_7 + 1
    with open(fz_file, "r") as file_fz:
        fz = csv.reader(file_fz)
        for row in fz:
            if int(row[12])==2:
                fz_Status_2 = fz_Status_2 + 1
            elif int(row[12])==5:
                fz_Status_5 = fz_Status_5 + 1
            elif int(row[12])==7:
                fz_Status_7 = fz_Status_7 + 1

    rl_2_list.append(rl_Status_2)
    rl_5_list.append(rl_Status_5)
    rl_7_list.append(rl_Status_7)

    ahp_2_list.append(ahp_Status_2)
    ahp_5_list.append(ahp_Status_5)
    ahp_7_list.append(ahp_Status_7)

    fz_2_list.append(fz_Status_2)
    fz_5_list.append(fz_Status_5)
    fz_7_list.append(fz_Status_7)



AHPServedRatio = []

for i in range(0, NumberofRunsInt):
    TotalReq = ahp_5_list[i] + ahp_7_list[i] + ahp_2_list[i]
    if TotalReq!=0:
        ServeRatio = (ahp_7_list[i]/TotalReq)*100
    else: 
        ServeRatio = 0
    AHPServedRatio.append(ServeRatio)



RLServedRatio = []

for i in range(0, NumberofRunsInt):
    TotalReq = rl_5_list[i] + rl_7_list[i] + rl_2_list[i]
    if TotalReq!=0:
        ServeRatio = (rl_7_list[i]/TotalReq)*100
    else: 
        ServeRatio = 0
    RLServedRatio.append(ServeRatio)



FZServedRatio = []

for i in range(0, NumberofRunsInt):
    TotalReq = fz_5_list[i] + fz_7_list[i] + fz_2_list[i]
    if TotalReq!=0:
        ServeRatio = (fz_7_list[i]/TotalReq)*100
    else: 
        ServeRatio = 0
    FZServedRatio.append(ServeRatio)




# CI Calculation for ahp_2   
lb, ub = sms.DescrStatsW(ahp_2_list).tconfint_mean()
ahp_2_ci = np.mean(ahp_2_list)-lb



# CI Calculation for ahp_5   
lb, ub = sms.DescrStatsW(ahp_5_list).tconfint_mean()
ahp_5_ci = np.mean(ahp_5_list)-lb


# CI Calculation for ahp_7   
lb, ub = sms.DescrStatsW(ahp_7_list).tconfint_mean()
ahp_7_ci = np.mean(ahp_7_list)-lb



# CI Calculation for rl_2   
lb, ub = sms.DescrStatsW(rl_2_list).tconfint_mean()
rl_2_ci = np.mean(rl_2_list)-lb


# CI Calculation for rl_5   
lb, ub = sms.DescrStatsW(rl_5_list).tconfint_mean()
rl_5_ci = np.mean(rl_5_list)-lb


# CI Calculation for rl_7
lb, ub = sms.DescrStatsW(rl_7_list).tconfint_mean()
rl_7_ci = np.mean(rl_7_list)-lb


# CI Calculation for fz_2
lb, ub = sms.DescrStatsW(fz_2_list).tconfint_mean()
fz_2_ci = np.mean(fz_2_list)-lb


# CI Calculation for fz_5
lb, ub = sms.DescrStatsW(fz_5_list).tconfint_mean()
fz_5_ci = np.mean(fz_5_list)-lb


# CI Calculation for fz_7
lb, ub = sms.DescrStatsW(fz_7_list).tconfint_mean()
fz_7_ci = np.mean(fz_7_list)-lb


# CI Calculation for ahp_ratio
lb, ub = sms.DescrStatsW(AHPServedRatio).tconfint_mean()
ahp_ratio_ci = np.mean(AHPServedRatio)-lb

# CI Calculation for rl_ratio
lb, ub = sms.DescrStatsW(RLServedRatio).tconfint_mean()
rl_ratio_ci = np.mean(RLServedRatio)-lb

# CI Calculation for fz_ratio
lb, ub = sms.DescrStatsW(FZServedRatio).tconfint_mean()
fz_ratio_ci = np.mean(FZServedRatio)-lb



#Write All Data in CSV file
with open('/home/mohaimin/Desktop/ResultShort/phase2/allData.csv', 'a', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    data = [np.mean(ahp_2_list), np.mean(ahp_5_list), np.mean(ahp_7_list), np.mean(rl_2_list), np.mean(rl_5_list), np.mean(rl_7_list), np.mean(fz_2_list), np.mean(fz_5_list), np.mean(fz_7_list), np.mean(AHPServedRatio), np.mean(RLServedRatio), np.mean(FZServedRatio), ahp_2_ci, ahp_5_ci, ahp_7_ci, rl_2_ci, rl_5_ci, rl_7_ci, fz_2_ci, fz_5_ci, fz_7_ci, ahp_ratio_ci, rl_ratio_ci, fz_ratio_ci]

    writer.writerow(data)



#Write status 2 data in CSV file
with open('/home/mohaimin/Desktop/ResultShort/phase2/Status2Data.csv', 'a', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    data = [np.mean(rl_2_list), np.mean(ahp_2_list), np.mean(fz_2_list), rl_2_ci, ahp_2_ci, fz_2_ci,]

    writer.writerow(data)


#Write status 5 data in CSV file
with open('/home/mohaimin/Desktop/ResultShort/phase2/Status5Data.csv', 'a', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    data = [np.mean(rl_5_list), np.mean(ahp_5_list), np.mean(fz_5_list), rl_5_ci, ahp_5_ci, fz_5_ci,]

    writer.writerow(data)


#Write status 7 data in CSV file
with open('/home/mohaimin/Desktop/ResultShort/phase2/Status7Data.csv', 'a', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    data = [np.mean(rl_7_list), np.mean(ahp_7_list), np.mean(fz_7_list), rl_7_ci, ahp_7_ci, fz_7_ci,]

    writer.writerow(data)



#Write status ratio data in CSV file
with open('/home/mohaimin/Desktop/ResultShort/phase2/ReqRatio.csv', 'a', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    data = [np.mean(AHPServedRatio), np.mean(RLServedRatio), np.mean(FZServedRatio), ahp_ratio_ci, rl_ratio_ci, fz_ratio_ci,]

    writer.writerow(data)
