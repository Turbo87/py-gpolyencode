#!/usr/bin/env python

try:
    from setuptools import setup, Extension
except:
    from distutils.core import setup, Extension

setup(name='GPolyEncode',
    version='0.1.1',
    description="Google Maps Polyline encoding (pure Python)",
    long_description="Encode line & polygon coordinates for use in Google Maps.",
    author='Robert Coup',
    author_email='robert.coup@koordinates.com',
    provides=['gpolyencode'],
    keywords='gis,geospatial,google-maps,gmaps,mapping',
    url='http://code.google.com/p/py-gpolyencode/',
    py_modules=['gpolyencode'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Web Environment',
        'License :: OSI Approved :: BSD License',
        'Intended Audience :: Developers',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering :: GIS',
        'Topic :: Utilities'],
)

