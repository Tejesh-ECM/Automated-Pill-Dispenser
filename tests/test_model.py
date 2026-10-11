import sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"desktop"))
from model import make_config,encode,compatible
class ModelTests(unittest.TestCase):
 def test_roundtrip(self):
  p=make_config(["A","B","C","D"],["08:00","14:00","20:00"],[[1,0,2],[0,1,0],[0,0,0],[2,2,2]],True)
  self.assertEqual(p["doses"],[[1,0,0,2],[0,1,0,2],[2,0,0,2]])
  self.assertLess(len(encode(p)),600)
 def test_invalid_inputs(self):
  for times in [["08:00","08:00","20:00"],["25:00","14:00","20:00"],["x","14:00","20:00"]]:
   with self.assertRaises(ValueError):make_config(["A"]*4,times,[[1]*3]*4,True)
  for row in [["-1","1","0"],["1.5","0","0"],["11","0","0"]]:
   with self.assertRaises(ValueError):make_config(["A"]*4,["08:00","14:00","20:00"],[row]*4,True)
 def test_names_and_protocol(self):
  with self.assertRaises(ValueError):make_config([""]*4,["08:00","14:00","20:00"],[[1]*3]*4,True)
  self.assertFalse(compatible(dict(protocol=2,firmware="SmartPillUSB-R4-2")))
  self.assertTrue(compatible(dict(protocol=4,firmware="SmartPillBudget-R4-4")))
if __name__=="__main__":unittest.main()

