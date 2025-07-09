import os
import cv2
import numpy as np
from typing import Dict

def read_images(
    dir_name: str
) -> Dict[str, np.ndarray]:
    '''
    Read all images from the directory and return them as a dictionary.

    :param dir_name: str
        The name of the directory where the images are stored.
    
    :return: Dict[str, np.ndarray]
        A dictionary where the key is the file name and the value is the image.
    '''
    
    images = {}
    for file_name in os.listdir(dir_name):
        image_file = os.path.join(dir_name, file_name)
        image = cv2.imread(image_file)
        images[file_name] = image

    return images


def show_all_images(
    images: list[np.ndarray]
) -> None:
    '''
    Show all images in a list.

    :param images: list[np.ndarray]
        A list of images to show.

    :return: None
    '''
    
    for image in images:
        cv2.imshow("", image)
        cv2.waitKey(0)

    cv2.destroyAllWindows()


def detect_markers(
    img: np.ndarray
) -> tuple[list[np.ndarray], np.ndarray | None]:
    '''
    Detect markers in the image.

    :param img: np.ndarray
        The image where to detect markers.

    :return: tuple[list[np.ndarray], np.ndarray | None]
        A tuple containing the corners of the markers and the ids of the markers.
    '''
    
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_APRILTAG_16h5)
    parameters = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(dictionary, parameters)

    corners, ids, _ = detector.detectMarkers(img)

    return corners, ids


# Not the best solution, but can't find any function 
# that would return the object points.
# We know that the board looks right this w.r.t. ids:
#  28 | 23 | 18
#  ------------
#  29 | 24 | 19
#
# We set the point (0, 0, 0) to be the bottom left corner of the bottom left marker.
# The points are ordered clockwise for each marker, starting from the bottom left corner.
def find_object_points(
    id: int = -1,
    with_distances: bool = False
) -> np.ndarray:
    '''
    Find the object points for the marker.

    :param id: int
        The id of the marker.

    :param with_distances: bool
        If True, the object points will be returned with distances between the markers.
        If False, the object points will be returned without distances between the markers.

    :return: np.ndarray
        The object points for the marker.
    '''
    
    MARKER_SIZE = 168
    GAP_SIZE = 70

    marker_x = 0
    marker_y = 0

    if with_distances:
        # set the marker left bottom corner coordinates for marker
        if (id == 28):                                  # LEFT TOP MARKER
            marker_x = 0
            marker_y = MARKER_SIZE + GAP_SIZE
        elif (id == 23):                                # MIDDLE TOP MARKER
            marker_x = MARKER_SIZE + GAP_SIZE
            marker_y = MARKER_SIZE + GAP_SIZE
        elif (id == 18):                                # RIGHT TOP MARKER
            marker_x = 2 * (MARKER_SIZE + GAP_SIZE)
            marker_y = MARKER_SIZE + GAP_SIZE
        elif (id == 29):                                # LEFT DOWN MARKER
            marker_x = 0
            marker_y = 0
        elif (id == 24):                                # MIDDLE DOWN MARKER
            marker_x = MARKER_SIZE + GAP_SIZE
            marker_y = 0
        elif (id == 19):                                # RIGHT DOWN MARKER
            marker_x = 2 * (MARKER_SIZE + GAP_SIZE)
            marker_y = 0
    
    return np.array([
        [marker_x, marker_y, 0],                                # LEFT DOWN
        [marker_x, marker_y + MARKER_SIZE, 0],                  # LEFT UP
        [marker_x + MARKER_SIZE, marker_y + MARKER_SIZE, 0],    # RIGHT UP
        [marker_x + MARKER_SIZE, marker_y, 0]                   # RIGHT DOWN
    ], dtype=np.float32)


