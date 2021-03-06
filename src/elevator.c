#include "elevator.h"
#define TUI

static ElevatorState m_elevator_current_state;
static int m_elevator_current_target;
static int m_elevator_current_floor;

// Last floor and movement direction must be remembered in case elevator is e-stopped between floors
static HardwareMovement m_elevator_last_movement_direction;
static int m_elevator_last_floor;
//orders_init();

int main(int argc, char **argv)
{

    int error = hardware_init();
    if (error != 0)
    {
        fprintf(stderr, "unable to initialize hardware\n");
        exit(1);
    }
    error = elevator_init();
    if (error != 0)
    {
        fprintf(stderr, "unable to initialize elevator FSM\n");
        exit(1);
    }
    init_orders();

#ifdef TUI
    printf("=== Elevator control ===\n");
#endif
    while (1)
    {
#ifdef TUI
        printf("\r Last: %d | Current: %d | Target: %d \t\t", m_elevator_last_floor, m_elevator_current_floor, m_elevator_current_target);

#endif

        // Don't update orders in ESTOP or INIT states
        if (m_elevator_current_state != ELEVATOR_STATE_ESTOP && m_elevator_current_state != ELEVATOR_STATE_ESTOP_OPEN &&
            m_elevator_current_state != ELEVATOR_STATE_INIT && m_elevator_current_state != ELEVATOR_STATE_INIT_MOVING_DOWN)

            if (elevator_update_orders(m_elevator_last_movement_direction))
            {
                fprintf(stderr, "Unable to update orders");
            }

        if (elevator_update_floor_status())
        {
            fprintf(stderr, "Unable to update floor status");
        }

        if (elevator_update_state())
        {
            fprintf(stderr, "Unable to update state");
        }
    }
    return 0;
}

int elevator_init()
{
    m_elevator_current_state = ELEVATOR_STATE_INIT;
    m_elevator_current_floor = -1;
    m_elevator_current_target = -1;
    m_elevator_last_movement_direction = HARDWARE_MOVEMENT_STOP;
    return 0;
}

int elevator_update_floor_status()
{
    for (int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++)
    {
        if (hardware_read_floor_sensor(floor))
        {
            if (floor != m_elevator_current_floor)
            {
                hardware_command_floor_indicator_on(floor);
                m_elevator_current_floor = floor;
                m_elevator_last_floor = floor;
            }
            return 0;
        }
    }
    m_elevator_current_floor = -1;
    return 0;
}

