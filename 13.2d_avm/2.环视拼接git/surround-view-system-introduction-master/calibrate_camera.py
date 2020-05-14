#!/usr/bin/python3
"""
~~~~~~~~~~~~~~~~~~
Camera calibration
~~~~~~~~~~~~~~~~~~

Usage:
    python calib.py \
        -i /dev/video0 \
        -grid 9x6 \
        -out fisheye.yaml \
        -framestep 20 \
        -resolution 640x480
        --fisheye
"""
import argparse
import os
import numpy as np
import yaml
import cv2


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", default=0, help="input video file or camera device")
    parser.add_argument("-grid", "--grid", default="20x20", help="size of the grid (rows x cols)")
    parser.add_argument("-framestep", type=int, default=20, help="use every nth frame in the video")
    parser.add_argument("-o", "--output", default="./yaml",
                        help="path to output yaml file")
    parser.add_argument("-resolution", "--resolution", default="640x480",
                        help="resolution of the camera")
    parser.add_argument("-fisheye", "--fisheye", action="store_true",
                        help="set ture if this is a fisheye camera")

    args = parser.parse_args()

    if not os.path.exists(args.output):
        os.mkdir(args.output)

    try:
        source = cv2.VideoCapture(int(args.input))
    except:
        source = cv2.VideoCapture(args.input)

    W, H = [int(x) for x in args.resolution.split("x")]
    source.set(3, W)
    source.set(4, H)

    grid_size = tuple(int(x) for x in args.grid.split("x"))
    grid_points = np.zeros((np.prod(grid_size), 3), np.float32)
    grid_points[:, :2] = np.indices(grid_size).T.reshape(-1, 2)

    objpoints = []  # 3d point in real world space
    imgpoints = []  # 2d points in image plane

    quit = False
    do_calib = False
    i = -1
    while True:
        i += 1
        retcode, img = source.read()
        if not retcode:
            raise ValueError("cannot read frame from video")
        if i % args.framestep != 0:
            continue

        print("searching for chessboard corners in frame " + str(i) + "...")
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        found, corners = cv2.findChessboardCorners(
            gray,
            grid_size,
            cv2.CALIB_CB_ADAPTIVE_THRESH +
            cv2.CALIB_CB_NORMALIZE_IMAGE +
            cv2.CALIB_CB_FILTER_QUADS
        )
        if found:
            term = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_COUNT, 30, 0.01)
            cv2.cornerSubPix(gray, corners, (5, 5), (-1, -1), term)
            print("OK")
            imgpoints.append(corners.reshape(1, -1, 2))
            objpoints.append(grid_points.reshape(1, -1, 3))
            cv2.drawChessboardCorners(img, grid_size, corners, found)
        text1 = "press c to calibrate"
        text2 = "press q to quit"
        text3 = "device: {}".format(args.input)
        fontscale = 0.6
        cv2.putText(img, text1, (20, 70), cv2.FONT_HERSHEY_SIMPLEX, fontscale, (255, 200, 0), 2)
        cv2.putText(img, text2, (20, 110), cv2.FONT_HERSHEY_SIMPLEX, fontscale, (255, 200, 0), 2)
        cv2.putText(img, text3, (20, 30), cv2.FONT_HERSHEY_SIMPLEX, fontscale, (255, 200, 0), 2)
        cv2.imshow("corners", img)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("c"):
            do_calib = True
            break

        elif key == ord("q"):
            quit = True
            break

    if quit:
        source.release()
        cv2.destroyAllWindows()

    if do_calib:
        print("\nPerforming calibration...\n")
        N_OK = len(objpoints)
        if N_OK < 12:
            print("Less than 12 corners detected, calibration failed")
            return

        K = np.zeros((3, 3))
        D = np.zeros((4, 1))
        rvecs = [np.zeros((1, 1, 3), dtype=np.float64) for _ in range(N_OK)]
        tvecs = [np.zeros((1, 1, 3), dtype=np.float64) for _ in range(N_OK)]
        calibration_flags = (cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC +
                             cv2.fisheye.CALIB_CHECK_COND +
                             cv2.fisheye.CALIB_FIX_SKEW)

        # 求出内参矩阵和畸变系数
        if args.fisheye:
            ret, mtx, dist, rvecs, tvecs = cv2.fisheye.calibrate(
                objpoints,
                imgpoints,
                (W, H),
                K,
                D,
                rvecs,
                tvecs,
                calibration_flags,
                (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 1e-6)
            )
        else:
            ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(
                objpoints,
                imgpoints,
                (W, H),
                None,
                None)
        if ret:
            print(ret)
            data = {"dim": np.array([W, H]).tolist(), "K": K.tolist(), "D": D.tolist()}
            fname = os.path.join(args.output, "camera" + str(args.input) + ".yaml")
            print(fname)
            with open(fname, "w") as f:
                yaml.safe_dump(data, f)
            print("succesfully saved camera data")

            cv2.putText(img, "Success!", (220 , 240), cv2.FONT_HERSHEY_COMPLEX, 2, (0, 0, 255), 2)
        else:
            cv2.putText(img, "Failed!", (220, 240), cv2.FONT_HERSHEY_COMPLEX, 2, (0, 0, 255), 2)

        cv2.imshow("corners", img)
        cv2.waitKey(0)


if __name__ == "__main__":
    main()