def object_and_image_points(
    images: Dict[str, np.ndarray]
) -> tuple[list[np.ndarray], list[np.ndarray], list[np.ndarray]]:
    '''
    Find the object and image points for the images.

    :param images: Dict[str, np.ndarray]
        A dictionary where the key is the file name and the value is the image.

    :return: tuple[list[np.ndarray], list[np.ndarray], list[np.ndarray]]
        A tuple containing the image points, 
        the object points without known distances, 
        and the object points with known distances.
    '''
    
    obj_pts_with_distances = []
    obj_pts_without_distances = []
    image_points = []

    for img in images.values():
        corners, ids = detect_markers(img)

        image_points.extend([corner[0] for corner in corners])
        obj_pts_without_distances.extend([find_object_points(
                                                    with_distances=False)
                                                    for _ in ids]
                                                    )
        obj_pts_with_distances.extend([find_object_points(
                                                    id, 
                                                    with_distances=True)
                                                    for id in ids]
                                                    )
    
    return image_points, obj_pts_without_distances, obj_pts_with_distances
    

def calibrate_camera(
    image_size: tuple[int, int],
    image_points: list[np.ndarray],
    obj_pts_without_distances: list[np.ndarray],
    obj_pts_with_distances: list[np.ndarray],
    with_distances: bool = False,
) -> tuple[float, np.ndarray, np.ndarray]:
    '''
    Calibrate the camera.

    :param image_size: tuple[int, int]
        The size of the image.
    
    :param image_points: list[np.ndarray]
        The image points.

    :param obj_pts_without_distances: list[np.ndarray]
        The object points without known distances.

    :param obj_pts_with_distances: list[np.ndarray]
        The object points with known distances.
    '''

    if with_distances:
        ret, mtx, dist, _, _ = cv2.calibrateCamera(
            obj_pts_with_distances, image_points, image_size, None, None
        )

        return ret, mtx, dist
    else:
        ret, mtx, dist, _, _ = cv2.calibrateCamera(
            obj_pts_without_distances, image_points, image_size, None, None
        )

        return ret, mtx, dist
        

def undistort_images(
    images: Dict[str, np.ndarray],
    camera_matrix: np.ndarray,
    dist_coeffs: np.ndarray
) -> Dict[str, np.ndarray]:
    '''
    Undistort the images given the camera matrix and the distortion coefficients.

    :param images: Dict[str, np.ndarray]
        A dictionary where the key is the file name and the value is the image.

    :param camera_matrix: np.ndarray
        The camera matrix.

    :param dist_coeffs: np.ndarray
        The distortion coefficients.

    :return: Dict[str, np.ndarray]
        A dictionary where the key is the file name 
        and the value is the undistorted image.
    '''

    undistorted_images = {}

    alpha = 0
    img_size = next(iter(images.values())).shape[:2][::-1]

    rect_camera_matrix = cv2.getOptimalNewCameraMatrix(
                                        camera_matrix, 
                                        dist_coeffs, 
                                        img_size, 
                                        alpha,
                                        img_size)[0]
    
    map1, map2 = cv2.initUndistortRectifyMap(
                                        camera_matrix, 
                                        dist_coeffs, 
                                        np.eye(3), 
                                        rect_camera_matrix, 
                                        img_size,
                                        cv2.CV_32FC1
                                        )
    

    for img_file, img in images.items():
        undistorted_img = cv2.remap(img, map1, map2, cv2.INTER_LINEAR)
        undistorted_images[img_file] = undistorted_img

    return undistorted_images


def projects_corners(
    image: np.ndarray,
    projection_matrix: np.ndarray,
) -> np.ndarray:
    '''
    Projects the corners of the image to the new coordinate system.

    :param image: np.ndarray
        The image to project.

    :param projection_matrix: np.ndarray
        The projection matrix.

    :return: np.ndarray
        The projected corners (min y, max y, min x, max x).
    '''
    
    height, width = image.shape[:2]
    corners = np.array([
        [0, 0, 1],
        [0, height, 1],
        [width, 0, 1],
        [width, height, 1]
    ], dtype=np.float32).T

    projected_corners = projection_matrix @ corners
    projected_corners /= projected_corners[2]

    # round down the min values and round up the max values 
    # to not lose any information.
    min_x = int(np.floor(projected_corners[0].min()))
    max_x = int(np.ceil(projected_corners[0].max()))
    min_y = int(np.floor(projected_corners[1].min()))
    max_y = int(np.ceil(projected_corners[1].max()))

    return min_y, max_y, min_x, max_x


