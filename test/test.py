import jpeg2000_test as m
import numpy as np
import pytest
from pywt import dwt
from itertools import starmap


ONE_DIMENSIONAL_DATA = [
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]),
    np.array([1, 2, 3, 4]),
    np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]),
    np.array([1, -2, 3, -4, 5, -6, 7, -8, 9, -10])
]


@pytest.fixture
def dwt_rows(request):
    out = m.downsampling_convolution_f(request.param, m.lut_bior2_2_f,
                                       m.padding_mode.symmetric, 1)
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
def dwt_cols(request):
    ref = dwt(request.param.T, 'bior2.2')[0]
    return request.param, ref.T


# @pytest.mark.parametrize("dwt_cols", TWO_DIMENSIONAL_DATA, indirect=True)
# def test_dwt_cols(dwt_cols):
#     out = m.downsampling_convolution_f(dwt_cols[0], m.lut_bior2_2_f,
#                                        m.padding_mode.symmetric,
#                                        dwt_cols[0].shape[1])
#     print(out)
#     assert np.allclose(out, dwt_cols[1])
