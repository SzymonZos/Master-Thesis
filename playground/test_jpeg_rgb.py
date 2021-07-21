import os
import subprocess
from pywt import dwt2, dwt
import numpy as np
import cv2
from skimage.measure import shannon_entropy


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


def shan_entropy(img):
    img_size = img.shape[0] * img.shape[1]
    return (img_size / orig_size) * shannon_entropy(img)


def dwt_cols(image, wavelet):
    ll, hh = dwt(image, wavelet, axis=1)
    return ll, hh


def normalize(img):
    return cv2.normalize(img, dst=None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)


def median_diff(img):
    img = normalize(img)
    median = cv2.medianBlur(img, 3)
    return cv2.absdiff(img, median)


def entropy(img):
    img = normalize(img)
    return shan_entropy(img)


def no_dwt_estimate(img, cb_diff):
    diff = cb_diff(img)
    return shan_entropy(diff)


def dwt_rows_estimate(img, wavelet, cb_diff):
    ll, hh = dwt(img, wavelet, axis=0)
    diff = cb_diff(ll)
    return entropy(hh) + shan_entropy(diff)


def dwt_cols_estimate(img, wavelet, cb_diff):
    ll, hh = dwt_cols(img, wavelet)
    diff = cb_diff(ll)
    return entropy(hh) + shan_entropy(diff)


def dwt_2d_estimate(img, wavelet, cb_diff):
    ll, (lh, hl, hh) = dwt2(img, wavelet, axes=(0, 1))
    diff = cb_diff(ll)
    return entropy(lh) + entropy(hl) + entropy(hh) + shan_entropy(diff)


def test_median(img):
    original = cv2.imread(img, cv2.IMREAD_COLOR)
    global orig_size
    orig_size = original.shape[0] * original.shape[1]
    wavelet = 'bior2.2'
    res = [[] for _ in range(4)]
    for i in range(3):
        res[0].append(no_dwt_estimate(original[:, :, i], median_diff))
        res[1].append(dwt_rows_estimate(original[:, :, i], wavelet, median_diff))
        res[2].append(dwt_cols_estimate(original[:, :, i], wavelet, median_diff))
        res[3].append(dwt_2d_estimate(original[:, :, i], wavelet, median_diff))
    print(np.argmin(np.sum(res, 1)))


def shift_diff(img):
    img = normalize(img)
    num_rows, num_cols = img.shape[:2]
    trans_mat = np.float32([[1, 0, 1], [0, 1, 0]])
    img_translation = cv2.warpAffine(img, trans_mat, (num_cols, num_rows))
    return cv2.absdiff(img, img_translation)


def test_shift(img):
    original = cv2.imread(img, cv2.IMREAD_COLOR)
    wavelet = 'bior2.2'
    res = [[] for _ in range(4)]
    for i in range(3):
        res[0].append(no_dwt_estimate(original[:, :, i], shift_diff))
        res[1].append(dwt_rows_estimate(original[:, :, i], wavelet, shift_diff))
        res[2].append(dwt_cols_estimate(original[:, :, i], wavelet, shift_diff))
        res[3].append(dwt_2d_estimate(original[:, :, i], wavelet, shift_diff))
    print(np.argmin(np.sum(res, 1)))


def highpass(img):
    _, hh = dwt(img, 'bior2.2', axis=0)
    hh = normalize(hh)
    return hh


def test_highpass(img):
    original = cv2.imread(img, cv2.IMREAD_COLOR)
    wavelet = 'bior2.2'
    res = [[] for _ in range(4)]
    for i in range(3):
        res[0].append(no_dwt_estimate(original[:, :, i], highpass))
        res[1].append(dwt_rows_estimate(original[:, :, i], wavelet, highpass))
        res[2].append(dwt_cols_estimate(original[:, :, i], wavelet, highpass))
        res[3].append(dwt_2d_estimate(original[:, :, i], wavelet, highpass))
    print(np.argmin(np.sum(res, 1)))


def set_orig_size(img):
    global orig_size
    orig_size = img.shape[0] * img.shape[1]


def test_part(img, wavelet):
    res = [[] for _ in range(4)]
    for i in range(3):
        # res[0].append(no_dwt_estimate(original[:, :, i], shift_diff))
        res[0].append(float("inf"))
        res[1].append(dwt_rows_estimate(img[:, :, i], wavelet, shift_diff))
        res[2].append(dwt_cols_estimate(img[:, :, i], wavelet, shift_diff))
        res[3].append(dwt_2d_estimate(img[:, :, i], wavelet, shift_diff))
    sum_res = np.sum(res, 1)
    arg_min_res = np.argmin(sum_res)
    return arg_min_res, sum_res[arg_min_res]


def no_dwt(img):
    return img


def dwt_rows_low(img, wavelet):
    ll, _ = dwt(img, wavelet, axis=0)
    return ll


def dwt_cols_low(img, wavelet):
    ll, _ = dwt(img.T, wavelet, axis=1)
    return ll.T


def dwt_2d_low(img, wavelet):
    ll, (_, _, _) = dwt2(img, wavelet, axes=(0, 1))
    return ll


lut_dwt = [no_dwt, dwt_rows_low, dwt_cols_low, dwt_2d_low]


def test_full(img, wavelet):
    img_test = img
    heuristics = []
    for i in range(5):
        res, min_val = test_part(img_test, wavelet)
        heuristics.append(res)
        if res == 0:
            break
        img_test = lut_dwt[res](img_test, wavelet)
    return heuristics, min_val


if __name__ == "__main__":
    NEW_MODE = True
    for file in os.scandir('../img/images_ppm'):
        print(file.path)
        if NEW_MODE:
            original = cv2.imread(file.path, cv2.IMREAD_COLOR)
            set_orig_size(original)
            results = dict()
            for wavelet in ["bior2.2", "haar", "bior2.6"]:
                results[wavelet] = test_full(original, wavelet)
            min_result = min(results.items(), key=lambda k: k[1][1])
            print(min_result)
            print(results)
        else:
            test_median(file.path)
            test_shift(file.path)
            test_highpass(file.path)
        print("")
