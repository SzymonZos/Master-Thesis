import os, shutil
import matplotlib.pyplot as plt

from pywt import dwt, dwt2, Wavelet
from pywt.data import camera

from itertools import product, takewhile


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


def main():
    original = camera()
    results = my_dwt(original, 3)
    save_results(results)


if __name__ == "__main__":
    main()
