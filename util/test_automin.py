import automin
import unittest

class TestCover(unittest.TestCase):
  def check(self, cover, real_minimal):
    minimal, found = automin.brute_force_set_cover(cover)
    complete = (frozenset.union(*cover.values())  if cover else ())
    self.assertItemsEqual(complete, found)
    self.assertItemsEqual(minimal, real_minimal)

  def test_no_test_cases(self):
    self.check({}, ())


  def test_none_exposed(self):
    self.check({'a':frozenset()}, ())


  def test_strictly_unnecessary1(self):
    cover = {
        'a':frozenset('Helo hi'),
        'b':frozenset('hi'),
      }
    self.check(cover, 'a')


  def test_strictly_unnecessary2(self):
    cover = {
        'a':frozenset('Helo hi'),
        'b':frozenset('hi'),
        'c':frozenset(),
      }
    self.check(cover, 'a')


  def test_strictly_unnecessary3(self):
    cover = {
        'a':frozenset('Helo hi'),
        'c':frozenset(),
      }
    self.check(cover, 'a')


  def test_strictly_unnecessary4(self):
    cover = {
        'a':frozenset('Helo hi'),
        'c':frozenset('Helo hi'),
      }
    self.check(cover, 'a')


  def test_strictly_unnecessary5(self):
    cover = {
        'a':frozenset(),
        'c':frozenset(),
      }
    self.check(cover, ())


  def test_all_necessary_disjoint(self):
    cover = {
        'a':frozenset('abcd'),
        'b':frozenset('efg'),
      }
    self.check(cover, cover.keys())


  def test_all_necessary_intersecting(self):
    cover = {
        'a':frozenset('abcd'),
        'b':frozenset('abefg'),
      }
    self.check(cover, cover.keys())


  def test_any_two(self):
    cover = {
        'a':frozenset('ab'),
        'b':frozenset('bc'),
        'c':frozenset('ac'),
      }
    minimal, found = automin.brute_force_set_cover(cover)
    complete = frozenset.union(*cover.values())
    self.assertEqual(len(minimal), 2)
    self.assertItemsEqual(complete, found)
    self.assertLess(frozenset(minimal), frozenset(cover.keys()))

  def test_almost_any_two(self):
    cover = {
        'a':frozenset('ab'),
        'b':frozenset('bc'),
        'c':frozenset('ac'),
        'd':frozenset('a'),
        'e':frozenset('b'),
        'f':frozenset('c'),
        'g':frozenset(),
      }
    minimal, found = automin.brute_force_set_cover(cover)
    complete = frozenset.union(*cover.values())
    self.assertEqual(len(minimal), 2)
    self.assertItemsEqual(complete, found)
    self.assertLess(frozenset(minimal), frozenset(cover.keys()))
