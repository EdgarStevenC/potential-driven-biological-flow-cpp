#!/usr/bin/env python3
import sys
from pathlib import Path
from PIL import Image

if len(sys.argv) != 3:
    print("Usage: python3 scripts/make_gif.py <frames_dir> <output_gif>")
    sys.exit(1)

frames_dir = Path(sys.argv[1])
out_gif = Path(sys.argv[2])
frames = sorted(frames_dir.glob("frame_*.ppm"))
if not frames:
    raise SystemExit(f"No frames found in {frames_dir}")

imgs = [Image.open(f).convert("P", palette=Image.Palette.ADAPTIVE) for f in frames]
out_gif.parent.mkdir(parents=True, exist_ok=True)
imgs[0].save(out_gif, save_all=True, append_images=imgs[1:], duration=45, loop=0, optimize=True)
print(f"Wrote {out_gif} with {len(imgs)} frames")
