"""Standard-Python template generator; millimetres. Fit is NOT verified.
Edit dimensions after measuring the assembled original container inlets and boards.
Exports orthogonal, closed STL solids and equivalent editable OpenSCAD.
"""
from pathlib import Path
from collections import Counter
import struct,math,json
ROOT=Path(__file__).resolve().parent
OUT=ROOT/"budget_candidates";OUT.mkdir(exist_ok=True)
def export(name,adds,cuts=()):
    axes=[sorted({b[k] for b in adds+list(cuts)}|{b[k+3] for b in adds+list(cuts)}) for k in range(3)]
    X,Y,Z=axes
    def inside(p,b):return all(b[k]<p[k]<b[k+3] for k in range(3))
    occupied=set()
    for i in range(len(X)-1):
      for j in range(len(Y)-1):
       for k in range(len(Z)-1):
        p=((X[i]+X[i+1])/2,(Y[j]+Y[j+1])/2,(Z[k]+Z[k+1])/2)
        if any(inside(p,b) for b in adds) and not any(inside(p,b) for b in cuts):occupied.add((i,j,k))
    quads=[]
    for i,j,k in occupied:
      a,b=X[i],X[i+1];c,d=Y[j],Y[j+1];e,f=Z[k],Z[k+1]
      faces=[
       ((i,j,k-1),[(a,d,e),(b,d,e),(b,c,e),(a,c,e)]),
       ((i,j,k+1),[(a,c,f),(b,c,f),(b,d,f),(a,d,f)]),
       ((i-1,j,k),[(a,d,e),(a,c,e),(a,c,f),(a,d,f)]),
       ((i+1,j,k),[(b,c,e),(b,d,e),(b,d,f),(b,c,f)]),
       ((i,j-1,k),[(a,c,e),(b,c,e),(b,c,f),(a,c,f)]),
       ((i,j+1,k),[(b,d,e),(a,d,e),(a,d,f),(b,d,f)])]
      for neighbor,q in faces:
       if neighbor not in occupied:quads.append(q)
    ts=[t for q in quads for t in [(q[0],q[1],q[2]),(q[0],q[2],q[3])]]
    edges=Counter(tuple(sorted((t[k],t[(k+1)%3]))) for t in ts for k in range(3))
    oriented=Counter((t[k],t[(k+1)%3]) for t in ts for k in range(3))
    assert all(v==2 for v in edges.values())
    assert all(v==oriented[(b,a)] for (a,b),v in oriented.items())
    def cross(a,b):return (a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0])
    volume=sum(sum(a[k]*cross(b,c)[k] for k in range(3))/6 for a,b,c in ts)
    expected=sum((X[i+1]-X[i])*(Y[j+1]-Y[j])*(Z[k+1]-Z[k]) for i,j,k in occupied)
    assert abs(volume-expected)<.001 and volume>0
    with (OUT/(name+".stl")).open("wb") as fp:
      fp.write(("BUDGET TEMPLATE - FIT NOT VERIFIED: "+name).encode()[:80].ljust(80,b" "));fp.write(struct.pack("<I",len(ts)))
      for a,b,c in ts:
       normal=cross(tuple(b[k]-a[k] for k in range(3)),tuple(c[k]-a[k] for k in range(3)))
       length=math.sqrt(sum(x*x for x in normal))
       fp.write(struct.pack("<12fH",*[x/length for x in normal],*a,*b,*c,0))
    def box(b):return f"translate({list(b[:3])}) cube({[b[i+3]-b[i] for i in range(3)]});"
    scad="// TEMPLATE ONLY. Measure and edit before printing. Millimetres.\ndifference(){union(){\n"
    scad+="\n".join(box(b) for b in adds)+"\n}\n"+"\n".join(box(b) for b in cuts)+"\n}\n"
    (OUT/(name+".scad")).write_text(scad)
    return dict(part=name,triangles=len(ts),closed_edges=True,consistent_orientation=True,volume_mm3=round(volume,3),fit="NOT VERIFIED")

checks=[]
# EXAMPLE ONLY: inlet positions are NOT extracted assembly coordinates.
W,H,T=190,150,3
centres=[(50,45),(140,45),(50,105),(140,105)]
ports=[(x-16,y-16,-1,x+16,y+16,T+1) for x,y in centres]
mounts=[(x-2,y-2,-1,x+2,y+2,4) for x,y in [(8,8),(182,8),(8,142),(182,142)]]
checks.append(export("manual_refill_plate",[(0,0,0,W,H,T)],ports+mounts))
# Four identical removable caps; label externally 1-4. 0.4 mm side clearance on plug.
checks.append(export("refill_cap_print_four",[(0,0,0,40,40,3),(4.4,4.4,-4,35.6,35.6,0)]))
# Shared lift-off guard covers all caps; retained with captive screws/strap.
checks.append(export("shared_refill_guard",[(0,0,0,196,156,15)],[(3,3,-1,193,153,12)]))
checks.append(export("lcd_16x2_plate",[(0,0,0,120,65,3)],[
 (24,21,-1,96,46,4), # example viewing window 72 x 25
 *[(x-1.7,y-1.7,-1,x+1.7,y+1.7,4) for x,y in [(22.5,18),(97.5,18),(22.5,49),(97.5,49)]]
]))
# Fixed cradle, screwed down to base. Insert separately sourced cleanable liner.
checks.append(export("fixed_receiving_tray",[(0,0,0,130,95,20)],
 [(3,3,3,127,92,21),(8,8,-1,12,12,3.1),(118,8,-1,122,12,3.1),
 (8,83,-1,12,87,3.1),(118,83,-1,122,87,3.1)]))
checks.append(export("controller_plate",[(0,0,0,110,85,3)],
 [(x-5,y-1.8,-1,x+5,y+1.8,4) for x,y in [(20,16),(21.3,64.3),(72.1,21.1),(72.1,49.1)]]
))
checks.append(export("cover_switch_bracket",[(0,0,0,35,25,3),(0,0,3,35,3,23)],
 [(x-2,8,-1,x+2,20,4) for x in (8,27)]+[(5,-1,9,30,4,12)]))
checks.append(export("refill_led_tab_print_four",[(0,0,0,18,14,3)],
 [(6.3,4.3,-1,11.7,9.7,4),(1,5,-1,4,9,4)]))
(OUT/"geometry_checks.json").write_text(json.dumps(checks,indent=2))
print("Exported",len(checks),"closed oriented STL templates; physical fit remains unverified")

