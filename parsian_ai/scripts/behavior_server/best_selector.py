from parsian_msgs.msg import parsian_behavior
from parsian_msgs.msg import parsian_ai_status
from threading import Timer

threshold_amount = .1
queue_size = 10

class BestSelector:
    def __init__(self):
        self.data = {}
        self.last_best = None
        self.upper_bound = 0.9
        self.lower_bound = 0.1
        self.timer = Timer(3, self.timer_cb)
        self.hasTimePassed = True

    def update_data(self, new_action):
        # type:( parsian_behavior ) -> None
        if new_action.name not in self.data.keys():
            self.data[new_action.name] = NQueue(queue_size)
        self.data[new_action.name].update(new_action)

    def get_best(self):
        if len(self.data) is 0:
            return -1

        # gets the best plan and checks if it is beyond boundries
        best = self.data[max(self.data, key=lambda x: self.data[x].get_average())]
        best = self.check_bounds(best)

        if self.last_best is not None:
            self.last_best.has_threshold = 0
        best.has_threshold = 1
        self.last_best = best
        return best.queue[0]

    def update_success_rate(self, ai_status):
        # type:(parsian_ai_status) -> None
        self.data[ai_status.behavior].update_success_rate(ai_status.success_rate)

    def update_config(self, q_size, th_amount):
        global queue_size, threshold_amount
        queue_size = q_size
        threshold_amount = th_amount
        
    def check_bounds(self, action):
        if action.probability < self.lower_bound:
            if self.last_best is not None:
                return self.last_best
            else:
                return -1
        elif self.last_best.probabilty > self.upper_bound:
            return self.last_best
        else:
            return action

    def timer_cb(self):
        self.hasTimePassed = True

class NQueue:
    def __init__(self, length):
        self.queue = []
        self.success_rate = -1
        self.has_threshold = 0

    def update(self, action):
        if len(self.queue) >= queue_size:
            self.queue.pop()
        self.queue.insert(0, action)

    def get_average(self):
        if len(self.queue) is 0:
            return 0
        return (sum([action.probability for action in self.queue]) / len(self.queue)) + self.has_threshold * threshold_amount

    def update_success_rate(self, success_rate):
        self.success_rate = success_rate

    def get_corrected_eval(self):
        pass

    def Geometric_mean(self, first, f_coef, second, s_coef):
        return (first ** f_coef * second ** s_coef) ** (1 / (f_coef + s_coef))

