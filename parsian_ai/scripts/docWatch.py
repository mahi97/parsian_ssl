import time
import os
import signal
import sys
import re
import json
import random
from parsian_msgs.msg import parsian_plan
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import rospkg

class Watcher:
    DIRECTORY_TO_WATCH = ""

    def __init__(self):
        signal.signal(signal.SIGINT, self.signal_handler)
        self.path = rospkg.RosPack().get_path("parsian_ai")
        print ("pack path: " + self.path)
        self.path += "/plans/"
        self.__observer = Observer()
        # DIRECTORY_TO_WATCH = DIRECTORY_TO_WATCH + self.__p
        self.__event_handler = Handler(self.path)

        # l=[]
        # l.append("/home/fateme/Workspace/parsian_ws/src/parsian_ssl/parsian_ai/plans/2Pass/pass.json")
        # self.__event_handler.update_master_active(l, True, True)

    def run(self):
        self.__observer.schedule(self.__event_handler, self.path, recursive=True)
        self.__observer.start()
        try:
            while True:
                time.sleep(5)
        except:
            self.__observer.stop()
        # print("Error")
        sys.exit(0)
        self.__observer.join()

    def get_all_plans(self):
        return self.__event_handler.get_all_plans_msgs()

    def update_master_active(self, name_list, plan_index, is_master, is_active):
        if len(name_list) != len(plan_index):
            print("not all plans are indexed")
            return None
        return self.__event_handler.update_master_active(name_list, plan_index, is_master, is_active)

    def choose_plan(self, player_num, game_mode):
        return self.__event_handler.choose_plan(player_num, game_mode)

    def signal_handler(self, signal, frame):
        print("\nctrl+C pressed!")
        sys.exit(0)

