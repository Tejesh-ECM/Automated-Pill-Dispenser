"""One-window wired controller. Run python desktop/app.py."""
import calendar,json,time,uuid
from pathlib import Path
from datetime import datetime
import tkinter as tk
from tkinter import ttk,messagebox
from model import make_config,encode,compatible

class App:
    def __init__(self,root):
        self.root=root;self.serial=None;self.ready=False;self.buffer=bytearray()
        self.last_rx=0;self.last_ping=0;self.logged_seq=-1;self.pending=None;self.device_config=None;self.last_state="DISCONNECTED"
        self.data=Path(__file__).resolve().parents[1]/"data";self.data.mkdir(exist_ok=True)
        root.title("SmartPill Budget - USB");root.geometry("870x750");root.minsize(820,730)
        frame=ttk.Frame(root,padding=15);frame.pack(fill="both",expand=True)
        ttk.Label(frame,text="Automatic schedule stays on the dispenser. Keep its power connected.",font=("Segoe UI",11,"bold")).pack(anchor="w")
        ttk.Label(frame,text="Use the fixed receiving tray and empty it before every dose. Tray contents are not detected.",wraplength=800).pack(anchor="w",pady=(4,12))
        bar=ttk.Frame(frame);bar.pack(fill="x")
        self.port=tk.StringVar();self.ports=ttk.Combobox(bar,textvariable=self.port,width=15);self.ports.pack(side="left")
        ttk.Button(bar,text="Refresh ports",command=self.refresh).pack(side="left",padx=4)
        ttk.Button(bar,text="Connect / disconnect",command=self.connect).pack(side="left")
        ttk.Button(bar,text="STOP",command=lambda:self.send({"cmd":"stop"})).pack(side="right")
        self.status=tk.StringVar(value="Disconnected");ttk.Label(frame,textvariable=self.status,wraplength=790).pack(anchor="w",pady=10)
        self.lcd=tk.StringVar(value="Disconnected\n")
        tk.Label(frame,textvariable=self.lcd,font=("Consolas",18),bg="#d9e7bf",width=20,height=2,anchor="w",justify="left").pack(anchor="w")
        grid=ttk.Frame(frame);grid.pack(anchor="w",pady=12)
        self.times=[tk.StringVar(value=t) for t in ["08:00","14:00","20:00"]]
        ttk.Label(grid,text="Container / medicine").grid(row=0,column=0,columnspan=2,sticky="w")
        for s,v in enumerate(self.times):
            ttk.Entry(grid,textvariable=v,width=9).grid(row=0,column=s+2,padx=7)
        self.names=[];self.counts=[]
        for i in range(4):
            ttk.Label(grid,text=str(i+1)).grid(row=i+1,column=0,padx=5)
            name=tk.StringVar(value=f"Container {i+1}");self.names.append(name)
            ttk.Entry(grid,textvariable=name,width=23).grid(row=i+1,column=1,pady=4)
            row=[]
            for s in range(3):
                value=tk.StringVar(value="0");row.append(value)
                ttk.Entry(grid,textvariable=value,width=9).grid(row=i+1,column=s+2,padx=7)
            self.counts.append(row)
        self.enabled=tk.BooleanVar(value=False)
        ttk.Checkbutton(frame,text="Enable saved daily schedule",variable=self.enabled).pack(anchor="w")
        bar=ttk.Frame(frame);bar.pack(fill="x",pady=8)
        for label,fn in [
            ("Save to dispenser",self.upload),("Read dispenser",lambda:self.send({"cmd":"get_config"})),
            ("Set clock",self.clock),("Enable controls",lambda:self.send({"cmd":"arm"}))]:
            ttk.Button(bar,text=label,command=fn).pack(side="left",padx=2)
        bar=ttk.Frame(frame);bar.pack(fill="x",pady=4)
        ttk.Button(bar,text="Refill instructions",command=self.refill).pack(side="left",padx=2)
        ttk.Button(bar,text="Clear fault after inspection",command=self.clear).pack(side="left",padx=2)
        self.period=tk.IntVar(value=1)
        ttk.Spinbox(bar,from_=1,to=3,textvariable=self.period,width=3).pack(side="left",padx=(16,3))
        ttk.Button(bar,text="Manual dose (period 1-3)",command=self.manual).pack(side="left")
        refillbar=ttk.LabelFrame(frame,text="Refill guide - select one container",padding=7);refillbar.pack(fill="x",pady=6)
        self.refill_choice=tk.StringVar()
        self.refill_picker=ttk.Combobox(refillbar,textvariable=self.refill_choice,state="readonly",width=27)
        self.refill_picker.pack(side="left",padx=3)
        self.refill_select=ttk.Button(refillbar,text="Light selected container",command=self.select_refill,state="disabled")
        self.refill_select.pack(side="left",padx=3)
        self.refill_finish=ttk.Button(refillbar,text="Finish refill",command=lambda:self.send({"cmd":"refill_done"}),state="disabled")
        self.refill_finish.pack(side="left",padx=3)
        self.refill_status=tk.StringVar(value="Refill LEDs: off / disconnected")
        ttk.Label(frame,textvariable=self.refill_status).pack(anchor="w")
        self.result=tk.StringVar(value="Last device result: unknown")
        ttk.Label(frame,textvariable=self.result,wraplength=790).pack(anchor="w",pady=12)
        ttk.Label(frame,text="Names are labels, not pill identification. Quantities are whole pills.\n"
                  "Missed times are skipped. 'Dispensed' does not confirm collection or intake.\n"
                  "Only the latest result survives offline; connected events are saved in data/events.jsonl.",
                  wraplength=790).pack(anchor="w")
        self.refresh();root.protocol("WM_DELETE_WINDOW",self.close);root.after(100,self.poll)
    def refresh(self):
        try:
            from serial.tools import list_ports
            values=[p.device for p in list_ports.comports()]
        except ImportError: values=[]
        self.ports["values"]=values
        if values and not self.port.get():self.port.set(values[0])
    def disconnect(self,text):
        if self.serial:
            if self.ready:
                try:self.serial.write(encode({"cmd":"refill_cancel"}))
                except Exception:pass
            try:self.serial.close()
            except Exception:pass
        self.serial=None;self.ready=False;self.pending=None;self.device_config=None;self.last_state="DISCONNECTED";self.logged_seq=-1;self.status.set(text)
        self.refill_select.configure(state="disabled");self.refill_finish.configure(state="disabled")
        self.refill_status.set("Disconnected; refill LEDs turn off within 5 seconds.")
    def connect(self):
        if self.serial:self.disconnect("Disconnected; saved schedule remains on device.");return
        try:
            import serial
            self.serial=serial.Serial(port=None,baudrate=115200,timeout=0,write_timeout=0.5)
            self.serial.dtr=False;self.serial.port=self.port.get();self.serial.open();self.serial.dtr=True
            self.ready=False;self.buffer.clear();self.last_rx=time.monotonic();self.last_ping=0
            self.status.set("Waiting for budget firmware identification...")
        except Exception as e:self.disconnect(str(e))
    def send(self,p):
        if not self.serial or (not self.ready and p["cmd"]!="ping"):
            self.status.set("Connect to compatible budget firmware first.");return False
        try:
            data=encode(p)
            if self.serial.write(data)!=len(data):raise OSError("Partial command write")
            return True
        except Exception as e:
            self.disconnect(f"USB error: {e}. Outcome uncertain; inspect device. Command was not retried.");return False
    def config(self):
        return make_config([v.get() for v in self.names],[v.get() for v in self.times],
                           [[v.get() for v in row] for row in self.counts],self.enabled.get())
    def upload(self):
        try:
            if self.pending is not None:
                self.status.set("Previous upload unresolved. Read dispenser settings before sending another.");return
            p=self.config()
            if self.send(p):
                self.pending=p;self.device_config=None;self.refill_select.configure(state="disabled")
                self.status.set("Sent; wait for device readback before using manual or refill controls.")
        except ValueError as e:messagebox.showerror("Settings",str(e))
    def clock(self):
        if messagebox.askyesno("Set clock","Set the dispenser to this computer's LOCAL date and time? Check the computer clock first."):
            self.send(dict(cmd="set_time",timestamp=calendar.timegm(datetime.now().timetuple())))
    def clear(self):
        if messagebox.askyesno("Inspect first","Empty the tray, remove any partial dose, clear the jam, close the cover and release the stop. Have you inspected the mechanism?"):
            self.send({"cmd":"clear"})
    def refill(self):
        messagebox.showinfo("Manual refill","Connect USB and wait for IDLE. Select the saved container/medicine and click Light selected container.\n"
            "Only that container's LED lights. Open the cover and fill its labelled port; the light does not detect pills.\n"
            "To fill another, close the first cap and select the next container.\n"
            "Close all caps and the cover to finish; LEDs switch off after one second. Finish refill also works with the cover closed.\n"
            "Do not open the cover during dispensing. Schedules due during refill are skipped.")
    def select_refill(self):
        if not self.ready or self.device_config is None or self.pending is not None:
            self.status.set("Connect and read saved container labels first.");return
        index=self.refill_picker.current()
        if index not in range(4):self.status.set("Select container 1-4.");return
        if self.send({"cmd":"refill","container":index+1}):
            self.refill_status.set("Selection sent; wait for device confirmation.")
    def manual(self):
        try:
            p=self.device_config;s=self.period.get()-1
            if p is None or self.pending is not None:raise ValueError("Wait for verified dispenser settings first.")
            if not 0<=s<3 or not sum(p["doses"][s]):raise ValueError("Choose a nonempty period, 1-3.")
            if messagebox.askyesno("Extra manual dose","This is an EXTRA dose and does not replace a scheduled dose.\n"
                     "Check the saved labels, empty the fixed tray, and close the cover. Dispense now?"):
                self.send(dict(cmd="dispense",id=uuid.uuid4().hex[:20],counts=p["doses"][s]))
        except (ValueError,tk.TclError) as e:messagebox.showerror("Manual dose",str(e))
    def event(self,e):
        # A board reset/new firmware must be identified again, even on an open port.
        if self.ready and e.get("event")=="boot":
            self.ready=False;self.device_config=None;self.pending=None;self.logged_seq=-1
        if not self.ready:
            if not compatible(e):
                if e.get("event") in ("status","boot"):self.disconnect("Wrong firmware; load SmartPillBudget protocol 4.")
                return
            self.ready=True;self.send({"cmd":"get_config"})
        if e.get("event")=="config":
            try:
                names=e["names"];times=e["times"];d=e["doses"]
                valid=make_config(names,[f"{x//60:02}:{x%60:02}" for x in times],
                                  [[d[s][i] for s in range(3)] for i in range(4)],e["enabled"])
                for i,v in enumerate(self.names):v.set(valid["names"][i])
                for s,v in enumerate(self.times):v.set(f"{valid['times'][s]//60:02}:{valid['times'][s]%60:02}")
                for i,row in enumerate(self.counts):
                    for s,v in enumerate(row):v.set(str(valid["doses"][s][i]))
                self.enabled.set(valid["enabled"]);self.device_config=valid
                self.refill_picker["values"]=[f"{i+1} - {name or '(unassigned)'}" for i,name in enumerate(valid["names"])]
                self.refill_picker.current(0)
                self.refill_select.configure(state="normal" if self.last_state in ("IDLE","REFILL") else "disabled")
                if self.pending is not None:
                    matches=all(valid[k]==self.pending[k] for k in ("enabled","times","names","doses"))
                    if matches:
                        (self.data/"last_uploaded.json").write_text(json.dumps(valid,indent=2),encoding="utf-8")
                    self.pending=None
                    self.status.set("Device settings verified." if matches else "Device settings differ; review before uploading.")
            except (KeyError,IndexError,TypeError,ValueError):self.disconnect("Invalid device configuration")
            return
        if "lcd" in e:self.lcd.set("\n".join(e["lcd"][:2]))
        state=e.get("state","");detail=e.get("detail","")
        if state:self.last_state=state
        if "refill_selected" in e:
            selected=e["refill_selected"]
            self.refill_status.set(f"Container {selected} LED ON; other LEDs OFF." if selected in (1,2,3,4) else "Refill LEDs: all off")
            self.refill_select.configure(state="normal" if self.device_config is not None and state in ("IDLE","REFILL") else "disabled")
            self.refill_finish.configure(state="normal" if state=="REFILL" else "disabled")
        self.status.set(f"{state} | controls {'enabled' if e.get('armed') else 'disabled'} | {detail}")
        if e.get("event")=="config_saved" and self.pending:
            self.send({"cmd":"get_config"})
            self.status.set("Device acknowledged settings; waiting for verified readback.")
        seq=e.get("sequence")
        if seq is not None and seq!=self.logged_seq:
            self.logged_seq=seq
            result=f"Last device result: {e.get('outcome','UNKNOWN')} | device timestamp {e.get('timestamp',0)}"
            self.result.set(result)
            with (self.data/"events.jsonl").open("a",encoding="utf-8") as f:
                f.write(json.dumps(dict(received=datetime.now().isoformat(),device=e))+"\n")
    def poll(self):
        now=time.monotonic()
        if self.serial:
            try:
                self.buffer.extend(self.serial.read(self.serial.in_waiting))
                if len(self.buffer)>16384:raise OSError("Oversized serial stream")
                while b"\n" in self.buffer:
                    raw,_,self.buffer=self.buffer.partition(b"\n")
                    try:e=json.loads(raw)
                    except (ValueError,UnicodeError):continue
                    if isinstance(e,dict):
                        self.last_rx=now;self.event(e)
                        if not self.serial:break
                if self.serial and now-self.last_rx>8:self.disconnect("No device response; commands not retried. Inspect device.")
                if self.serial and now-self.last_ping>1:self.last_ping=now;self.send({"cmd":"ping"})
            except Exception as e:self.disconnect(f"Connection/log error: {e}")
        self.root.after(100,self.poll)
    def close(self):
        self.disconnect("Closed");self.root.destroy()
if __name__=="__main__":
    root=tk.Tk();App(root);root.mainloop()