def projective_transform(
    image: np.ndarray,
    projection_matrix: np.ndarray,
    new_origin: bool = False
) -> tuple[np.ndarray, np.ndarray]:
    '''
    Project the image to a new coordinate system.

    :param image: np.ndarray
        The image to project.

    :param projection_matrix: np.ndarray
        The projection matrix.

    :param new_origin: bool
        If True, the image will be projected to a new origin.

    :return: tuple[np.ndarray, np.ndarray]
        A tuple containing: the projected image, 
                            the projected corners (min y, max y, min x, max x).
    '''
    
    height, width = image.shape[:2]
    min_x, max_x, min_y, max_y = 0, width, 0, height

    if new_origin:
        min_y, max_y, min_x, max_x = projects_corners(image, projection_matrix)

        width = max_x - min_x
        height = max_y - min_y

        # To ensure that the image starts at (0, 0) we need to translate the image.
        translation_matrix = np.array([
            [1, 0, -min_x],
            [0, 1, -min_y],
            [0, 0, 1]
        ])
        projection_matrix = translation_matrix @ projection_matrix

    inverse_projection_matrix = np.linalg.inv(projection_matrix)

    projected_image = np.zeros((height, width, image.shape[2])).astype(np.uint8)

    dest_cords = np.indices((width, height)).reshape(2, -1)
    dest_cords = np.vstack((dest_cords, np.ones(dest_cords.shape[1])))

    src_cords = inverse_projection_matrix @ dest_cords
    src_cords /= src_cords[2]
    src_cords = src_cords[:2]
  

    # round to the nearest integer
    src_cords = np.rint(src_cords).astype(dtype=np.int32)   
    
    mask = (
        (0 <= src_cords[0]) & (src_cords[0] < image.shape[1]) &
        (0 <= src_cords[1]) & (src_cords[1] < image.shape[0])
    )

    dest_cords = dest_cords[:, mask]
    src_cords = src_cords[:, mask]

    projected_image[
        dest_cords[1].astype(np.int32),
        dest_cords[0].astype(np.int32)
    ] = image[
        src_cords[1],
        src_cords[0]
    ]

    return projected_image, (min_y, max_y, min_x, max_x)


def find_homography(
    source_points: np.ndarray,
    destination_points: np.ndarray
) -> np.ndarray:
    '''
    Find the homography matrix between the source and destination points, 
    by solving the linear system of equations.

    :param source_points: np.ndarray
        The source points.

    :param destination_points: np.ndarray
        The destination points.

    :return: np.ndarray
        The homography matrix.
    '''

    A = []

    for source_point, desination_point in zip(source_points, destination_points):

        x_s, y_s = source_point
        x_d, y_d = desination_point

        A.append([x_s, y_s, 1, 0, 0, 0, -x_d*x_s, -x_d*y_s, -x_d])
        A.append([0, 0, 0, x_s, y_s, 1, -y_d*x_s, -y_d*y_s, -y_d])

    A = np.array(A)

    _, _, V = np.linalg.svd(A)
    H = V[-1, :].reshape(3, 3)

    H /= H[2, 2]

    return H


def compute_cost_matrix(
    source_image: np.ndarray,
    destination_image: np.ndarray
) -> np.ndarray:
    '''
    Compute the cost matrix between the source and destination images, by
    computing the absolute difference between the images of each pixel.
    Then convert the difference to grayscale and return the squared difference.

    :param source_image: np.ndarray
        The source image.

    :param destination_image: np.ndarray
        The destination image.

    :return: np.ndarray
        The cost matrix.
    '''
    
    diff = np.abs(source_image - destination_image)
    grayscale_diff = cv2.cvtColor(diff, cv2.COLOR_BGR2GRAY)
    
    return np.square(grayscale_diff).astype(np.float32)


