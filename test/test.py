from itertools import product, chain

import numpy as np
import pytest

from pywt import dwt, dwt2
import jpeg2000_test as m


def _dwt2(data, wavelet, mode):
    ll, (_, _, _) = dwt2(data, wavelet, mode)
    return ll


def _dwt_rows(data, wavelet, mode):
    ll, _ = dwt(data, wavelet, mode)
    return ll


def _dwt_cols(data, wavelet, mode):
    ll, _ = dwt(data.T, wavelet, mode)
    return ll.T


WAVELETS = {
    "bior2.2": m.lut_bior2_2_f
}

PADDING_MODES = {
    "symmetric": m.padding_mode.symmetric,
    "zero": m.padding_mode.zeropad
}

DWT_FUNCS = {
    "dwt 1d": (_dwt_rows, m.dwt_1d_f),
    "dwt 2d": (_dwt2, m.dwt_2d_f),
    "dwt 2d rows": (_dwt_rows, m.dwt_2d_rows_f),
    "dwt 2d cols": (_dwt_cols, m.dwt_2d_cols_f)
}


def get_1d_data(dtype):
    return [
        np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], dtype=dtype),
        np.array([1, 2, 3, 4], dtype=dtype),
        np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15], dtype=dtype),
        np.array([1, -2, 3, -4, 5, -6, 7, -8, 9, -10], dtype=dtype)
    ]


def get_2d_data(dtype):
    return [
        np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]], dtype=dtype),
        np.array([[1, 2], [3, 4]], dtype=dtype),
        np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]], dtype=dtype),
        np.array([[1, -2], [3, -4], [5, -6], [7, -8], [9, -10]], dtype=dtype)
    ]


def get_test_suit(data, dwt_func: str):
    return product(data, WAVELETS, PADDING_MODES, [dwt_func])


def get_all_test_suits():
    return chain(
        get_test_suit(get_1d_data(np.float32), "dwt 1d"),
        get_test_suit(get_2d_data(np.float32), "dwt 2d"),
        get_test_suit(get_2d_data(np.float32), "dwt 2d rows"),
        get_test_suit(get_2d_data(np.float32), "dwt 2d cols")
    )


@pytest.fixture
def dwt_fixture(request):
    data, wavelet, padding_mode, dwt_func = request.param
    dwt_ref, dwt_impl = DWT_FUNCS[dwt_func]
    ref = dwt_ref(data, wavelet, padding_mode)
    out = dwt_impl(data, WAVELETS[wavelet], PADDING_MODES[padding_mode])
    print(f"{ref}\n{out}")
    return ref, out


@pytest.mark.parametrize("dwt_fixture", get_all_test_suits(), indirect=True)
def test_dwt(dwt_fixture):
    assert np.allclose(*dwt_fixture)
