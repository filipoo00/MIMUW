import mujoco
from mujoco import viewer
import numpy as np

model = mujoco.MjModel.from_xml_path("scene.xml")
data = mujoco.MjData(model)
mujoco.mj_forward(model, data)
viewer = viewer.launch_passive(model, data)
viewer.cam.distance = 4.
viewer.cam.lookat = np.array([0, 0, 1])
viewer.cam.elevation = -30.

from drone_simulator import DroneSimulator
from pid import PID

if __name__ == '__main__':
    desired_altitude = 2

    # If you want the simulation to be displayed more slowly, decrease rendering_freq
    # Note that this DOES NOT change the timestep used to approximate the physics of the simulation!
    drone_simulator = DroneSimulator(
        model, data, viewer, desired_altitude = desired_altitude,
        altitude_sensor_freq = 0.01, wind_change_prob = 0.1, rendering_freq = 1
        )

    # TODO: Create necessary PID controllers using PID class

    pid_altitude = PID(
        gain_prop=0.1, gain_int=0.0, gain_der=0.7,
        sensor_period=drone_simulator.altitude_sensor_period
        )
    
    pid_acc = PID(
        gain_prop=0.0, gain_int=20.0, gain_der=0.0, 
        sensor_period=model.opt.timestep
        )

    acc_curr_prev = [data.sensor("body_linacc").data[2] - 9.81] * 2
    # Increase the number of iterations for a longer simulation
    for i in range(4000):
        # TODO: Use the PID controllers in a cascade designe to control the drone
        if i % (1 / drone_simulator.altitude_sensor_freq) == 0:
            a_desired = pid_altitude.output_signal(
                commanded_variable=desired_altitude,
                sensor_readings=drone_simulator.measured_altitudes
            )

        acc_curr = data.sensor("body_linacc").data[2] - 9.81
        acc_curr_prev = [acc_curr, acc_curr_prev[0]]

        t_desired = pid_acc.output_signal(
            commanded_variable=a_desired,
            sensor_readings=acc_curr_prev
        )

        drone_simulator.sim_step(t_desired)