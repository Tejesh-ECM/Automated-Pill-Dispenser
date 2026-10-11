import sys,unittest,json,tempfile
from pathlib import Path
from unittest.mock import patch
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"desktop"))
class UITests(unittest.TestCase):
 def setUp(self):
  import tkinter as tk
  try:self.root=tk.Tk();self.root.withdraw()
  except tk.TclError as e:self.skipTest(str(e))
  from app import App
  self.app=App(self.root);self.scratch=tempfile.TemporaryDirectory();self.app.data=Path(self.scratch.name);self.root.update_idletasks()
 def tearDown(self):
  if hasattr(self,"app"):self.app.close()
  if hasattr(self,"scratch"):self.scratch.cleanup()
 def test_config_read_and_roundtrip(self):
  self.app.ready=True
  self.app.event(dict(event="config",names=["A","B","C","D"],times=[480,840,1200],doses=[[1,0,0,0],[0,2,0,0],[0,0,3,1]],enabled=True))
  p=self.app.config()
  self.assertEqual(p["doses"][2],[0,0,3,1])
  self.assertTrue(p["enabled"])
 def test_protocol_rejection(self):
  self.app.event(dict(event="status",protocol=2,firmware="SmartPillUSB-R4-2"))
  self.assertFalse(self.app.ready);self.assertIn("Wrong firmware",self.app.status.get())
 def test_no_automatic_retry(self):
  class BadSerial:
   def write(self,b):raise OSError("lost")
   def close(self):pass
  self.app.ready=True;self.app.serial=BadSerial()
  self.assertFalse(self.app.send(dict(cmd="dispense",id="M1",counts=[1,0,0,0])))
  self.assertIsNone(self.app.serial);self.assertIn("not retried",self.app.status.get())
 def test_refill_selection_uses_saved_label_and_waits_for_confirmation(self):
  self.app.ready=True
  self.app.event(dict(event="config",names=["A","B","C","D"],times=[480,840,1200],doses=[[1,0,0,0],[0,2,0,0],[0,0,3,1]],enabled=True))
  self.app.refill_picker.current(2)
  with patch.object(self.app,"send",return_value=True) as send:
   self.app.select_refill();send.assert_called_once_with({"cmd":"refill","container":3})
  self.assertIn("wait for device",self.app.refill_status.get())
  self.app.event(dict(event="status",state="REFILL",refill_selected=3,armed=True))
  self.assertIn("Container 3 LED ON",self.app.refill_status.get())
  self.app.event(dict(event="status",state="FAULT",refill_selected=0,armed=False))
  self.assertIn("all off",self.app.refill_status.get())
  self.assertEqual(str(self.app.refill_select["state"]),"disabled")
 def test_refill_selection_requires_connection(self):
  with patch.object(self.app,"send") as send:
   self.app.select_refill();send.assert_not_called()
 def test_upload_requires_verified_readback(self):
  self.app.ready=True
  event=dict(event="config",names=["A","B","C","D"],times=[480,840,1200],doses=[[1,0,0,0],[0,2,0,0],[0,0,3,1]],enabled=True)
  self.app.event(event)
  with patch.object(self.app,"send",return_value=True) as send:
   self.app.upload();self.assertIsNone(self.app.device_config);self.assertIsNotNone(self.app.pending)
   self.app.event(dict(event="config_saved"));self.assertIsNotNone(self.app.pending)
   send.reset_mock();self.app.select_refill();send.assert_not_called()
   with patch("app.messagebox.showerror"),patch("app.messagebox.askyesno") as ask:
    self.app.manual();ask.assert_not_called();send.assert_not_called()
   self.app.event(event);self.assertIsNone(self.app.pending);self.assertIsNotNone(self.app.device_config)
 def test_boot_reidentifies_firmware(self):
  self.app.ready=True;self.app.device_config={"old":True}
  self.app.event(dict(event="boot",protocol=2,firmware="Wrong"))
  self.assertFalse(self.app.ready);self.assertIsNone(self.app.device_config)
if __name__=="__main__":unittest.main()

