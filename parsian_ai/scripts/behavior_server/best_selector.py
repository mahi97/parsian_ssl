from parsian_msgs.msg import parsian_behavior
from parsian_msgs.msg import parsian_ai_status
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

    def update_success_rate(self,ai_status):
        #type:(parsian_ai_status)
        self.data[ai_status.behavior].update_success_rate(ai_status.success_rate)

class NQueue:
    def __init__(self, length):
        self.queue = []
        self.success_rate = -1
        self.max_len = length

    def update(self, action):
        if len(self.queue) >= self.max_len:
            self.queue.pop()
        self.queue.insert(0, action)

    def get_average(self):
        return sum([action.probability for action in self.queue]) / len(self.queue)

    def update_success_rate(self, success_rate):
        self.success_rate = success_rate

    def get_corrected_eval(self):
        pass

    def Geometric_mean(self, first, f_coef, second, s_coef):
        return ( first**f_coef * second**s_coef ) ** (1/(f_coef+s_coef))