def find_optimal_seam(
    cost_matrix: np.ndarray
) -> np.ndarray:
    '''
    Find the optimal seam in the cost matrix. We use dynamic programming to find the
    seam with the minimum cost. We first initialize the first row of the dp matrix 
    with the cost matrix. Then we move down the rows and for each pixel we compute the
    cost of the pixel and add the minimum cost of the three pixels above it.
    We then backtrack the seam by starting from the pixel with the minimum cost 
    in the last row and moving up the rows by selecting the pixel with the 
    minimum cost from the three pixels above it. This path is the optimal seam.

    :param cost_matrix: np.ndarray
        The cost matrix.

    :return: np.ndarray
        The optimal seam in form of a list of coordinates (row, column).
    '''
    
    rows, cols = cost_matrix.shape
    dp = np.zeros_like(cost_matrix, dtype = np.float32)

    dp[0, :] = cost_matrix[0, :]

    for i in range(1, rows):
        for j in range(cols):
            dp[i, j] = cost_matrix[i, j] + min(
                                        dp[i-1, j-1] if j > 0 else np.inf,
                                        dp[i-1, j],
                                        dp[i-1, j+1] if j < cols - 1 else np.inf
                                        )
            
    seam = []
    j = np.argmin(dp[-1, :])
    seam.append((rows - 1, j))

    for i in range(rows - 2, -1, -1):
        prev_j = seam[-1][1]
        j = prev_j + np.argmin([
                            dp[i, prev_j - 1] if prev_j > 0 else np.inf,
                            dp[i, prev_j],
                            dp[i, prev_j + 1] if prev_j < cols - 1 else np.inf
                            ]) - 1
        seam.append((i, j))

    seam.reverse()

    return seam


def blend_along_seam(
    aligned_channel: np.ndarray,
    destination_channel: np.ndarray,
    seam: np.ndarray,
    source_on_left: bool = True
) -> np.ndarray:
    '''
    Blend the aligned and destination channels along the seam.
    On the left side of the seam we take the aligned channel 
    and on the right side we take the destination channel.
    We take the pixel only if it's value is greater than 0.

    :param aligned_channel: np.ndarray
        The aligned channel.

    :param destination_channel: np.ndarray
        The destination channel.

    :param seam: np.ndarrays
        The seam.
    
    :param source_on_left: bool
        If True, the source image is on the left side of the seam.
        If False, the source image is on the right side of the seam.

    :return: np.ndarray
        The blended channel.
    '''

    blended = np.zeros_like(aligned_channel)

    mask_aligend = (aligned_channel > 0)
    mask_destination = (destination_channel > 0)
    
    if source_on_left:
        for i, j in seam:
            blended[i, :j] = np.where(
                                    mask_aligend[i, :j], 
                                    aligned_channel[i, :j], 
                                    destination_channel[i, :j]
                                    )
            blended[i, j:] = np.where(
                                    mask_destination[i, j:], 
                                    destination_channel[i, j:], 
                                    aligned_channel[i, j:]
                                    )
    else:
        for i, j in seam:
            blended[i, j:] = np.where(
                                    mask_aligend[i, j:], 
                                    aligned_channel[i, j:], 
                                    destination_channel[i, j:]
                                    )
            blended[i, :j] = np.where(
                                    mask_destination[i, :j], 
                                    destination_channel[i, :j], 
                                    aligned_channel[i, :j])

    return blended


