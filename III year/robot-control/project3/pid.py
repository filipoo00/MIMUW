# TODO: implement a class for PID controller
class PID:
    def __init__(self, gain_prop, gain_int, gain_der, sensor_period):
        self.gain_prop = gain_prop
        self.gain_der = gain_der
        self.gain_int = gain_int
        self.sensor_period = sensor_period
        # TODO: add aditional variables to store the current state of the controller
        self.integral = 0

    # TODO: implement function which computes the output signal
    def output_signal(self, commanded_variable, sensor_readings):
        current = sensor_readings[0]
        previous = sensor_readings[1]

        error_curr = commanded_variable - current
        error_prev = commanded_variable - previous

        error_der = (error_curr - error_prev) / self.sensor_period
        
        self.integral += error_curr * self.sensor_period

        output = (
            self.gain_prop * error_curr + 
            self.gain_int * self.integral +
            self.gain_der * error_der
        )

        return output