import jpeg2000_test as m
import numpy as np
import pytest
from pywt import dwt, dwt2
from itertools import starmap


ONE_DIMENSIONAL_DATA = [
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]),
    np.array([1, 2, 3, 4]),
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]),
    np.array([1, -2, 3, -4, 5, -6, 7, -8, 9, -10])
]


@pytest.fixture
def dwt_rows(request):
    out = m.dwt_1d_f(request.param, m.lut_bior2_2_f, m.padding_mode.symmetric)
    ref = dwt(request.param, 'bior2.2')[0]
    return out, ref


@pytest.mark.parametrize("dwt_rows", ONE_DIMENSIONAL_DATA, indirect=True)
def test_dwt_rows(dwt_rows):
    assert starmap(np.allclose, dwt_rows)


TWO_DIMENSIONAL_DATA = [
    np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]]),
    np.array([[1, 2], [3, 4]]),
    np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]),
    np.array([[1, -2], [3, -4], [5, -6], [7, -8], [9, -10]])
]


@pytest.fixture
def dwt_2d(request):
    out = m.dwt_2d_f(request.param, m.lut_bior2_2_f, m.padding_mode.symmetric)
    ref = dwt2(request.param, 'bior2.2')[0]
    return out, ref


@pytest.mark.parametrize("dwt_2d", TWO_DIMENSIONAL_DATA, indirect=True)
def test_dwt_cols(dwt_2d):
    assert starmap(np.allclose, dwt_2d)
