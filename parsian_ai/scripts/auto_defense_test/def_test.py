from enum import Enum


class STATE(Enum):
    UserOptions = 1,
    Attack = 2,
    ChangeOrExit = 3


class ATTACK_PLAN(Enum):
    NoPlan = 4,
    Plan1 = 5,
    Plan2 = 6,
    Plan3 = 7,
    Plan4 = 8,
    Plan5 = 9,
    Plan6 = 10


class MasterAttackPlan():
    
    def __init__(self):
        self.robots = []

    def execute(self, wm):
        pass
    
    def update(self, wm):
        pass


class AttackPlan1(MasterAttackPlan):
    
    def __init__(self):
        super().__init__()


class AutoDefenseTest():
    
    def __init__(self):

        self.state = STATE.UserOptions

        self.attack_plan = None
        
        self.plan1 = AttackPlan1()

    def wm_cb(self, wm):
        
        if self.state == STATE.UserOptions:
            self.get_user_options()

        if self.state == STATE.Attack:

            if self.attack_plan == ATTACK_PLAN.Plan1:
                self.plan1.update(wm)
                if self.plan1.execute(wm):
                    self.state = STATE.UserOptions

    def get_user_options(self):
        pass



