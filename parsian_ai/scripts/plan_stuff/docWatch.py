import time
import os

import re
import json

import random


from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler


class Watcher:
    DIRECTORY_TO_WATCH = ""

    def __init__(self):
        self.__p = os.getcwd().rsplit('/', 1)[0]
        self.__p = self.__p.rsplit('/', 1)[0]
        self.__p = self.__p + "/plans/"

        self.__observer = Observer()
        # DIRECTORY_TO_WATCH = DIRECTORY_TO_WATCH + self.__p

    def run(self):
        self.__event_handler = Handler(self.__p)
        self.__observer.schedule(self.__event_handler, self.__p, recursive=True)
        self.__observer.start()
        try:
            while True:
                time.sleep(5)
        except:
            self.__observer.stop()
            print("Error")
        self.__observer.join()

    def get_all_plans(self):
        return self.__event_handler.get_all_plans()


    def choose_plan(self):
        return self.__event_handler.choose_plan()

class Handler(FileSystemEventHandler):

    def __init__(self, p):
        self.__final_list = []
        self.__ignore = []
        self.__shuffleCount = 0

        global final_list, shuffleCount

        # read_plan(f[5])
        self.__final_list = self.list_valid_plans(p)

        self.plansToDict()

        self.shuffle_indexing(self.__final_list)  # call once

    # @staticmethod
    def on_any_event(self, event):
        if event.is_directory:
            return None

        # elif event.event_type == 'created':
            # print "Received created event - %s." % event.src_path

        elif event.event_type == 'modified':
            # print "Received modified event - %s" % event.src_path
            self.add_plan(event.src_path)

        elif event.event_type == 'deleted':
            # print "Received deleted event - %s" % event.src_path
            self.remove_plan(event.src_path)




    def list_valid_plans(self, path):
        file_list = []
        for path, dirs, files in os.walk(path):
            for filename in files:
                fname = os.path.join(path, filename)
                #         print fname.rsplit('/', 1)[1]   # just file name
                file_list.append(fname)

        for f in file_list:  # find ignore list
            if f != None:
                if f.endswith('.ignore'):
                    ignore_lst = open(str(f)).read().split("\n")
                    file_list.remove(f)
                    break

        if '' in ignore_lst:
            ignore_lst.remove('')

        no_cmnt_ignr_lst = [li for li in ignore_lst if not li.startswith('#')]
        print (no_cmnt_ignr_lst)
        self.__ignore = no_cmnt_ignr_lst

        bad_files = []
        for f in file_list:
            if f != None:
                if f.endswith('.json'):
                    # if os.stat(str(f)).st_size != 0:
                        # print "adding " + str(f).rsplit('/', 1)[1]
                        # with open(str(f)) as json_data:
                        #     plan_data.append(json.load(json_data))
                    if os.stat(str(f)).st_size == 0:
                        bad_files.append(f)
                        print("empty: " + str(f) + " --> removed!")
                else:
                    bad_files.append(f)
                    print ("not json: " + str(f) + " --> removed!")

        file_list2 = [f for f in file_list if not f in bad_files]

        # print file_list2

        self.__final_list = self.ignore_plans(file_list2, ignore_lst)
        return self.__final_list


    def add_plan(self, path_to_plan):

        flag = 0
        if str(path_to_plan).endswith(".json"):
            if not (str(path_to_plan).split("/plans")[1]) in self.__ignore:
                for pattern in self.__ignore:
                    if pattern != '':
                        try:
                            if (re.search(pattern, str(path_to_plan).split("/plans")[1]) and
                                    re.search(pattern, str(path_to_plan).split("/plans")[1]).start() == 0):
                                flag = 1
                        except:
                            print ("Invalid Expression: " + pattern)
            else:
                flag = 1

        if flag == 0:
            self.__final_list.append(path_to_plan)
            print (str(path_to_plan).split("/plans")[1]+" added.")

        return self.__final_list


    def remove_plan(self, path_to_plan):
        if path_to_plan in self.__final_list:
            print (str(path_to_plan).split("/plans")[1]+" removed.")
            self.__final_list.remove(path_to_plan)


    def ignore_plans(self, file_list, ignore_list):
        # files:
        new_file_list = [fi for fi in file_list if (str(fi).split("/plans")[1] not in ignore_list)]
        new_file_list2 = [fi for fi in file_list if fi not in new_file_list]
        for pattern in ignore_list:
            if pattern != '':
                try:
                    for f in new_file_list:
                        if re.search(pattern, str(f).split("/plans")[1]) and re.search(pattern, str(f).split("/plans")[1]).start() == 0:
                            new_file_list2.append(f)
                except:
                    print("Invalid Expression: "+pattern)

        print "ignored plans:"
        for fil in new_file_list2:
            print "\t"+str(fil).split("/plans")[1]

        last = [term for term in new_file_list if not term in new_file_list2]

        print "FINAL LIST:"
        for i in last:
            print "\t"+str(i).split("/plans")[1]

        return last


    def shuffle_indexing(self, alist):
        random.shuffle(alist)


    def read_plan(self, plan_path):
        print "opening plan "+str(plan_path).split("/plans")[1]
        with open(str(plan_path)) as json_data:
            a = (json.load(json_data))

            print a["apiVersion"]
            print a["id"]

            plans_cnt = len(a["plans"])
            for p_i in range(0,plans_cnt):
                print "-------------------------------plan %d:"%p_i
                plan_i = a["plans"][p_i]
                agents_cnt = len(plan_i["agents"])
                for p_a_i in range(0, agents_cnt):
                    plan_i_ag = plan_i["agents"][p_a_i]
                    print "agent ID: "
                    print plan_i_ag["ID"]
                    plan_i_ag_i_pos = plan_i_ag["positions"]
                    agents_pos_cnt = len(plan_i_ag["positions"])
                    for p_a_p_i in range(0, agents_pos_cnt):
                        print plan_i_ag_i_pos[p_a_p_i]["angel"]
                        print plan_i_ag_i_pos[p_a_p_i]["pos-x"]
                        print plan_i_ag_i_pos[p_a_p_i]["pos-y"]
                        print plan_i_ag_i_pos[p_a_p_i]["tolerance"]
                        plan_i_ag_i_pos_i_s = plan_i_ag_i_pos[p_a_p_i]["skills"]
                        agents_pos_skill_cnt = len(plan_i_ag_i_pos[p_a_p_i]["skills"])
                        for p_a_p_S_i in range(0, agents_pos_skill_cnt):
                            print plan_i_ag_i_pos_i_s[p_a_p_S_i]["flag"]
                            print plan_i_ag_i_pos_i_s[p_a_p_S_i]["name"]
                            print plan_i_ag_i_pos_i_s[p_a_p_S_i]["primary"]
                            print plan_i_ag_i_pos_i_s[p_a_p_S_i]["secondary"]
                            if "target" in plan_i_ag_i_pos_i_s[p_a_p_S_i]:
                                print plan_i_ag_i_pos_i_s[p_a_p_S_i]["target"]["agent"]
                                print plan_i_ag_i_pos_i_s[p_a_p_S_i]["target"]["index"]
                            else:
                                print "no target for this skill"
                            print ""
                print plan_i["chance"]
                print plan_i["lastDist"]
                print plan_i["planMode"]
                print plan_i["tags"]
                print plan_i["ballInitPos"]["x"]
                print plan_i["ballInitPos"]["y"]

    def plansToDict(self):
        dictlist = []
        for plan in self.__final_list:
            planDict = open(str(plan)).read()
            dictlist.append(planDict)
            # print "nanananananananannanananananna"
            # print (str(plan))
            # print(planDict.keys)
            return dictlist

    def choose_plan(self):
        if len(self.__final_list) > self.__shuffleCount:
            self.__shuffleCount += 1
            return self.__final_list[self.__shuffleCount-1]
        else:
            self.__shuffleCount = 1
            return self.__final_list[self.__shuffleCount - 1]

    def get_all_plans(self):
        return self.__final_list


if __name__ == '__main__':
    w = Watcher()
    w.run()