class Handler(FileSystemEventHandler):

    def __init__(self, p):
        self.__final_list = []
        self.__ignore = []
        self.__shuffleCount = 0
        self.__final_dict = []

        global final_list, shuffleCount, path
        path = p

        # read_plan(f[5])
        self.__final_list = self.list_valid_plans(p)

        self.plans_to_dict()

        self.shuffle_indexing(self.__final_list)  # call once

        # self.choose_plan(4, 3)
        # print (self.message_generator(self.__final_dict[0]))

    # @staticmethod
    def on_any_event(self, event):
        if event.is_directory:
            return None

        # elif event.event_type == 'created':
            # print("Received created event - %s." % event.src_path)

        elif event.event_type == 'modified':
            # print("Received modified event - %s" % event.src_path)
            # self.add_plan(event.src_path)
            self.refresh()

        elif event.event_type == 'deleted':
            # print("Received deleted event - %s" % event.src_path)
            self.refresh()
            # self.remove_plan(event.src_path)

    def list_valid_plans(self, path):
        file_list = []
        for path, dirs, files in os.walk(path):
            for filename in files:
                fname = os.path.join(path, filename)
                #         printfname.rsplit('/', 1)[1]   # just file name
                file_list.append(fname)

        ignore_lst = []
        for f in file_list:  # find ignore list
            if f != None:
                if f.endswith('.ignore'):
                    ignore_lst = open(str(f)).read().split("\n")
                    file_list.remove(f)
                    break

        if '' in ignore_lst:
            ignore_lst.remove('')

        no_cmnt_ignr_lst = [li for li in ignore_lst if not li.startswith('#')]
        print(no_cmnt_ignr_lst)
        self.__ignore = no_cmnt_ignr_lst

        bad_files = []
        for f in file_list:
            if f != None:
                if f.endswith('.json'):
                    # if os.stat(str(f)).st_size != 0:
                        # print("adding " + str(f).rsplit('/', 1)[1]
                        # with open(str(f)) as json_data:
                        #     plan_data.append(json.load(json_data))
                    if os.stat(str(f)).st_size == 0:
                        bad_files.append(f)
                        print("empty: " + str(f) + " --> removed!")
                else:
                    bad_files.append(f)
                    print("not json: " + str(f) + " --> removed!")

        file_list2 = [f for f in file_list if not f in bad_files]

        # printfile_list2

        self.__final_list = self.ignore_plans(file_list2, ignore_lst)
        return self.__final_list

    def add_plan(self, path_to_plan):
        flag = 0
        print ("adding plan...")
        if str(path_to_plan).endswith(".json"):
            if not (str(path_to_plan).split("/plans")[1]) in self.__ignore:
                for pattern in self.__ignore:
                    if pattern != '':
                        try:
                            if (re.search(pattern, str(path_to_plan).split("/plans")[1]) and
                                    re.search(pattern, str(path_to_plan).split("/plans")[1]).start() == 0):
                                flag = 1
                        except:
                            print("Invalid Expression: " + pattern)
            else:
                flag = 1

        if flag == 0:
            self.__final_list.append(path_to_plan)
            print(str(path_to_plan).split("/plans")[1]+" added.")

        return self.__final_list

    def remove_plan(self, path_to_plan):
        if path_to_plan in self.__final_list:
            print(str(path_to_plan).split("/plans")[1]+" removed.")
            self.__final_list.remove(path_to_plan)

    def refresh(self):
        global path
        self.__final_list = self.list_valid_plans(path)
        self.plans_to_dict()

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

        print("ignored plans:")
        for fil in new_file_list2:
            str_list = str(fil).split("/plans")
            if len(str_list) > 0:
                print("\t"+[1])

        last = [term for term in new_file_list if not term in new_file_list2]

        print("FINAL LIST:")
        for i in last:
            print("\t"+str(i).split("/plans")[1])

        return last

    def shuffle_indexing(self, alist):
        random.shuffle(alist)

    def read_plan(self, plan_path):
        print("opening plan "+str(plan_path).split("/plans")[1])
        with open(str(plan_path)) as json_data:
            a = (json.load(json_data))

            print(a["apiVersion"])
            print(a["id"])

            plans_cnt = len(a["plans"])
            for p_i in range(0,plans_cnt):
                print("-------------------------------plan %d:"%p_i)
                plan_i = a["plans"][p_i]
                agents_cnt = len(plan_i["agents"])
                for p_a_i in range(0, agents_cnt):
                    plan_i_ag = plan_i["agents"][p_a_i]
                    print("agent ID: ")
                    print(plan_i_ag["ID"])
                    plan_i_ag_i_pos = plan_i_ag["positions"]
                    agents_pos_cnt = len(plan_i_ag["positions"])
                    for p_a_p_i in range(0, agents_pos_cnt):
                        print(plan_i_ag_i_pos[p_a_p_i]["angel"])
                        print(plan_i_ag_i_pos[p_a_p_i]["pos-x"])
                        print(plan_i_ag_i_pos[p_a_p_i]["pos-y"])
                        print(plan_i_ag_i_pos[p_a_p_i]["tolerance"])
                        plan_i_ag_i_pos_i_s = plan_i_ag_i_pos[p_a_p_i]["skills"]
                        agents_pos_skill_cnt = len(plan_i_ag_i_pos[p_a_p_i]["skills"])
                        for p_a_p_S_i in range(0, agents_pos_skill_cnt):
                            print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["flag"])
                            print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["name"])
                            print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["primary"])
                            print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["secondary"])
                            if "target" in plan_i_ag_i_pos_i_s[p_a_p_S_i]:
                                print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["target"]["agent"])
                                print(plan_i_ag_i_pos_i_s[p_a_p_S_i]["target"]["index"])
                            else:
                                print("no target for this skill")
                            print("")
                print(plan_i["chance"])
                print(plan_i["lastDist"])
                print(plan_i["planMode"])
                print(plan_i["tags"])
                print(plan_i["ballInitPos"]["x"])
                print(plan_i["ballInitPos"]["y"])

    def choose_plan(self, player_num, game_mode):
        # DIRECT   = 1
        # INDIRECT = 2
        # KICKOFF  = 3
        plan_mode = ""
        if game_mode == 1:
            plan_mode = "DIRECT"
        elif game_mode == 2:
            plan_mode = "INDIRECT"
        elif game_mode == 3:
            plan_mode = "KICKOFF"

        #get a sublist from final_list based on player_num and game_mode
        sublist = []
        for plan in self.__final_dict:
            if len(plan["agentInitPos"]) >= player_num and plan["planMode"] == plan_mode and plan["chance"] > 0 and plan["lastDist"] >= 0:
                print("matched: "+plan["filename"].split("plans/")[1]+" --- num agents: "+str(len(plan["agentInitPos"])))
                sublist.append(plan)

        return sublist[0]

    def get_all_plans_msgs(self):
        plans_msg = []
        for plan in self.__final_dict:
            plans_msg.append(self.message_generator(plan))
        return plans_msg

    def update_master_active(self, name_list, plan_index, is_master, is_active):
        for plan in self.__final_dict:
            for i in range(0, len(name_list)):
                if plan["filename"] == name_list[i] and plan["index"] == plan_index[i]:
                    plan["isMaster"] = is_master
                    plan["isActive"] = is_active

        return self.get_all_plans_msgs()

    def plans_to_dict(self):
        self.__final_dict = []
        for plan in self.__final_list:
            with open(str(plan)) as json_data:
                tmp = json.load(json_data)
                for i in range(0, len(tmp["plans"])):
                    dict1 = tmp["plans"][i]
                    dict1.update({"index": i})
                    dict1.update({"filename": str(plan)})
                    dict1.update({"isMaster": False})
                    dict1.update({"isActive": True})
                    dict1.update({"successRate": 0})
                    dict1.update({"planRepeat": 0})
                    self.__final_dict.append(dict1)
        return self.__final_dict

    def message_generator(self, plan_dict):
        plan_response = parsian_plan
        plan_msg = parsian_plan()
        plan_msg.isActive      = plan_dict["isActive"]
        plan_msg.isMaster      = plan_dict["isMaster"]
        plan_msg.planFile      = plan_dict["filename"]
        plan_msg.agentSize     = len(plan_dict["agentInitPos"])
        plan_msg.chance        = plan_dict["chance"]
        plan_msg.lastDist      = plan_dict["lastDist"]
        plan_msg.tags          = plan_dict["tags"]
        plan_msg.planMode      = plan_dict["planMode"]
        plan_msg.ballInitPos.x = plan_dict["ballInitPos"]["x"]
        plan_msg.ballInitPos.y = plan_dict["ballInitPos"]["y"]
        plan_msg.planRepeat    = plan_dict["planRepeat"]
        plan_msg.succesRate    = plan_dict["successRate"]
        return plan_msg


if __name__ == '__main__':
    w = Watcher()
    w.run()
