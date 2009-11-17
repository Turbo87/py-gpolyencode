#!/usr/bin/env python

import os
import re
import unittest
import time

try:
    import json     # Py2.6+
except ImportError:
    import simplejson as json

import gpolyencode
import cgpolyencode

class CGPolyEncoderTest(unittest.TestCase):
    def setUp(self):
        self.E = cgpolyencode.GPolyEncoder
        self.e = cgpolyencode.GPolyEncoder()
    
    def test_glineenc(self):
        # use same tests & setup as glineenc
        e = self.E(num_levels=4, zoom_factor=32, threshold=0.00001)
        
        p = ((-120.2, 38.5), (-126.453, 43.252), (-120.95, 40.7))
        r = e.encode(p)
        # JS:                          "_p~iF~ps|U_c_\\fhde@~lqNwxq`@",         "BBB"
        # glineenc:                    '_p~iF~ps|U_c_\\\\fhde@~lqNwxq`@',       'BBB'
        self.assertEqual(r, {'points':'_p~iF~ps|U_c_\\fhde@~lqNwxq`@', 'levels':'BBB', 'zoomFactor':32, 'numLevels':4})
        
        p = ((-122.1419, 37.4419), (-122.1519, 37.4519), (-122.1819, 37.4619),)
        r = e.encode(p)
        # JS:                         "yzocFzynhVq}@n}@o}@nzD",          "B@B"
        # glineenc:                   'yzocFzynhVq}@n}@o}@nzD',          'B@B'
        self.assertEqual(r, {'points':'yzocFzynhVq}@n}@o}@nzD', 'levels':'B@B', 'zoomFactor':32, 'numLevels':4})
        
        p = [(-120.2, 38.5)]
        r = e.encode(p)
        # JS:                         "_p~iF~ps|U",          "BB"
        # glineenc:                   '_p~iF~ps|U',          'B'
        self.assertEqual(r, {'points':'_p~iF~ps|U', 'levels':'BB', 'zoomFactor':32, 'numLevels':4})
        
    def test_java(self):
        # use same tests & setup as Java
        e = self.E(18, 2, 0.00001, True)
        p = ((8.94328,52.29834), (8.93614,52.29767), (8.93301,52.29322), (8.93036,52.28938), (8.97475,52.27014))
        r = e.encode(p)
        # JS:                         "soe~Hovqu@dCrk@xZpR~VpOfwBmtG",          "PG@IP"
        # Java:                       'soe~Hovqu@dCrk@xZpR~VpOfwBmtG',          'PPPPP'
        self.assertEqual(r, {'points':'soe~Hovqu@dCrk@xZpR~VpOfwBmtG', 'levels':'PG@IP', 'zoomFactor':2, 'numLevels':18})
        
    def test_js(self):
        pass
    

class GPolyEncoderTest(CGPolyEncoderTest):
    """ 
    Test the same as the C extension, except do some more
    invasive testing of the internal python code.
    """
    def setUp(self):
        self.E = gpolyencode.GPolyEncoder
        self.e = gpolyencode.GPolyEncoder()
    
    def test_glineenc_encode_number(self):
        # use same tests & setup as glineenc
        e = self.E(num_levels=4, zoom_factor=32, threshold=0.00001)
        
        self.assertEqual(e._encode_signed_number(int(-179.9832104 * 1E5)), '`~oia@')
        self.assertEqual(e._encode_signed_number(int(-120.2 * 1E5)), '~ps|U')
        self.assertEqual(e._encode_signed_number(int(38.5 * 1E5)), '_p~iF')

# more data
def _load_data(data_file):
    f = open(data_file, 'r')
    s = 0
    for line in f:
        if s == 0:
            data = []
            name = line.strip()
            reverse_coords = '(yx)' in name
            if '(t=' in name:
                threshold = float(re.search('\(t=(\d+\.\d+)\)', name).groups()[0])
            else:
                threshold = 0.00001
            s += 1
        elif s == 1:
            expected = json.loads(line.strip())
            s += 1
        elif s == 2 and len(line.strip()):
            coords = map(float, re.split('[, ]+', line.strip(), 2))
            if reverse_coords:
                coords.reverse()
            data.append(tuple(coords))
        else:
            s = 0
            yield name, data, expected, threshold
            data = []
    
    if len(data):
        yield name, data, expected, threshold


def data_test_c():
    for t in _build_data_tests(cgpolyencode.GPolyEncoder):
        yield t

def data_test():
    for t in _build_data_tests(gpolyencode.GPolyEncoder):
        yield t

def _build_data_tests(encoder):
    from nose.tools import assert_equal
    for name,data,expected,threshold in _load_data(os.path.join(os.path.split(__file__)[0], 'test_data.txt')):
        e = encoder(zoom_factor=expected['zoomFactor'], num_levels=expected['numLevels'], threshold=threshold)
        s_info = "%s.%s: %s" % (encoder.__module__, encoder.__name__, name)
        
        def check():
            t0 = time.clock()
            r = e.encode(data)
            te = time.clock() - t0
            print "\n%s: runtime (%d points): %0.3fs" % (s_info, len(data), te)
            assert_equal(r['levels'], expected['levels'], 'levels don\'t match: got=%s expected=%s' % (repr(r['levels'][:200]), repr(expected['levels'][:200])))
            assert_equal(r['points'], expected['points'], 'points don\'t match: got=%s expected=%s' % (repr(r['points'][:200]), repr(expected['points'][:200])))
            assert_equal(r, expected, "Entire data doesn't match got=%s expected=%s" % (repr(r)[-200:], repr(expected)[-200:]))
        
        check.description = s_info
        yield (check,)


if __name__ == "__main__":
    import nose
    nose.main()
