"""
Stub for homework 2
"""

import time
import random
import numpy as np
import mujoco
from mujoco import viewer


import numpy as np
import cv2
from numpy.typing import NDArray


TASK_ID = 3

WIDTH = 640
HEIGHT = 480

world_xml_path = f"car_{TASK_ID}.xml"
model = mujoco.MjModel.from_xml_path(world_xml_path)
renderer = mujoco.Renderer(model, height=480, width=640)
data = mujoco.MjData(model)
mujoco.mj_forward(model, data)
viewer = viewer.launch_passive(model, data)


def sim_step(
    n_steps: int, /, view=True, rendering_speed = 10, **controls: float
) -> NDArray[np.uint8]:
    """A wrapper around `mujoco.mj_step` to advance the simulation held in
    the `data` and return a photo from the dash camera installed in the car.

    Args:
        n_steps: The number of simulation steps to take.
        view: Whether to render the simulation.
        rendering_speed: The speed of rendering. Higher values speed up the rendering.
        controls: A mapping of control names to their values.
        Note that the control names depend on the XML file.

    Returns:
        A photo from the dash camera at the end of the simulation steps.

    Examples:
        # Advance the simulation by 100 steps.
        sim_step(100)

        # Move the car forward by 0.1 units and advance the simulation by 100 steps.
        sim_step(100, **{"forward": 0.1})

        # Rotate the dash cam by 0.5 radians and advance the simulation by 100 steps.
        sim_step(100, **{"dash cam rotate": 0.5})
    """

    for control_name, value in controls.items():
        data.actuator(control_name).ctrl = value

    for _ in range(n_steps):
        step_start = time.time()
        mujoco.mj_step(model, data)
        if view:
            viewer.sync()
            time_until_next_step = model.opt.timestep - (time.time() - step_start)
            if time_until_next_step > 0:
                time.sleep(time_until_next_step / rendering_speed)

    renderer.update_scene(data=data, camera="dash cam")
    img = renderer.render()
    return img



# TODO: add addditional functions/classes for task 1 if needed

def find_contours(
    img: np.ndarray,
    color: str,
) -> list[np.ndarray]:
    """
    Find contours of the specified color in the image.

    Args:
        img: The image to find contours in.
        color: The color to find contours of.

    Returns:
        A list of contours found in the image.
    """

    # Crop the image to remove some car's parts, to not detect them
    img = img[:-60, ...]

    img_bgr = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    hsv = cv2.cvtColor(img_bgr, cv2.COLOR_BGR2HSV)

    if color == 'red':
        mask1 = cv2.inRange(hsv, (0, 50, 20), (10, 255, 255))
        mask2 = cv2.inRange(hsv, (180, 50, 20), (180, 255, 255))

        mask = mask1 | mask2

    elif color == 'green':
        mask = cv2.inRange(hsv, (45, 50, 20), (70, 255, 255))

    elif color == 'blue':
        mask = cv2.inRange(hsv, (120, 50, 50), (130, 255, 255))

    elif color == 'white':
        mask = cv2.inRange(hsv, (0, 0, 180), (180, 50, 255))

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    return contours


def find_centers(
    contours: list[np.ndarray],
) -> list[tuple[int, int]]:
    """
    Find the centers of the contours in the image.

    Args:
        contours: The contours to find centers of.

    Returns:
        A list of centers of the contours.
    """

    centers = []

    for cnt in contours:
        M = cv2.moments(cnt)

        if M["m00"] != 0:
            c_x = int(M["m10"] / M["m00"])
            c_y = int(M["m01"] / M["m00"])
            centers.append((c_x, c_y))

    return centers
        
# /TODO


