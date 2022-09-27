import sys
import os.path

RSUTimeStatus = 0
RunNumber = sys.argv[1]


filepath = "ResultAnalysisFCFS.txt"
OutputFilePath = "OutputForRun_"+RunNumber+".txt"
ResultAnalysisFolder = "/home/mohaimin/Downloads/veins/project2/veins-cdt/examples/veins/ResutlAnalysis"
OuptuFileFullPath = ResultAnalysisFolder+"/"+OutputFilePath
RequestList = []



if os.path.exists(OuptuFileFullPath):
	file_object = open(OuptuFileFullPath, 'a')
	file_object.write("****************************FCFS*************************\n")

with open(filepath, 'r') as fp:
    lines = fp.readlines()
for x in lines:
    SplitedLine = x.split(" ")
    VehilceID = SplitedLine[0]
    RequestArrivalTime = SplitedLine[1]
    RequestServiceTime = SplitedLine[2]
    RequestDeadline = SplitedLine[3]

    SingleRequest = [VehilceID, RequestArrivalTime, RequestServiceTime, RequestDeadline]
    RequestList.append(SingleRequest)



RequestedListRefined = [list(t) for t in set(tuple(element) for element in RequestList)]

RequestedListRefined.sort(key=lambda row: (float(row[1])))

#print(RequestedListRefined)


for list in RequestedListRefined:


    VehilceID = float(list[0])
    RequestArrivalTime = float(list[1])
    RequestServiceTime = float(list[2])
    RequestDeadline = float(list[3])
    RequestDeadlineLeft = RequestDeadline - RequestArrivalTime
    if (RequestDeadlineLeft < RequestServiceTime):
        toprint = (str(VehilceID) + " has " + str(RequestDeadline) + " left to finish a " + str(RequestServiceTime) + " time request. So can not fulfil this request!!!")
    else:
        RSUTimeStatus = RSUTimeStatus + RequestServiceTime
        toprint = (str(VehilceID) + "'s request has started. it will take time- " + str(RequestServiceTime) + " ending on "+ str(RSUTimeStatus) )
    file_object.write(toprint+"\n")
    print(toprint)
        
        
if os.path.isfile(filepath):
  #os.remove(filepath)
  print("File has been deleted")
else:
  print("File does not exist")



