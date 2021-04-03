import os
import shutil
from itertools import product, takewhile

import matplotlib.pyplot as plt
import numpy as np
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
    ll, (_, _, _) = dwt2(image, 'bior1.3')
    return ll


def _dwt_cols(image):
    ll, _ = dwt(image, 'db3')
    return ll


def _dwt_rows(image):
    ll, _ = dwt(image.T, 'db3')
    return ll.T


def _no_dwt(image):
    return image


def my_dwt(image, depth):
    cbs = [_dwt2, _dwt_cols, _dwt_rows, _no_dwt]
    comps = {tuple(takewhile(lambda x: x != _no_dwt, comp)) for comp in product(cbs, repeat=depth)}
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


def haar_dwt(image: np.array):
    # This function assumes that input.length=2^n, n>1
    output = np.zeros_like(image)
    length = image.shape[0] // 2
    while True:
        length = length // 2
        for i in range(length):
            output[i] = image[2 * i] + image[2 * i + 1]
            output[length + i] = image[2 * i] - image[2 * i + 1]

        if length == 1:
            return output
        image = output


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


def get_lut(wavelet: str):
    luts = {
        "db1": lut_db1,
        "haar": lut_db1,
        "db2": lut_db2,
        "db3": lut_db3
    }
    return luts.get(wavelet, 0)


def dwt_re(array: np.array, wavelet: str):
    lut = list(reversed(get_lut(wavelet)))
    lut_len = len(lut)
    out_len = array.shape[0] // 2 + lut_len // 2 - 1
    output = np.zeros(shape=out_len, dtype=np.float64)
    array_extended = array.copy()
    array_extended = np.insert(array_extended, 0, array[1])
    array_extended = np.insert(array_extended, 1, array[0])
    array_extended = np.append(array_extended, array[-1])
    array_extended = np.append(array_extended, array[-2])
    for i in range(out_len):
        for j in range(lut_len):
            output[i] += array_extended[2 * i + j] * lut[(lut_len - 1) - j]
    return output


def main():
    original = camera()
    results = my_dwt(original, 3)
    save_results(results)


if __name__ == "__main__":
    dummy = np.array([1, 2, 3, 4])
    print(dwt_re(dummy, 'haar'))
    print(dwt(dummy, 'haar')[0])
    print(dwt_re(dummy, 'db2'))
    print(dwt(dummy, 'db2')[0])
    print(np.convolve([2, 1, 1, 2, 3, 4, 4, 3], list(reversed(lut_db2)), mode='valid'))