def task_1():
    steps = random.randint(0, 2000)
    controls = {"forward": 0, "turn": 0.1}
    img = sim_step(steps, view=False, **controls)

    # TODO: Change the lines below.
    # For car control, you can use only sim_step function

    steps = 200

    # when we start moving the car towards the ball, the position of the ball's center y-coordinate
    # on the image is around 285, when we touch the ball the position of the ball's center y-coordinate
    # is around 310, so we want to stop the car when we are close to the ball
    y_threshold = 306

    while True:

        contours = find_contours(img, color="red")
        centers = find_centers(contours)

        if not centers:
            img = sim_step(steps, **{"turn": 0.1})
        else:
            c_x = centers[0][0]
            c_y = centers[0][1]

            if c_y > y_threshold:
                break

            rot = (WIDTH // 2 - c_x) / WIDTH

            img = sim_step(steps, **{"turn": rot, "forward": 0.1})

    # Now we are in front of the ball, we turn the car for about 45 degrees to the right
    sim_step(steps, **{"forward": 0, "turn": -0.2})

    # Move the car a little bit forward to be closer to the ball
    for _ in range(10):
        sim_step(steps, **{"forward": 0.1, "turn": 0})

    # /TODO


# TODO: add addditional functions/classes for task 2 if needed

# /TODO

def task_2():
    speed = random.uniform(-0.3, 0.3)
    turn = random.uniform(-0.2, 0.2)
    controls = {"forward": speed, "turn": turn}
    img = sim_step(1000, view=True, **controls)

    # TODO: Change the lines below.
    # For car control, you can use only sim_step function
    
    steps = 200

    # Find white box contours
    while True:
        contours = find_contours(img, color="white")

        if not contours:
            img = sim_step(steps, **{"turn": -0.1, "forward": 0})
        else:
            break

    # The car is pointing to the white box, we need to move the car to the left to the blue box
    # We only care about the left blue box, not the right one
    while True:
        contours_blue = find_contours(img, color="blue")
        centers_blue = find_centers(contours_blue)

        contours_green = find_contours(img, color="green")
        centers_green = find_centers(contours_green)

        if centers_blue and centers_green:
            c_x_blue = centers_blue[0][0]
            c_x_green = centers_green[0][0]

            if c_x_blue > c_x_green:
                img = sim_step(steps, **{"turn": 0.1, "forward": 0})
            else:
                break   
        else:
            img = sim_step(steps, **{"turn": 0.1, "forward": 0})

    # Turn a little bit more to the left to point more in the middle of the blue box
    for _ in range(5):
        img = sim_step(steps, **{"forward": 0, "turn": 0.1})

    # Now move the car to the blue box and stop when we are close to the blue box
    blue_box_y_threshold = 325
    while True:
        contours = find_contours(img, color="blue")
        centers = find_centers(contours)

        if not centers:
            img = sim_step(steps, **{"turn": 0.1, "forward": 0})
        else:
            c_x = centers[0][0]
            c_y = centers[0][1]

            if c_y > blue_box_y_threshold:
                break

            rot = 1/2 * (WIDTH // 2 - c_x) / WIDTH

            img = sim_step(steps, **{"turn": rot, "forward": 0.1})

    # Now we are in front of the blue box, we turn the car for about 90 degrees to the left
    # Now we should point to the bottom green box
    for _ in range(5):
        img = sim_step(steps, **{"forward": 0, "turn": 0.1})

    # Move the car a little bit forward to be closer to the green box
    for _ in range(12):
        img = sim_step(steps, **{"forward": 0.1, "turn": 0})

    # Now move the car to the green box and stop when we are close
    green_box_y_threshold = 317
    while True:
        contours_green = find_contours(img, color='green')
        centers_green = find_centers(contours_green)

        if centers_green:
            c_x = centers_green[0][0]
            c_y = centers_green[0][1]

            if c_y > green_box_y_threshold:
                break

            rot = 1/2 * (WIDTH // 2 - c_x) / WIDTH

            img = sim_step(steps, **{"forward": 0.1, "turn": rot})

        else:
            img = sim_step(steps, **{"forward": 0.1, "turn": 0})

    # Turn the car 90 degrees to the left to point to the exit from the maze
    for _ in range(5):
        img = sim_step(steps, **{"forward": 0, "turn": 0.1})

    # Move the car forward to leave the maze
    for _ in range(25):
        img = sim_step(steps, **{"forward": 0.2, "turn": 0})

    # Find the red ball and move the car to the ball
    red_ball_y_threshold = 304
    while True:
        contours = find_contours(img, color="red")
        centers = find_centers(contours)

        if not centers:
            img = sim_step(steps, **{"turn": 0.1, "forward": 0})
        else:
            c_x = centers[0][0]
            c_y = centers[0][1]

            if c_y > red_ball_y_threshold:
                break

            rot = (WIDTH // 2 - c_x) / WIDTH

            img = sim_step(steps, **{"turn": rot, "forward": 0.3})

    # Now we are in front of the ball, we turn the car for about 45 degrees to the right
    sim_step(steps, **{"forward": 0, "turn": -0.2})

    # Move the car a little bit forward to be closer to the ball
    for _ in range(10):
        sim_step(steps, **{"forward": 0.1, "turn": 0})

    # /TODO



def ball_is_close() -> bool:
    """Checks if the ball is close to the car."""
    ball_pos = data.body("target-ball").xpos
    car_pos = data.body("dash cam").xpos
    print(car_pos, ball_pos)
    return np.linalg.norm(ball_pos - car_pos) < 0.2


def ball_grab() -> bool:
    """Checks if the ball is inside the gripper."""
    print(data.body("target-ball").xpos[2])
    return data.body("target-ball").xpos[2] > 0.1


def teleport_by(x: float, y: float) -> None:
    data.qpos[0] += x
    data.qpos[1] += y
    sim_step(10, **{"dash cam rotate": 0})


def get_dash_camera_intrinsics():
    '''
    Returns the intrinsic matrix and distortion coefficients of the camera.
    '''
    h = 480
    w = 640
    o_x = w / 2
    o_y = h / 2
    fovy = 90
    f = h / (2 * np.tan(fovy * np.pi / 360))
    intrinsic_matrix = np.array([[-f, 0, o_x], [0, f, o_y], [0, 0, 1]])
    distortion_coefficients = np.array([0.0, 0.0, 0.0, 0.0, 0.0])  # no distortion

    return intrinsic_matrix, distortion_coefficients


# TODO: add addditional functions/classes for task 3 if needed


def detect_markers(
    image: np.ndarray
) -> tuple[list[np.ndarray], list[np.ndarray]]:
    """
    Detect ArUco markers in the image.

    Args:
        image: The image to detect markers in.
    
    Returns:
        A tuple of corners and ids of the detected markers.
    """

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
    parameters = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)
    corners, ids, _ = detector.detectMarkers(gray)
    
    return corners, ids


def my_estimatePoseSingleMarkers(
    corners: list[np.ndarray],
    object_points: list[np.ndarray],
    mtx: np.ndarray,
    distortion: np.ndarray
) -> tuple[list[np.ndarray], list[np.ndarray], list[np.ndarray]]:
    '''
    This will estimate the rvec and tvec for each of the marker corners detected by:
       corners, ids, rejectedImgPoints = detector.detectMarkers(image)
    corners - is an array of detected corners for each detected marker in the image
    marker_size - is the size of the detected markers
    mtx - is the camera matrix
    distortion - is the camera distortion matrix
    RETURN list of rvecs, tvecs, and trash (so that it corresponds to the old estimatePoseSingleMarkers())

    stolen from stackoverflow
    '''

    trash = []
    rvecs = []
    tvecs = []
    for cors, obj_pts in zip(corners, object_points):
        nada, R, t = cv2.solvePnP(obj_pts, cors, mtx, distortion, False, cv2.SOLVEPNP_IPPE_SQUARE)
        rvecs.append(R)
        tvecs.append(t)
        trash.append(nada)
    return rvecs, tvecs, trash


def sort_markers_by_id(
    corners: list[np.ndarray],
    ids: np.ndarray
) -> tuple[list[np.ndarray], np.ndarray]:
    """
    Sort the markers by ids.

    Args:
        corners: The corners of the markers.
        ids: The ids of the markers.
    
    Returns:
        A tuple of sorted corners and ids.
    """
    
    sorted_indices = np.argsort(ids.flatten())
    corners = [corners[i] for i in sorted_indices]
    ids = ids[sorted_indices]

    return corners, ids


def generate_object_points(
    box_pos: np.ndarray,
    box_size: float,
) -> list[np.ndarray]:
    """
    Generate object points for the markers. 
    We assume that (0, 0, 0) is the center of the world.

    Args:
        box_pos: The position of the box.
        box_size: The size of the box.

    Returns:
        A list of object points for the markers.
    """
    
    x, y  = box_pos
    offset = 0.01

    obj_points = []

    obj_points_front = np.array([
        [x - box_size / 2 + offset, y + box_size / 2, offset],            # right bottom
        [x + box_size / 2 - offset, y + box_size / 2, offset],            # left bottom
        [x + box_size / 2 - offset, y + box_size / 2, box_size - offset], # left top
        [x - box_size / 2 + offset, y + box_size / 2,  box_size - offset] # right top
    ])

    obj_points_left = np.array([
        [x + box_size / 2 - offset, y + box_size / 2, box_size - offset], # right top
        [x + box_size / 2 - offset, y + box_size / 2, offset],            # right bottom
        [x + box_size / 2 + offset, y - box_size / 2, offset],            # left bottom
        [x + box_size / 2 + offset, y - box_size / 2, box_size - offset]  # left top
    ])
    
    # The corners are sorted in the order of the ids.
    # So the first marker is the left side of the box and the second marker is the front side of the box.
    obj_points.append(obj_points_left)
    obj_points.append(obj_points_front)

    return obj_points


def calculate_camera_position(
    rvecs: list[np.ndarray],
    tvecs: list[np.ndarray]
) -> tuple[float, float]:
    """
    Calculate the position of the camera relative to the marker.

    Args:
        rvecs: The rotation vectors of the markers.
        tvecs: The translation vectors of the markers.
    
    Returns:
        A tuple of x, y coordinates of the camera.
    """

    for i in range(len(rvecs)):

        rvec = rvecs[i]
        tvec = tvecs[i]

        R, _ = cv2.Rodrigues(rvec)

        camera_position = -np.dot(R.T, tvec).flatten()

        # The first marker is the left side of the box so it provides y
        if i == 0:
            y = camera_position[1]

        # The second marker is the front side of the box so it provides x
        elif i == 1:
            x = camera_position[0]

    return x, y


def move_rotate_and_grab():
    """
    Move the car and gripper to be able to grab the ball and grab the ball.

    After teleporting the car to the ball, we are always in the same position.
    """

    # We are on the right to the ball. We need to move a little bit more to the right
    # to be able to grab the ball
    for _ in range(20):
        img = sim_step(150, **{"forward": 0.1})

    # lift the gripper
    img = sim_step(10, **{"forward": 0, "lift": 1})

    # rotate the gripper 180 degrees to be above the ball
    for _ in range(20):
        img = sim_step(750, **{"jib rotate": 0.1})

    # Stop rotating the gripper and drop the gripper
    for _ in range(10):
        img = sim_step(100, **{"lift": 0, "jib rotate": 0})

    # The ball is inside the gripper, so we need to close the trapdoor
    img = sim_step(10, **{"trapdoor close/open": 1})

    # Lift the gripper
    for _ in range(10):
        img = sim_step(200, **{"lift": 1})


# /TODO


def task_3():
    start_x = random.uniform(-0.2, 0.2)
    start_y = random.uniform(0, 0.2)
    teleport_by(start_x, start_y)

    # TODO: Get to the ball
    #  - use the dash camera and ArUco markers to precisely locate the car
    #  - move the car to the ball using teleport_by function

    # Ball position and box position hardcoded from xml file.
    # The third coordinate is not important for the solution.
    ball_pos = np.array([1., 2.])
    box_pos = np.array([-0.3, -0.3])
    box_size = 0.1
    offset_x = 0.15
    steps = 100

    mtx, coeff = get_dash_camera_intrinsics()

    for _ in range(100):
        img = sim_step(steps, **{"dash cam rotate": 0.1})

        corners, ids = detect_markers(img)

        if len(corners) >= 2:
            break

    # There can be times when we detect only one marker (only when the car's x - coordinate is around -0.2)
    # So we move the car forward and still rotating the dash cam to detect two markers.
    if len(corners) < 2:
        while True:
            img = sim_step(steps, **{"forward": 0.1, **{"dash cam rotate": 0.1}})

            corners, ids = detect_markers(img)

            if len(corners) >= 2:
                break

    corners, ids = sort_markers_by_id(corners, ids)

    obj_points = generate_object_points(box_pos, box_size)

    rvecs, tvecs, _ = my_estimatePoseSingleMarkers(corners, obj_points, mtx, coeff)

    x, y = calculate_camera_position(rvecs, tvecs)

    # teleport the car to the ball position plus some offset to the right
    teleport_by(ball_pos[0] - x + offset_x, ball_pos[1] - y)

    # /TODO

    assert ball_is_close()

    # TODO: Grab the ball
    # - the car should be already close to the ball
    # - use the gripper to grab the ball
    # - you can move the car as well if you need to

    move_rotate_and_grab()

    # /TODO

    assert ball_grab()


if __name__ == "__main__":
    print(f"Running TASK_ID {TASK_ID}")
    if TASK_ID == 1:
        task_1()
    elif TASK_ID == 2:
        task_2()
    elif TASK_ID == 3:
        task_3()
    else:
        raise ValueError("Unknown TASK_ID")
