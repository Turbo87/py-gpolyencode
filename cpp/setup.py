#!/usr/bin/env python

try:
    from setuptools import setup, Extension
except:
    from distutils.core import setup, Extension

setup(name='cGPolyEncode',
    version='0.1.1',
    description="Google Maps Polyline encoding (C++ extension)",
    long_description="Encode line & polygon coordinates for use in Google Maps.",
    author='Robert Coup',
    author_email='robert.coup@koordinates.com',
    provides=['cgpolyencode'],
    keywords='gis,geospatial,google-maps,gmaps,mapping',
    url='http://code.google.com/p/py-gpolyencode/',
    ext_modules=[
        Extension("cgpolyencode", ["gpolyencode_py.cpp", "GPolyEncoder.cpp"]),
    ],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Web Environment',
        'License :: OSI Approved :: BSD License',
        'Intended Audience :: Developers',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: C++',
        'Topic :: Scientific/Engineering :: GIS',
        'Topic :: Utilities'],
)

