import numpy as np
import matplotlib.pyplot as plt

from pywt import dwt, dwt2, Wavelet
from pywt.data import camera

from itertools import product


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
    results = []
    for comp in product(cbs, repeat=depth):
        ll = image
        for cb in comp:
            if cb.__name__ == "_no_dwt":
                break
            ll = cb(ll)
        results.append(ll)
    return results


def main():
    print(Wavelet('bior1.3'))
    print(Wavelet('bior3.7'))
    print(Wavelet('db1'))

    # Load image
    original = camera()
    results = my_dwt(original, 3)
    # Wavelet transform of image, and plot approximation and details
    titles = ['Approximation', ' Horizontal detail',
              'Vertical detail', 'Diagonal detail']
    LL, (LH, HL, HH) = dwt2(original, 'bior1.3')
    ll, (lh, hl, hh) = dwt2(LL, 'bior1.3')

    plot_subbands(LL, LH, HL, HH, titles)
    plot_subbands(ll, lh, hl, hh, titles)


if __name__ == "__main__":
    main()
