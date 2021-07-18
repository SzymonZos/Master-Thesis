import os
import subprocess
from pywt import dwt2, dwt
import numpy as np
import cv2


orig_size = 0


def memoryless_entropy(img):
    hist = cv2.calcHist([img], [0], None, [256], [0, 256])
    ent_res = 0
    img_size = img.shape[0] * img.shape[1]
    for val in hist:
        if val:
            tmp = val[0] / img_size
            ent_res += tmp * np.log2(1. / tmp)
    return (img_size / orig_size) * ent_res


def dwt_cols(image, wavelet):
    ll, hh = dwt(image.T, wavelet)
    return ll.T, hh.T


def normalize(img):
    return cv2.normalize(img, dst=None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)


def median_diff(img):
    img = normalize(img)
    median = cv2.medianBlur(img, 3)
    return cv2.absdiff(img, median)


def entropy(img):
    img = normalize(img)
    return memoryless_entropy(img)


def no_dwt_estimate(img, cb_diff):
    diff = cb_diff(img)
    return 2 * memoryless_entropy(diff)


def dwt_rows_estimate(img, wavelet, cb_diff):
    ll, hh = dwt(img, wavelet)
    diff = cb_diff(ll)
    return entropy(hh) + 2 * memoryless_entropy(diff)


def dwt_cols_estimate(img, wavelet, cb_diff):
    ll, hh = dwt_cols(img, wavelet)
    diff = cb_diff(ll)
    return entropy(hh) + 2 * memoryless_entropy(diff)


def dwt_2d_estimate(img, wavelet, cb_diff):
    ll, (lh, hl, hh) = dwt2(img, wavelet)
    diff = cb_diff(ll)
    return entropy(lh) + entropy(hl) + entropy(hh) + 2 * memoryless_entropy(diff)


def test_median(img):
    original = cv2.imread(img, cv2.IMREAD_GRAYSCALE)
    global orig_size
    orig_size = original.shape[0] * original.shape[1]
    wavelet = 'bior2.2'
    res = [no_dwt_estimate(original, median_diff),
           dwt_rows_estimate(original, wavelet, median_diff),
           dwt_cols_estimate(original, wavelet, median_diff),
           dwt_2d_estimate(original, wavelet, median_diff)]
    print(np.argmin(res))


def shift_diff(img):
    img = normalize(img)
    num_rows, num_cols = img.shape[:2]
    trans_mat = np.float32([[1, 0, 1], [0, 1, 0]])
    img_translation = cv2.warpAffine(img, trans_mat, (num_cols, num_rows))
    return cv2.absdiff(img, img_translation)


def test_shift(img):
    original = cv2.imread(img, cv2.IMREAD_GRAYSCALE)
    wavelet = 'bior2.2'
    res = [no_dwt_estimate(original, shift_diff),
           dwt_rows_estimate(original, wavelet, shift_diff),
           dwt_cols_estimate(original, wavelet, shift_diff),
           dwt_2d_estimate(original, wavelet, shift_diff)]
    print(np.argmin(res))


def highpass(img):
    _, hh = dwt(img, 'bior2.2')
    hh = normalize(hh)
    return hh


def test_highpass(img):
    original = cv2.imread(img, cv2.IMREAD_GRAYSCALE)
    wavelet = 'bior2.2'
    res = [no_dwt_estimate(original, highpass),
           dwt_rows_estimate(original, wavelet, highpass),
           dwt_cols_estimate(original, wavelet, highpass),
           dwt_2d_estimate(original, wavelet, highpass)]
    print(np.argmin(res))


if __name__ == "__main__":
    PYTHON_APP = True
    for file in os.scandir('science_png'):
        print(file.path)
        if PYTHON_APP:
            test_median(file.path)
            test_shift(file.path)
            test_highpass(file.path)
        else:
            res = subprocess.run(["./bin/jpeg2000.exe",
                              "--opencv-queue-entropy",
                              f"{file.path}"], stdout=subprocess.PIPE)
            print(res.stdout.decode('utf-8'))
        print("")
