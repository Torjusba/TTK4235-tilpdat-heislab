# TTK4235-tilpdat-heislab



## In progress:
- Be able to move back to last floor after estop (hw test needed)
- Integrate with orders (hw test needed)
- Fix bug #2 - Not reproduced after 827ff04


## TODO:
- Open door if elevator is ordered to current floor?


## Known bugs:
1. Elevator goes oob if another floor button is held down while moving(most likely introduced by test button interface)
2. Elevator segfaults randomly, not quite sure why
  - Reproduce 1: `orders_get_new_target_(HARDWARE_MOVEMENT_DOWN, 3)`c spamming buttons
  - Reproduce 2: `orders_get_new_target_(HARDWARE_MOVEMENT_DOWN, 3)`c spamming buttons
  - Reproduce 3: `orders_get_new_target_(HARDWARE_MOVEMENT_UP, 0)`c after estop
  - Reproduce 4: `orders_get_new_target_(HARDWARE_MOVEMENT_DOWN, 3)`c spamming buttons
  - Reproduce 4: `orders_get_new_target_(HARDWARE_MOVEMENT_DOWN, 3)`c spamming buttons zxcsd