// TODO: Reference document
// This function should not be read without its acompanying state diagram, found on page {PAGE} in rapport.pdf
int elevator_update_state()
{
    // ESTOP takes priority, but only if not already estopped
    if (m_elevator_current_state != ELEVATOR_STATE_ESTOP && m_elevator_current_state != ELEVATOR_STATE_ESTOP_OPEN)
    {
        if (hardware_read_stop_signal())
        {
            elevator_clear_orders();
            hardware_command_stop_light(1);
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            m_elevator_current_state = ELEVATOR_STATE_ESTOP;
        }
    }

    switch (m_elevator_current_state)
    {
    case ELEVATOR_STATE_INIT:
        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        m_elevator_current_state = ELEVATOR_STATE_INIT_MOVING_DOWN;
        m_elevator_last_movement_direction = HARDWARE_MOVEMENT_DOWN;
        break;

    case ELEVATOR_STATE_INIT_MOVING_DOWN:
        if (m_elevator_current_floor != -1)
        {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            m_elevator_current_state = ELEVATOR_STATE_IDLE;
            m_elevator_last_movement_direction = HARDWARE_MOVEMENT_STOP;
        }
        break;

    case ELEVATOR_STATE_IDLE:
        if (m_elevator_current_target != -1)
        {
            if (m_elevator_current_target != m_elevator_current_floor)
            {
                // Uses last_floor + last_movement modifier instead of current_floor because
                // the elevator can be stopped by the stop button between floors
                if ((float)m_elevator_current_target < (float)m_elevator_last_floor - 0.5 * ((float)m_elevator_last_movement_direction - 1))
                {
                    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                    m_elevator_current_state = ELEVATOR_STATE_MOVING_DOWN;
                    m_elevator_last_movement_direction = HARDWARE_MOVEMENT_DOWN;
                }
                else
                {
                    hardware_command_movement(HARDWARE_MOVEMENT_UP);
                    m_elevator_current_state = ELEVATOR_STATE_MOVING_UP;
                    m_elevator_last_movement_direction = HARDWARE_MOVEMENT_UP;
                }
            }
            else
            {
                // Elevator is already at target
                m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
            }
        }
        break;

    case ELEVATOR_STATE_MOVING_DOWN:
        if (m_elevator_current_target == m_elevator_current_floor)
        {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            m_elevator_last_movement_direction = HARDWARE_MOVEMENT_STOP;
            if (m_elevator_current_floor != -1)
            {
                m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
            }
            else
            {
                // Engage estop if moving with no target
                fprintf(stderr, "Target set to -1 while moving. Engaging emergency stop\n");
                m_elevator_current_state = ELEVATOR_STATE_ESTOP;
            }
        }
        break;

    case ELEVATOR_STATE_MOVING_UP:
        if (m_elevator_current_target == m_elevator_current_floor)
        {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
            m_elevator_last_movement_direction = HARDWARE_MOVEMENT_STOP;
        }
        break;

    case ELEVATOR_STATE_STOPPED_AT_FLOOR:
        hardware_command_door_open(1);
        timer_reset();
        m_elevator_current_state = ELEVATOR_STATE_OPEN;
        break;

    case ELEVATOR_STATE_OPEN:
        if (hardware_read_obstruction_signal())
        {
            m_elevator_current_state = ELEVATOR_STATE_OBSTRUCTION;
        }
        else
        {
            if (timer_get_seconds() >= ELEVATOR_DOOR_WAIT_TIME)
            {
                hardware_command_door_open(0);
                elevator_clear_target();
                m_elevator_current_state = ELEVATOR_STATE_IDLE;
            }
        }
        break;

    case ELEVATOR_STATE_OBSTRUCTION:
        if (!hardware_read_obstruction_signal())
        {
            timer_reset();
            m_elevator_current_state = ELEVATOR_STATE_OPEN;
        }
        break;

    case ELEVATOR_STATE_ESTOP:
        if (m_elevator_current_floor != -1)
        {
            hardware_command_door_open(1);
            m_elevator_current_state = ELEVATOR_STATE_ESTOP_OPEN;
        }
        else if (!hardware_read_stop_signal())
        {
            hardware_command_stop_light(0);
            m_elevator_current_state = ELEVATOR_STATE_IDLE;
        }
        break;

    case ELEVATOR_STATE_ESTOP_OPEN:
        if (!hardware_read_stop_signal())
        {
            timer_reset();
            m_elevator_current_state = ELEVATOR_STATE_OPEN;
            hardware_command_stop_light(0);
        }
        break;

    default:
        fprintf(stderr, "Invalid state %d", m_elevator_current_state);
        return -1;
    }
    return 0;
}

int elevator_add_order_if_button_pressed(int floor, HardwareOrder hardware_order)
{
    if (hardware_read_order(floor, hardware_order))
    {
        hardware_command_order_light(floor, hardware_order, 1);
        orders_add_order(hardware_order, floor);
    }
    return 0;
}

/** For internal use only. Called when the elevator reaches its target */
int elevator_clear_target()
{
    hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_DOWN, 0);
    hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_INSIDE, 0);
    hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_UP, 0);

    orders_clear_target(m_elevator_last_floor);
    return 0;
}

/** For internal use only. Called when the estop button is pressed */
int elevator_clear_orders()
{
    elevator_clear_target();

    for (int floor = 0; floor <= HARDWARE_NUMBER_OF_FLOORS - 1; floor++)
    {
        hardware_command_order_light(floor, HARDWARE_ORDER_UP, 0);
        hardware_command_order_light(floor, HARDWARE_ORDER_INSIDE, 0);
        hardware_command_order_light(floor, HARDWARE_ORDER_DOWN, 0);
    }

    orders_clear_all();
    return 0;
}

/** For internal use only. Called every loop to poll for order button presses */
int elevator_update_orders()
{
    for (int floor = 0; floor <= HARDWARE_NUMBER_OF_FLOORS - 1; floor++)
    {
        elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_UP);
        elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_INSIDE);
        elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_DOWN);
    }
    m_elevator_current_target = orders_get_new_target(m_elevator_last_movement_direction, m_elevator_last_floor);
    return 0;
}
