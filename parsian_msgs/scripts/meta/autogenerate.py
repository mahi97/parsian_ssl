#!/usr/bin/env python3.5

import pystache
import datetime
import os
import re

PARSIAN_TYPE_MAP = {
    "bool": "bool",
    "uint8": "quint8",
    "int8": "qint8",
    "uint16": "quint16",
    "int16": "qint16",
    "uint32": "quint32",
    "int32": "qint32",
    "uint64": "quint64",
    "int64": "qint64",
    "float32": "float",
    "float64": "double",
    "string": "string",
    "enum": "enum",
    "vector2D": "Vector2D",
    "parsian_msgs/vector2D": "Vector2D",
    "parsian_msgs/parsian_skill_gotoPoint": 'gotoPoint'
}

PARSIAN_PROPERTIES_LIST = ["Vector2D",]

min_max_regex = re.compile('\[([0-9\.\-]+)\:([0-9\.\-]+)\]')
rend = pystache.Renderer()


def strip_text(text):
    return re.sub("\s\s+", " ", text.strip().replace('\n', '').replace('\r', '')).replace('"', '').replace("'", "")


def cap_word(text):
    return text.lower().title()


def today():
    return datetime.datetime.now().strftime("%Y-%m-%d")


def convert_property(ros_property):
    # print('type : ' + ros_property[0], 'name :' + ros_property[1])
    return PARSIAN_TYPE_MAP.get(ros_property[0]), ros_property[1]


def get_fulldict(file, properties_list):

    action_name = file.split('.')[0].split('_')
    if len(action_name):
        action_name = cap_word(action_name[len(action_name) - 1]) + 'Action'
    print(action_name)

    print(file.split('.')[0])
    new_dict = {"action_name": action_name,
                "has_base": False,
                "base_message": '',
                "properties": [],
                "parsian_properties": [],
                "message": file.split('.')[0],
                "file_name": action_name.lower()
                }

    # message name
    for m_property in properties_list:
        if str(m_property[1]) == 'base':
            new_dict["has_base"] = True
            new_dict["base_action"] = cap_word(m_property[0].replace('parsian_msgs/', '')) + 'Action'
            new_dict["base_action_file"] = str(new_dict["base_action"]).lower() + '.h'
            new_dict["base_message"] = "parsian_skill_" + str(m_property[0].replace('parsian_msgs/', ''))
        else:
            p = {"type": m_property[0], "name": m_property[1].title(), "local": m_property[1]}
            if m_property[0] in PARSIAN_PROPERTIES_LIST:
                new_dict['parsian_properties'].append(p)
            else:
                new_dict['properties'].append(p)
    print(new_dict)
    return new_dict


def generate_actions(folder):

    # Clean Out Folder
    if os.path.isdir(os.getcwd() + os.sep + 'out'):
        for f in os.listdir(os.path.join(os.getcwd(), 'out')):
            os.remove(os.getcwd() + os.sep + 'out' + os.sep + f)
    else:
        os.mkdir(os.getcwd() + os.sep + 'out')

    for m_file in os.listdir(folder):

        if m_file.startswith('parsian_skill_') is False:
            continue
        action_dict = {}
        ros_property_list = []
        parsian_property_list = []
        with open(os.path.join(folder, m_file)) as msg:
            for line in msg.readlines():
                line = line.replace("\n", '')
                ros_property_list.append(tuple(line.split(' ')))
            parsian_property_list = [convert_property(ros_property) for ros_property in ros_property_list
                                     if PARSIAN_TYPE_MAP.get(ros_property[0]) and len(ros_property) is 2]
            dict = get_fulldict(m_file, parsian_property_list)

        with open("out/" + dict['action_name'].lower() + '.h', "w") as f:
            f.write(rend.render_path('templates/action.h.mustache', dict))

        with open("out/" + dict['action_name'].lower() + '.cpp', "w") as f:
            f.write(rend.render_path('templates/action.cpp.mustache', dict))
        # print(rend.render_path('templates/action.h.mustache', dict))
        # print(file)


def main():
    # Setup stuff
    if os.getcwd().endswith('/parsian_ssl/parsian_msgs/scripts/meta') is False:
        print("Please run script on /scripts/meta folder")
        exit(1)

    generate_actions(os.pardir + os.sep + os.pardir + os.sep + 'msg')


if __name__ == "__main__":
    main()
