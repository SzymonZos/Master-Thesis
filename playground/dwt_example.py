import os
import shutil
from itertools import product, takewhile

import matplotlib.pyplot as plt
import numpy as np
import cv2
from pywt import dwt, dwt2, Wavelet
from pywt.data import camera


def plot_subbands(ll, lh, hl, hh, titles):
    fig = plt.figure(figsize=(12, 3))
    for i, a in enumerate([ll, lh, hl, hh]):
        ax = fig.add_subplot(1, 4, i + 1)
        ax.imshow(a, interpolation="nearest", cmap=plt.cm.gray)
        ax.set_title(titles[i], fontsize=10)
        ax.set_xticks([])
        ax.set_yticks([])
    fig.tight_layout()
    plt.show()


def _dwt2(image):
    ll, (_, _, _) = dwt2(image, 'bior2.2')
    return ll


def _dwt_rows(image):
    ll, _ = dwt(image, 'bior2.2')
    return ll


def _dwt_cols(image):
    ll, _ = dwt(image.T, 'bior2.2')
    return ll.T


def _no_dwt(image):
    return image


def my_dwt(image, depth):
    cbs = [_dwt2, _dwt_cols, _dwt_rows, _no_dwt]
    comps = {tuple(takewhile(lambda x: x != _no_dwt, comp))
             for comp in product(cbs, repeat=depth)}
    results = []
    for comp in comps:
        ll = image
        for cb in comp:
            ll = cb(ll)
        results.append(("_".join(cb.__name__ for cb in comp), ll))
    return results


def example():
    print(Wavelet('bior1.3'))
    print(Wavelet('bior3.7'))
    print(Wavelet('db1'))

    original = camera()
    titles = ['Approximation', ' Horizontal detail',
              'Vertical detail', 'Diagonal detail']
    LL, (LH, HL, HH) = dwt2(original, 'bior1.3')
    ll, (lh, hl, hh) = dwt2(LL, 'bior1.3')

    plot_subbands(LL, LH, HL, HH, titles)
    plot_subbands(ll, lh, hl, hh, titles)


def save_results(results):
    try:
        os.mkdir("temp")
    except FileExistsError:
        shutil.rmtree("temp")
        os.mkdir("temp")

    for i, (title, res) in enumerate(results):
        title = f"{i}_{title}"
        plt.title(title, fontsize=10)
        plt.tight_layout()
        plt.imsave(f"temp/{title}.png", res, cmap=plt.cm.gray)


lut_db1 = [
    7.071067811865475244008443621048490392848359376884740365883398e-01,
    7.071067811865475244008443621048490392848359376884740365883398e-01
]

lut_db2 = [
    4.829629131445341433748715998644486838169524195042022752011715e-01,
    8.365163037378079055752937809168732034593703883484392934953414e-01,
    2.241438680420133810259727622404003554678835181842717613871683e-01,
    -1.294095225512603811744494188120241641745344506599652569070016e-01
]

lut_db3 = [
    3.326705529500826159985115891390056300129233992450683597084705e-01,
    8.068915093110925764944936040887134905192973949948236181650920e-01,
    4.598775021184915700951519421476167208081101774314923066433867e-01,
    -1.350110200102545886963899066993744805622198452237811919756862e-01,
    -8.544127388202666169281916918177331153619763898808662976351748e-02,
    3.522629188570953660274066471551002932775838791743161039893406e-02
]

lut_bior2_2 = [
    -0.1767766952966368811002110905262,
    0.3535533905932737622004221810524,
    1.0606601717798212866012665431573,
    0.3535533905932737622004221810524,
    -0.1767766952966368811002110905262,
    0.0
]

lut_bior2_2_h = [
    0., 0.,
    0.3535533905932737622004221810524,
    -0.7071067811865475244008443621048,
    0.3535533905932737622004221810524,
    0.
]

lut_leg_l = [
    -0.125, 0.25, 0.75, 0.25, -0.125, 0
]

lut_leg_h = [
    0, 0, 0.5, -1., 0.5, 0
]


def get_lut(wavelet: str):
    luts = {
        "db1": lut_db1,
        "haar": lut_db1,
        "db2": lut_db2,
        "db3": lut_db3,
        "bior2.2": lut_bior2_2,
        "leg_l": lut_leg_l,
        "leg_h": lut_leg_h,
        "bior2.2_h": lut_bior2_2_h
    }
    return luts.get(wavelet, 0)


