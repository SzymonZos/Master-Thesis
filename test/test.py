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


PADDING_MODES = {
    "symmetric": m.padding_mode.symmetric,
    "zero": m.padding_mode.zeropad
}

PRECISION = {
    "float32": 0,
    "float64": 1
}

WAVELETS = {
    "bior2.2": (m.lut_bior2_2_f, m.lut_bior2_2_d)
}

DWT_FUNCS = {
    "dwt 1d": (_dwt_rows, (m.dwt_1d_f, m.dwt_1d_d)),
    "dwt 2d": (_dwt2, (m.dwt_2d_f, m.dwt_2d_d)),
    "dwt 2d rows": (_dwt_rows, (m.dwt_2d_rows_f, m.dwt_2d_rows_d)),
    "dwt 2d cols": (_dwt_cols, (m.dwt_2d_cols_f, m.dwt_2d_cols_d))
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


def get_precision_test_suits(dtype):
    return chain(
        get_test_suit(get_1d_data(dtype), "dwt 1d"),
        get_test_suit(get_2d_data(dtype), "dwt 2d"),
        get_test_suit(get_2d_data(dtype), "dwt 2d rows"),
        get_test_suit(get_2d_data(dtype), "dwt 2d cols")
    )


def get_all_test_suits():
    return chain(
        get_precision_test_suits(np.float32),
        get_precision_test_suits(np.float64)
    )


@pytest.fixture
def dwt_fixture(request):
    data, wavelet, padding_mode, dwt_func = request.param
    precision = data.dtype.name
    dwt_ref, dwt_impl = DWT_FUNCS[dwt_func]
    dwt_impl = dwt_impl[PRECISION[precision]]
    ref = dwt_ref(data, wavelet, padding_mode)
    out = dwt_impl(data, WAVELETS[wavelet][PRECISION[precision]],
                   PADDING_MODES[padding_mode])
    print(f"{ref}\n{out}")
    return ref, out


@pytest.mark.parametrize("dwt_fixture", get_all_test_suits(), indirect=True)
def test_dwt(dwt_fixture):
    assert np.allclose(*dwt_fixture)
