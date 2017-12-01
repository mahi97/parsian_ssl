import os
import json

import re

def path_to_dict(path):
    d = {'name': os.path.basename(path)}
    if os.path.isdir(path):
        d['type'] = "directory"
        d['children'] = [path_to_dict(os.path.join(path,x)) for x in os.listdir(path)]
    else:
        d['type'] = "file"
    return d


def find_file(diction, list, mystr):
    if diction['type'] is 'directory':
        if len(diction['children']) != 0:
            for d in diction['children']:
                list.append(find_file(d, list, mystr+"/"+diction['name']))
    else :
        return mystr+"/"+str(diction['name'])


def loadAllPlans(pathToPlanDirectory):
    diction = path_to_dict(pathToPlanDirectory)

    pathToPlanDirectory = pathToPlanDirectory.rsplit('/', 1)[0]

    pathList = []
    find_file(diction, pathList, "")

    data = []   #list of plans

    for p in pathList:
        if p != None:
            if p.endswith('.ignore'):
                ignore = open(pathToPlanDirectory + str(p)).read().split("\n")
                pathList.remove(p)

    ignoreList = re.compile(r"")

    for p in pathList:
        if p != None:
            if p.endswith('.json'):
                with open(pathToPlanDirectory+str(p)) as json_data:
                    data.append(json.load(json_data))

    return data

def loadOnePlan(pathToPlan):
    if pathToPlan != None:
        with open(pathToPlan) as json_data:
            data = json.load(json_data)

    return data


planList = []

if __name__ == "__main__":
    planList = loadAllPlans("/home/fateme/Workspace/parsian_ws/src/parsian_ssl/parsian_ai/plans")
    print "number of plans: %d"%len(planList)
