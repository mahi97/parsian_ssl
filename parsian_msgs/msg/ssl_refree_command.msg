# A referee command.
# This can take on any of the values defined below.
int32 command


# All robots should completely stop moving.
int32 HALT = 0
# Robots must keep 50 cm from the ball.
int32 STOP = 1
# A prepared kickoff or penalty may now be taken.
int32 NORMAL_START = 2
# The ball is dropped and free for either team.
int32 FORCE_START = 3
# The us team may move into kickoff position.
int32 PREPARE_KICKOFF_YELLOW = 4
# The them team may move into kickoff position.
int32 PREPARE_KICKOFF_BLUE = 5
# The us team may move into penalty position.
int32 PREPARE_PENALTY_YELLOW = 6
# The them team may move into penalty position.
int32 PREPARE_PENALTY_BLUE = 7
# The us team may take a direct free kick.
int32 DIRECT_FREE_YELLOW = 8
# The them team may take a direct free kick.
int32 DIRECT_FREE_BLUE = 9
# The us team may take an indirect free kick.
int32 INDIRECT_FREE_YELLOW = 10
# The them team may take an indirect free kick.
int32 INDIRECT_FREE_BLUE = 11
# The us team is currently in a timeout.
int32 TIMEOUT_YELLOW = 12
# The them team is currently in a timeout.
int32 TIMEOUT_BLUE = 13
# The us team just scored a goal.
# For information only.
# For rules compliance, teams must treat as STOP.
int32 GOAL_YELLOW = 14
# The them team just scored a goal.
int32 GOAL_BLUE = 15
# Equivalent to STOP, but the us team must pick up the ball and
# drop it in the Designated Position.
int32 BALL_PLACEMENT_YELLOW = 16
# Equivalent to STOP, but the them team must pick up the ball and drop
# it in the Designated Position.
int32 BALL_PLACEMENT_BLUE = 17
