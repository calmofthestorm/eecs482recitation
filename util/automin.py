#!/usr/bin/env python
# encoding: utf-8

# Automatically minimize autograder test cases using brute force.
#
# Alex Roper
# aroper@umich.edu
# EECS 482 GSI, Winter 2014
#
# Disclaimer: I wrote this in the hope you find it useful, but can't provide
# any guarantees of correctness or usefulness. You bear ALL RESPONSIBILITY for
# your use of this script, including lost points etc.
#
# In particular, this will break if you use a : or any whitespace in test
# case names, or the AG changes format.
#

# This is the optimization version of the set cover problem, which is NP hard.
# Good thing N is small. Brute force time!
# See http://en.wikipedia.org/wiki/Set_cover_problem)

import itertools
import sys

def parse():
  exposed = {}
  for line in sys.stdin:
    if not line.startswith('student test case test_'):
      continue

    test_name = line.split()[3]
    if 'exposed the' not in line:
      exposed[test_name] = frozenset()
      continue

    exposed[test_name] = frozenset(line.split(':')[1].strip())
  return exposed

def brute_force_set_cover(exposed):
  if not exposed:
    return ((), frozenset())

  needed = frozenset.union(*exposed.itervalues())

  if not needed:
    return ((), frozenset())

  # Brute force time! 2^20 is totally tractable!
  for count in xrange(1, 1 + len(exposed)):
    for chosen in itertools.combinations(exposed.iteritems(), count):
      names, cover = zip(*chosen)
      if frozenset.union(*cover) == needed:
        return names, needed
  else:
    assert 0

def main():
  exposed = parse()
  minimal, found = brute_force_set_cover(exposed)
  found = ''.join(sorted(found))

  if len(minimal) == len(exposed):
    print 'All %i test cases are necessary: %s' % (len(exposed), found)
  else:
    print ('You need %i cases to expose all %i bugs your current %i '
           'test cases get: %s' %
           (len(minimal), len(found), len(exposed), found))
  print
  print 'Keep:'
  for case, bugs in exposed.iteritems():
    if case in minimal:
      print '\t%s: %s' % (case, ''.join(sorted(bugs)))
  print
  print 'Cull:'
  for case, bugs in exposed.iteritems():
    if case not in minimal:
      print '\t%s: %s' % (case, ''.join(sorted(bugs)))

if __name__ == '__main__':
  if len(sys.argv) != 1:
    print "Usage: %s < AUTOGRADER_FEEDBACK" % sys.argv[0]
  else:
    main()
