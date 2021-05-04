from typing import List
from itertools import starmap, product, chain

import numpy as np
import pytest

from pywt import dwt, dwt2
import jpeg2000_test as m


WAVELETS = {
    "bior2.2": m.lut_bior2_2_f
}

PADDING_MODES = {
    "symmetric": m.padding_mode.symmetric,
    "zero": m.padding_mode.zeropad
}

DWT_FUNCS = {
    "dwt 1d": (dwt, m.dwt_1d_f),
    "dwt 2d": (dwt2, m.dwt_2d_f)
}

DATA_1D = [
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]),
    np.array([1, 2, 3, 4]),
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]),
    np.array([1, -2, 3, -4, 5, -6, 7, -8, 9, -10])
]

DATA_2D = [
    np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]]),
    np.array([[1, 2], [3, 4]]),
    np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]),
    np.array([[1, -2], [3, -4], [5, -6], [7, -8], [9, -10]])
]


def get_test_suit(data: List[np.array], dwt_func: str):
    return product(data, WAVELETS, PADDING_MODES, [dwt_func])


def get_all_test_suits():
    return chain(
        get_test_suit(DATA_1D, "dwt 1d"),
        get_test_suit(DATA_2D, "dwt 2d")
    )


@pytest.fixture
def dwt_fixture(request):
    data, wavelet, padding_mode, dwt_func = request.param
    dwt_ref, dwt_impl = DWT_FUNCS[dwt_func]
    ref = dwt_ref(data, wavelet, padding_mode)[0]
    out = dwt_impl(data, WAVELETS[wavelet], PADDING_MODES[padding_mode])
    return ref, out


@pytest.mark.parametrize("dwt_fixture", get_all_test_suits(), indirect=True)
def test_dwt(dwt_fixture):
    assert starmap(np.allclose, dwt_fixture)
