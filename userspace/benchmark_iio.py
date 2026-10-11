#!/usr/bin/env python3
"""Report IIO buffer throughput from a sysfs-configured /dev/iio:deviceN stream.
Run as a user with the appropriate device permissions after enabling the buffer."""
import argparse, os, time
p=argparse.ArgumentParser();p.add_argument('--device',default='/dev/iio:device0');p.add_argument('--seconds',type=float,default=5);p.add_argument('--chunk',type=int,default=65536);a=p.parse_args()
count=0;reads=0;start=time.monotonic();deadline=start+a.seconds
try:
 with open(a.device,'rb',buffering=0) as f:
  while time.monotonic()<deadline:
   try: data=f.read(a.chunk)
   except BlockingIOError: continue
   if not data: continue
   count+=len(data);reads+=1
except OSError as e:
 raise SystemExit(f'{a.device}: {e}')
elapsed=time.monotonic()-start
print(f'device={a.device} elapsed_s={elapsed:.3f} bytes={count} reads={reads} throughput_Bps={count/elapsed if elapsed else 0:.1f}')
