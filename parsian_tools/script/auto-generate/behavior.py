#!/usr/bin/python3
import pystache
import datetime
import os
import re
import argparse

rend = pystache.Renderer()

def parse_args():
    parser = argparse.ArgumentParser(description="Behavior Auto-generator")
    parser.add_argument("name", type=str, help="the name of the behavior")
    parser.add_argument("-d", "--description", type=str, help="the description of the behavior", default="")
    return parser.parse_args()


def get_dict(behav):
    d = {
        'behavior': str(behav['name']),
        'Tbehavior': str(behav['name']).title(),
        'UCbehavior': str(behav['name']).upper(),
        'behavior_description': 'describe me' if len(behav['description']) is 0 else behav['description']
    }
    return d


def generate_actions(renderer, behav):
    # Clean Out Folder
    if os.path.isdir(os.getcwd() + os.sep + 'out'):
        for f in os.listdir(os.path.join(os.getcwd(), 'out')):
            os.remove(os.getcwd() + os.sep + 'out' + os.sep + f)
    else:
        os.mkdir(os.getcwd() + os.sep + 'out')

    for template in os.listdir(os.getcwd() + os.sep + 'templates'):
        with open("out/" + template.replace('behavior', behav['behavior'])[:-len('.mustache')], "w") as f:
            f.write(renderer.render_path('templates' + os.sep + template, behav))


def main():
    args = parse_args()
    behav = \
        {
            'name': args.name,
            'description': args.description
        }
    generate_actions(rend, get_dict(behav))


if __name__ == "__main__":
    main()