def stitch_images_seam(
    source_image: np.ndarray,
    destination_image: np.ndarray,
    homography_matrix: np.ndarray,
    off_dest: list[int] = [0, 0],
) -> tuple[np.ndarray, np.ndarray]:
    '''
    Stitch the source image to the destination image using the homography matrix.
    We blend the images along the optimal seam, which we find using dynamic programming.

    :param source_image: np.ndarray
        The source image.

    :param destination_image: np.ndarray
        The destination image.

    :param homography_matrix: np.ndarray
        The homography matrix.

    :param off: list[int]
        The offset for the destination image. It is used only when we stitch more
        than two images together one by one. 
        First when we stitch two images, the offset is [0, 0]. Then when we stitch
        another image to the stitched image, the offset is from the previous stitching's
        offset. It's important because we want to set the previous destination image
        in right place in the canvas.

    :return: tuple[np.ndarray, np.ndarray]
        A tuple containing: the stitched image and the offset for next stitching.
    '''

    # aligned image is the source image projected to the destination image coordinate system
    aligned_img, aligned_coords = projective_transform(
                                                    source_image, 
                                                    homography_matrix, 
                                                    new_origin=True
                                                    )
    
    canvas_y_min = min(-off_dest[0], aligned_coords[0])
    canvas_y_max = max(destination_image.shape[0]-off_dest[0], aligned_coords[1])
    canvas_x_min = min(-off_dest[1], aligned_coords[2])
    canvas_x_max = max(destination_image.shape[1]-off_dest[1], aligned_coords[3])

    canvas_height = canvas_y_max - canvas_y_min
    canvas_width = canvas_x_max - canvas_x_min

    canvas = np.zeros((canvas_height, canvas_width, 3), dtype=np.uint8)

    # calculate offset
    if aligned_coords[0] < 0 and aligned_coords[2] < 0:
        offset = [-aligned_coords[0], -aligned_coords[2]]
    elif aligned_coords[0] < 0 and aligned_coords[2] >= 0:
        offset = [-aligned_coords[0], 0]
    elif aligned_coords[0] >= 0 and aligned_coords[2] < 0:
        offset = [0, -aligned_coords[2]]
    else:
        offset = [0, 0]

    # add destination image to the canvas
    canvas[
        offset[0]-off_dest[0]:offset[0]+destination_image.shape[0]-off_dest[0],
        offset[1]-off_dest[1]:offset[1]+destination_image.shape[1]-off_dest[1]
    ] = destination_image

    # slice of the canvas where the aligned image will be placed
    dest_slice = np.copy(canvas[
        offset[0]+aligned_coords[0]:offset[0]+aligned_coords[1],
        offset[1]+aligned_coords[2]:offset[1]+aligned_coords[3]
    ])

    # find the overlap region (aligned image and destination image)
    overlap = (aligned_img > 0) & (dest_slice > 0)
    overlap_projection = np.any(overlap, axis=-1)
    indices = np.argwhere(overlap_projection)

    if indices.size > 0:
        min_y, min_x = indices.min(axis=0)
        max_y, max_x = indices.max(axis=0)

    overlap_region = (
        slice(min_y, max_y),
        slice(min_x, max_x)
    )

    dest_img_overlapped = dest_slice[overlap_region]
    aligned_img_overlapped = aligned_img[overlap_region]

    cost = compute_cost_matrix(aligned_img_overlapped, dest_img_overlapped)
    seam = find_optimal_seam(cost)

    # add aligned image to the canvas
    mask = (aligned_img > 0).astype(np.uint8)
    canvas[
        offset[0]+aligned_coords[0]:offset[0]+aligned_coords[1],
        offset[1]+aligned_coords[2]:offset[1]+aligned_coords[3]
    ] = np.where(mask, aligned_img, dest_slice)

    for c in range(canvas.shape[2]):

        blended_region = blend_along_seam(
                                        aligned_img_overlapped[:, :, c],
                                        dest_img_overlapped[:, :, c],
                                        seam,
                                        source_on_left=True if offset[1] > 0 else False
                                        )
        
        canvas[
            offset[0]+aligned_coords[0]+min_y:offset[0]+aligned_coords[0]+max_y,
            offset[1]+aligned_coords[2]+min_x:offset[1]+aligned_coords[2]+max_x,
            c
        ] = blended_region

    return canvas, offset


