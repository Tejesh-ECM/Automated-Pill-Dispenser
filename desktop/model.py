"""Small, pure helpers shared by the desktop application and tests."""
import json
def make_config(names, times, doses, enabled):
    if len(names)!=4 or len(times)!=3 or len(doses)!=4:
        raise ValueError("Use four containers and three daily times.")
    minutes=[]
    for value in times:
        parts=value.split(":")
        if len(parts)!=2 or not all(x.isdigit() for x in parts): raise ValueError("Times must be HH:MM.")
        h,m=map(int,parts)
        if not 0<=h<24 or not 0<=m<60: raise ValueError("Invalid time.")
        minutes.append(h*60+m)
    if len(set(minutes))!=3: raise ValueError("Use three different times.")
    clean=[]
    for name,row in zip(names,doses):
        if len(name)>16 or any(ord(c)<32 or ord(c)>126 for c in name):
            raise ValueError("Medicine labels: up to 16 plain ASCII characters.")
        if len(row)!=3: raise ValueError("Enter three quantities per container.")
        values=[]
        for value in row:
            if not str(value).isdigit() or not 0<=int(value)<=10: raise ValueError("Quantities must be whole pills, 0-10.")
            values.append(int(value))
        if any(values) and not name.strip(): raise ValueError("Name every used container.")
        clean.append(values)
    return dict(cmd="set_config",names=names,times=minutes,
                doses=[[clean[i][s] for i in range(4)] for s in range(3)],enabled=bool(enabled))
def encode(packet):
    data=(json.dumps(packet,separators=(",",":"),ensure_ascii=True)+"\n").encode("ascii")
    if len(data)>599: raise ValueError("Command too long.")
    return data
def compatible(event):
    return event.get("protocol")==4 and event.get("firmware")=="SmartPillBudget-R4-4"

