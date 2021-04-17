from setuptools import setup


setup(
    name="jpeg2000_test",
    version="0.0.1",
    author="Szymon Zosgornik",
    author_email="szymon.zosgornik@gmail.com",
    description="A jpeg2000 test project using pybind11 and CMake",
    packages=["test"],
    long_description="",
    include_package_data=True,
    classifiers=[
        # Trove classifiers
        # The full list is here: https://pypi.python.org/pypi?%3Aaction=list_classifiers
        'Development Status :: 3 - Alpha',
    ]
)