##################################################
######  Functions for tasks with SuperGlue  ######
##################################################
def unpack_data(
    path: str
) -> tuple[np.ndarray, np.ndarray]:
    '''
    Unpack the data from the npz file. 
    Assuming that keypoints0 are the source points,
    and keypoints1 are the destination points.

    :param path: str
        The path to the npz file.

    :return: tuple[np.ndarray, np.ndarray]
        A tuple containing the coreesponding source and destination points
        from the npz file.
    '''
    
    npz = np.load(path)

    keypoints0 = npz['keypoints0']
    keypoints1 = npz['keypoints1']
    matches = npz['matches']

    matched_indices = matches > -1
    destination_points = keypoints0[matched_indices]
    source_points = keypoints1[matches[matched_indices]]

    return source_points, destination_points

def stitch_many_images(
    images_list: list[np.ndarray],
    source_points_list: list[np.ndarray],
    destination_points_list: list[np.ndarray],
) -> np.ndarray:
    '''
    Stitch many images together. Assuming that the images are ordered in the list
    and the first image is the destination image. After stitching first image 
    with the second image, the result is used as the destination image for the 
    next image in the list.

    :param images_list: list[np.ndarray]
        A list of images to stitch.

    :param source_points_list: list[np.ndarray]
        A list of source points for each image.
    
    :param destination_points_list: list[np.ndarray]
        A list of destination points for each image.

    :return: np.ndarray
        The stitched images.
    '''
    
    canvas = images_list[0]
    homography_matrix = np.eye(3)
    offset_dest = [0, 0]

    for source_points, destination_points, image in zip(source_points_list, 
                                                        destination_points_list, 
                                                        images_list[1:]
                                                        ):
        dest_points = np.hstack((destination_points, 
                                 np.ones((destination_points.shape[0], 1))))
        dest_points = (homography_matrix @ dest_points.T).T
        dest_points /= dest_points[:, 2].reshape(-1, 1)
        dest_points = dest_points[:, :2]

        dest_points = np.rint(dest_points).astype(np.int32)

        homography_matrix = find_homography(source_points, dest_points)
        canvas, offset_dest = stitch_images_seam(image, canvas, 
                                                 homography_matrix, offset_dest)

    return canvas


#################################################
################   TASKS    #####################
#################################################
def task1(
    dir_name_calibrate: str,
    dir_name_undistort: str,
    print_results: bool = True
) -> Dict[str, np.ndarray]:
    
    images_calibration = read_images(dir_name=dir_name_calibrate)

    img_size = next(iter(images_calibration.values())).shape[:2][::-1]

    image_points, obj_pts_no_dist, obj_pts_dist = object_and_image_points(
                                                        images_calibration
                                                        )
    
    ret_no_dist, mtx_no_dist, dist_coeffs_no_dist = calibrate_camera(
                                                        img_size,
                                                        image_points,
                                                        obj_pts_no_dist,
                                                        obj_pts_dist,
                                                        with_distances=False
                                                        )

    ret_dist, mtx_dist, dist_coeffs_dist = calibrate_camera(
                                                    img_size,
                                                    image_points,
                                                    obj_pts_no_dist,
                                                    obj_pts_dist,
                                                    with_distances=True
                                                    )
    
    if print_results:
        print(f""" 
Camera calibration results:
Without known distances:
Camera matrix:
{mtx_no_dist}
Distortion coefficients:
{dist_coeffs_no_dist}
Reprojection error: {ret_no_dist}

With known distances:
Camera matrix:
{mtx_dist}
Distortion coefficients:
{dist_coeffs_dist}
Reprojection error: {ret_dist}
            """)    

    images_undistort = read_images(dir_name=dir_name_undistort)
    undistorted_images = undistort_images(
                                    images_undistort,
                                    mtx_no_dist,
                                    dist_coeffs_no_dist
                                    )
    
    return undistorted_images