def dwt_re(arr_input: np.array, wavelet: str):
    lut = list(reversed(get_lut(wavelet)))
    symmetric_len = len(lut) - 2
    if symmetric_len:
        arr_input = np.concatenate(
            (arr_input[symmetric_len - 1::-1], arr_input,
             arr_input[:arr_input.shape[0] - symmetric_len - 1:-1]), axis=None)
    print(f"Dupa: {arr_input}")
    output = np.convolve(arr_input, lut, mode='valid')
    return output[::2]


def main():
    original = camera()
    results = my_dwt(original, 3)
    save_results(results)


def dwt_testing():
    dummy = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    print(dwt_re(dummy, 'haar'))
    print(dwt(dummy, 'haar')[0])
    print(dwt_re(dummy, 'db2'))
    print(dwt(dummy, 'db2')[0])
    print(_dwt_rows(dummy))
    print(dwt_re(dummy, 'bior2.2'))

    dummy_2d = np.array([[1, 2, 3, 4, 5], [5, 6, 7, 8, 9]])
    print(_dwt2(dummy_2d))
    print(_dwt_rows(dummy_2d))
    print(_dwt_cols(dummy_2d))
    print(dwt_re(np.array([1, 5]), 'bior2.2'))
    print(dwt(np.array([1, 5]), 'bior2.2')[0])


def opencv_dwt():
    original = cv2.imread('../img/lena.png', cv2.IMREAD_GRAYSCALE)
    cv2.imshow('Original lena.png', original)
    LL, (LH, HL, HH) = dwt2(original, 'bior1.3')

    def normalize_img(pixel):
        return 255 * (pixel - np.min(LL)) // (np.max(LL) - np.min(LL))

    norm_ll = np.array([*map(normalize_img, LL), ]).astype(np.uint8)
    cv2.imshow('opencv normalized ll from dwt', norm_ll)
    titles = ['Approximation', ' Horizontal detail',
              'Vertical detail', 'Diagonal detail']
    plot_subbands(LL, LH, HL, HH, titles)


def memoryless_entropy(img):
    hist = cv2.calcHist([img], [0], None, [256], [0, 256])
    entropy = 0
    img_size = img.shape[0] * img.shape[1]
    for val in hist:
        if val:
            tmp = val[0] / img_size
            entropy += tmp * np.log2(1. / tmp)
    return entropy / img_size


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
    return memoryless_entropy(diff)


def dwt_rows_estimate(img, wavelet, cb_diff):
    ll, hh = dwt(img, wavelet)
    diff = cb_diff(ll)
    return entropy(hh) + memoryless_entropy(diff)


def dwt_cols_estimate(img, wavelet, cb_diff):
    ll, hh = dwt_cols(img, wavelet)
    diff = cb_diff(ll)
    return entropy(hh) + memoryless_entropy(diff)


def dwt_2d_estimate(img, wavelet, cb_diff):
    ll, (lh, hl, hh) = dwt2(img, wavelet)
    diff = cb_diff(ll)
    return entropy(lh) + entropy(hl) + entropy(hh) + memoryless_entropy(diff)


def test_median():
    original = cv2.imread('../img/lena.png', cv2.IMREAD_GRAYSCALE)
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


def test_shift():
    original = cv2.imread('../img/lena.png', cv2.IMREAD_GRAYSCALE)
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


def test_highpass():
    original = cv2.imread('../img/lena.png', cv2.IMREAD_GRAYSCALE)
    wavelet = 'bior2.2'
    res = [no_dwt_estimate(original, highpass),
           dwt_rows_estimate(original, wavelet, highpass),
           dwt_cols_estimate(original, wavelet, highpass),
           dwt_2d_estimate(original, wavelet, highpass)]
    print(np.argmin(res))


if __name__ == "__main__":
    # main()
    # dwt_testing()
    # opencv_dwt()
    # img = np.array([55, 234, 70, 21, 88, 37])
    # print(dwt_re(img, 'bior2.2'))
    # print(dwt_re(img, 'bior2.2_h'))
    # l, h = dwt(img, 'bior2.2')
    # print(f"{l}\n{h}")
    # print(f"{l / np.sqrt(2)}\n{h / np.sqrt(2)}")
    # debug_img = np.array([21, 70, 234, 55, 55, 234, 70, 21, 88, 37, 37, 88, 21, 70])
    # print(np.convolve(debug_img, [*reversed(lut_leg_h), ], mode='full'))
    test_median()
    test_shift()
    test_highpass()
