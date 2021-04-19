import jpeg2000_test as m
import numpy as np
from pywt import dwt


def test():
    dummy = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    out = m.downsampling_convolution_f(dummy, m.lut_bior2_2_f,
                                       m.padding_mode.symmetric, 1)
    out_2 = dwt(dummy, 'bior2.2')[0]
    print(f"Finished\n{out}\n{out_2}")
    assert np.allclose(out, out_2)


if __name__ == "__main__":
    test()
