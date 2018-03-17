from parsian_msgs.msg import parsian_behavior
from parsian_msgs.msg import parsian_ai_status


threshold_amount = .1
queue_size = 5


class BestSelector:
    def __init__(self):

        # data: a dictionary with name of behaviors as keys and a NQueue of them as values
        self.data = {}

        # last best behavior that has been selected
        self.last_best = None

        # a dictionary representing the reward and enal
        self.rewards_penalties = {}

        # upper and lower bounds for behavior evaluation
        self.upper_bound = 0.9
        self.lower_bound = 0.1

    def update_data(self, new_behavior):
        # type:( parsian_behavior ) -> None
        if new_behavior.name not in self.data.keys():
            self.data[new_behavior.name] = NQueue(queue_size)
            if new_behavior.name in self.rewards_penalties:
                print(new_behavior.name + " behavior detected!")
                value = self.rewards_penalties[new_behavior.name]
                self.data[new_behavior.name].update_reward_penalty(value["reward"], value["penalty"])

        self.data[new_behavior.name].update(new_behavior)


    def get_best(self):
        if len(self.data) is 0:
            return -1

        # gets the best plan and checks if it is beyond boundries
        best = self.data[max(self.data, key=lambda x: self.data[x].get_effective_probability())]
        best = self.check_bounds(best)

        if self.last_best is not None:
            self.last_best = best
            self.last_best.has_threshold = 0
        best.has_threshold = 1

        return best.queue[0]

    def update_success_rate(self, ai_status):
        # type:( parsian_ai_status ) -> None
        self.data[ai_status.behavior].update_success_rate(ai_status.success_rate)

    def update_config(self, q_size, th_amount, upper_b, lower_b):
        global queue_size, threshold_amount
        queue_size = q_size
        threshold_amount = th_amount
        self.upper_bound = upper_b
        self.lower_bound = lower_b

    def check_bounds(self, best_behavior):
        if self.last_best.probabilty > self.upper_bound:
            return self.last_best
        else:
            return best_behavior

    def update_rewards_penalties(self, rewards_penalties):
        self.rewards_penalties = rewards_penalties
        for name in rewards_penalties:
            if name in self.data:
                print(name + " behavior detected!")
                value = rewards_penalties[name]
                self.data[name].update_reward_penalty(value["reward"], value["penalty"])


# a structure for an action, including a stack of its instances, and it's evaluation properties
# like reward, penalty and threshold
class NQueue:
    def __init__(self, length):
        self.queue = []
        self.success_rate = -1
        self.has_threshold = 0
        self.penalty = .5
        self.reward = .5

    def update_reward_penalty(self,reward, penalty):
        self.reward = reward
        self.penalty = penalty

    def update(self, behavior):
        if len(self.queue) >= queue_size:
            self.queue.pop()
        self.queue.insert(0, behavior)

    def get_effective_probability(self):
        """
        returns the effective probability of the current behavior
        based on the average and reward/penalty of the behavior
        """
        if len(self.queue) is 0:
            return 0
        average_probability = (sum([behavior.probability for behavior in self.queue]) / len(self.queue))
        effective_probability = (0.3*average_probability + 0.7*self.queue[0].probability) * self.reward - (1 - average_probability) * self.penalty
        return effective_probability + self.has_threshold * threshold_amount

    def get_average_probability(self):
        return sum([behavior.probability for behavior in self.queue]) / len(self.queue)

    def update_success_rate(self, success_rate):
        self.success_rate = success_rate

    def get_corrected_eval(self):
        pass

    def Geometric_mean(self, first, f_coef, second, s_coef):
        return (first ** f_coef * second ** s_coef) ** (1 / (f_coef + s_coef))

