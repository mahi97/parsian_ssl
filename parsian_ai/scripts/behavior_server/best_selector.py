from parsian_msgs.msg import parsian_behavior
import rospy
class BestSelector:
    def __init__(self):
        self.data = {}

    def update_data(self, new_action):
        # type:( parsian_behavior )
        if new_action.name not in self.data.keys():
            self.data[new_action.name] = NQueue(30)

        self.data[new_action.name].update(new_action)

    def get_best(self):
        return self.data[max(self.data, key=lambda x: self.data[x].get_average())].queue[0]

class NQueue:
    def __init__(self, length):
        self.queue = []
        self.max_len = length

    def update(self, action):
        if len(self.queue) >= self.max_len:
            self.queue.pop()
        self.queue.insert(0, action)

    def get_average(self):
        return sum([action.probability for action in self.queue]) / len(self.queue)