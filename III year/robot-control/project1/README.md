# Calibration and Stitching

This project demonstrates the use of Python's cv2 and numpy libraries to calibrate camera, 
undistort images and stitch them using dynamic programming to find optimal seam.

## Prerequisites

- Python version: **Python 3.11.9**
- Required dependencies are listed in `requirements.txt`.

---

## Setup Instructions

1. **Create and activate a virtual environment:**

    ```shell
    python -m venv venv
    source venv/bin/activate
    ```

2. **Install the required dependencies:**

    ```shell
    pip install -r requirements.txt
    ```

---

## How to run
Run the program with all tasks. You can change in main if you want to see the results from task 1 and output images from task 2, 5, 6, 7.

Execute the script:
```shell
python stitch.py
```


---

## Task Descriptions

### Task 1: Calibration and Undistortion

### Camera calibration results:
- **Without known distances:**

    Camera matrix:
    ```shell
    [[922.73339088   0.         669.48165737]
    [  0.         923.86650579 355.61702398]
    [  0.           0.           1.        ]]
    ```

    Distortion coefficients:
    ```shell
    [[ 0.15463712 -0.46165988 -0.00223023  0.00202738  0.38206162]]
    ```
    Reprojection error:
    ```shell
    0.27968494207980144
    ```

- **With known distances:**
   
    Camera matrix:
    ```shell
    [[922.73339099   0.         669.48165764]
    [  0.         923.86650593 355.61702397]
    [  0.           0.           1.        ]]
    ```
    Distortion coefficients:
    ```shell
    [[ 0.15463711 -0.46165988 -0.00223023  0.00202738  0.38206161]]
    ```
    Reprojection error: 
    ```shell
    0.27968494207980144
    ```

### Discussion about the results:
The results with known and unknown distances are almost identical. The only noticeable differences appear in the camera matrix and distortion coefficients, with a minimal error difference (~1e7). This similarity arises because the calibration process primarily relies on the proportional arrangement of the object points, rather than their absolute distances. Even without precise physical measurements, the algorithm minimizes the reprojection error effectively, especially with 28 images providing diverse perspectives. This demonstrates that a sufficient number of calibration images can compensate for missing real-world distances between markers, ensuring robust parameter estimation.

### Undistotion
Using calibrated camera we then undistort images (from stitching directory).

---

### Task 5: Stitching Two Images

Using homography matrix from Task 4 we stitch two images.

- **Source image:** `img2.png`  
- **Destination image:** `img1.png`  


---

### Task 6 & 7: Stitching with points from SuperGlue

#### SuperGlue Command for Matching
To use SuperGlue for matching image pairs, run:

```shell
python match_pairs.py --viz --resize 1280 720 --input_pairs 'file.txt' --output_dir 'output/' --input_dir 'input/'
```

#### Task 6: Match Two Images

- **Source image:** `img3.png`  
- **Destination image:** `img2.png`  

Path to the npz file:

```shell
 path = 'superglue/task6/'
```

#### Task 7: Multiple Image Stiching

Perform multiple stitching to combine many images (5).

1. **First stitching (Output: `image 9|8`):**  
   - **Source image:** `img8.png`  
   - **Destination image:** `img9.png`

2. **Second stitching (Output: `image 9|8|7`):**  
   - **Source image:** `img7.png`  
   - **Destination image:** `image 9|8`

3. **Third stitching (Output: `image 9|8|7|6`):**  
   - **Source image:** `img6.png`  
   - **Destination image:** `image 9|8|7`

4. **Fourth stitching (Output: `image 9|8|7|6|5`):**  
   - **Source image:** `img5.png`  
   - **Destination image:** `image 9|8|7|6`

- **Final Output:** `image 9|8|7|6|5`

Path to the npz files:

```shell
 path = 'superglue/task7/'
```