def task2(
    image: np.ndarray,
    projection_matrix: np.ndarray,
    show_images: bool = True
) -> None:

    projected_image, _ = projective_transform(image, projection_matrix)
        
    concatenated_images = np.hstack((image, projected_image))

    if show_images:
        show_all_images([concatenated_images])


def task3(
    number_of_tests: int = 10,
    number_of_points: list = [4]
) -> None:

    for _ in range(number_of_tests):
        H_true = np.random.rand(3, 3)
        H_true /= H_true[2, 2]

        for n in number_of_points:

            src_points = np.random.rand(n, 2) * 100
            src_points_homogenous = np.hstack((src_points, np.ones((n, 1))))

            dest_points = H_true @ src_points_homogenous.T
            dest_points /= dest_points[2, :]
            dest_points = dest_points[:2, :].T

            H_estimated = find_homography(src_points, dest_points)

            assert np.allclose(H_true, H_estimated, atol=1e-4), \
                f"Failed. \
                True homography:\n{H_true}\nEstimated homography:\n{H_estimated}"
            
                   
def task4():
    src_pts = np.array([ # img2
        [420, 337],
        [417, 296],
        [460, 296],
        [461, 336],
        [1103, 344],
        [977, 496],
        [1030, 366],
        [570, 429]
    ])

    dest_pts = np.array([  # img1
        [295, 329],
        [292, 285],
        [338, 285],
        [340, 328],
        [980, 339],
        [863, 486],
        [913, 360],
        [457, 424]
    ])

    homography_matrix = find_homography(src_pts, dest_pts)

    return homography_matrix


def task5(
    images: Dict[str, np.ndarray],
    homography_matrix: np.ndarray,
    show_images: bool = True
) -> None:
    
    destination_img = images['img1.png']
    source_img = images['img2.png']

    canvas, _ = stitch_images_seam(source_img, destination_img, homography_matrix)
    
    if show_images:
        show_all_images([canvas])


def task6(
    images: Dict[str, np.ndarray],
    show_images: bool = True
) -> None:
    
    path = 'superglue/task6/img2_img3_matches.npz'

    source_points, destination_points = unpack_data(path)

    homography_matrix = find_homography(source_points, destination_points)

    canvas, _ = stitch_images_seam(
                                images['img3.png'], 
                                images['img2.png'], 
                                homography_matrix
                                )

    if show_images:
        show_all_images([canvas])


def task7(
    images: Dict[str, np.ndarray],
    show_images: bool = True
) -> None:
    
    files= ['superglue/task7/img9_img8_matches.npz', 
            'superglue/task7/img8_img7_matches.npz', 
            'superglue/task7/img7_img6_matches.npz', 
            'superglue/task7/img6_img5_matches.npz']
    images_pairs = ['img9.png', 'img8.png', 'img7.png', 'img6.png', 'img5.png']

    source_points_list = []
    destination_points_list = []

    for file in files:
        source_points, destination_points = unpack_data(file)
        source_points_list.append(source_points)
        destination_points_list.append(destination_points)

    canvas = stitch_many_images(
                [images[img] for img in images_pairs],
                source_points_list,
                destination_points_list
            )
    
    if show_images:
        show_all_images([canvas])
        

if __name__ == '__main__':
    calibrated_images = task1(
                        dir_name_calibrate='calibration',
                        dir_name_undistort='stitching',
                        print_results=True
                        )
    
    projection_matrix = task4()

    task2(
        calibrated_images['img1.png'], 
        projection_matrix, 
        show_images=True
        )
    
    task3(
        number_of_tests=10,
        number_of_points=[4, 8, 16]
        )

    task5(
        calibrated_images,
        projection_matrix,
        show_images=True
        )

    task6(
        calibrated_images,
        show_images=True
        )

    task7(
        calibrated_images,
        show_images=True
        )