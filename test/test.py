import jpeg2000_test as m


def test():
    assert m.__version__ == "dev"
    assert m.add(1, 2) == 3
    assert m.subtract(1, 2) == -1
    a, b, c = 1.0, 1.0, 1.0
    m.downsampling_convolution_f(a, 1, b, 1, c, m.padding_mode.zeropad, 1)
    print(f"Finished: {a}, {b}, {c}")


if __name__ == "__main__":
    test()
