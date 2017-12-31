import time
import os

import re
import json

import subprocess

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

final_list = []
ignore = []

def load_valid_plans(path):
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
    print no_cmnt_ignr_lst
    global ignore
    ignore = no_cmnt_ignr_lst

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
                    print "empty: " + str(f) + " --> removed!"
            else:
                bad_files.append(f)
                print "not json: " + str(f) + " --> removed!"

    file_list2 = [f for f in file_list if not f in bad_files]

    # print file_list2
    global final_list
    final_list = ignore_plans(file_list2, ignore_lst)
    return final_list


def add_plan(path_to_plan):
    global final_list
    flag = 0
    if str(path_to_plan).endswith(".json"):
        if not (str(path_to_plan).split("/plans")[1]) in ignore:
            for pattern in ignore:
                if pattern != '':
                    try:
                        if (re.search(pattern, str(path_to_plan).split("/plans")[1]) and
                                re.search(pattern, str(path_to_plan).split("/plans")[1]).start() == 0):
                            flag = 1
                    except:
                        print "Invalid Expression: " + pattern
        else:
            flag = 1

    if flag == 0:
        final_list.append(path_to_plan)
        print str(path_to_plan).split("/plans")[1]+" added."

    return final_list


def remove_plan(path_to_plan):
    if path_to_plan in final_list:
        print str(path_to_plan).split("/plans")[1]+" removed."
        final_list.remove(path_to_plan)


def ignore_plans(file_list, ignore_list):
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
                print "Invalid Expression: "+pattern

    print "ignored plans:"
    for fil in new_file_list2:
        print "\t"+str(fil).split("/plans")[1]

    last = [term for term in new_file_list if not term in new_file_list2]

    print "FINAL LIST:"
    for i in last:
        print "\t"+str(i).split("/plans")[1]

    return last


class Watcher:
    DIRECTORY_TO_WATCH = ""

    def __init__(self, path):
        self.observer = Observer()
        self.DIRECTORY_TO_WATCH = self.DIRECTORY_TO_WATCH + path

    def run(self):
        event_handler = Handler()
        self.observer.schedule(event_handler, self.DIRECTORY_TO_WATCH, recursive=True)
        self.observer.start()
        try:
            while True:
                time.sleep(5)
        except:
            self.observer.stop()
            print "Error"
        self.observer.join()


class Handler(FileSystemEventHandler):

    @staticmethod
    def on_any_event(event):
        if event.is_directory:
            return None

        # elif event.event_type == 'created':
            # print "Received created event - %s." % event.src_path

        elif event.event_type == 'modified':
            # print "Received modified event - %s" % event.src_path
            add_plan(event.src_path)

        elif event.event_type == 'deleted':
            print "Received deleted event - %s" % event.src_path
            remove_plan(event.src_path)


if __name__ == '__main__':
    p = os.getcwd().rsplit('/', 1)[0]
    p = p.rsplit('/', 1)[0]
    p = p + "/plans/"

    f = load_valid_plans(p)
    w = Watcher(p)
    w.run()