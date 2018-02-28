class BestSelector:
    def __init__(self):
        self.data = {}

    def update_data(self, new_action):
        if new_action.name not in self.data.keys():
            self.data[new_action.name] = NQueue(30)

        self.data[new_action.name].update(new_action.probability)

    def get_best(self):
        return max(self.data, key=lambda x: self.data[x].get_average())

class NQueue:
    def __init__(self, length):
        self.queue = []
        self.max_len = length

    def update(self, probability):
        if len(self.queue) >= self.max_len:
            self.queue.pop()
        self.queue.insert(0, probability)

    def get_average(self):
        return sum(self.queue) / len(self.queue)