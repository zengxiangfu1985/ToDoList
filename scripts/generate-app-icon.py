#!/usr/bin/env python3
"""Generate app.ico and app.png from resources/icons/app_source.png (or legacy vector draw)."""

from __future__ import annotations

from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
ICONS = ROOT / "resources" / "icons"
SOURCE = ICONS / "app_source.png"
OUT_ICO = ICONS / "app.ico"
OUT_PNG = ICONS / "app.png"
ICO_SIZES = [16, 20, 24, 32, 40, 48, 64, 128, 256]
PNG_SIZE = 512


def square_image(img: Image.Image) -> Image.Image:
    img = img.convert("RGBA")
    w, h = img.size
    side = min(w, h)
    left = (w - side) // 2
    top = (h - side) // 2
    return img.crop((left, top, left + side, top + side))


def resize_icon(img: Image.Image, size: int) -> Image.Image:
    return img.resize((size, size), Image.Resampling.LANCZOS)


def from_source_png(source: Path) -> Image.Image:
    master = square_image(Image.open(source))
    return resize_icon(master, 256)


def draw_legacy_vector(size: int) -> Image.Image:
    from PIL import ImageDraw

    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    s = size / 64.0
    blue = (37, 99, 235, 255)
    white = (255, 255, 255, 242)
    green = (34, 197, 94, 255)
    radius = max(2, int(14 * s))
    draw.rounded_rectangle([0, 0, size - 1, size - 1], radius=radius, fill=blue)
    doc = [int(14 * s), int(16 * s), int(50 * s), int(48 * s)]
    draw.rounded_rectangle(doc, radius=max(1, int(4 * s)), fill=white)
    line_w = max(1, round(3 * s))
    for y in (28, 34, 40):
        x1 = int(22 * s)
        x2 = int(42 * s) if y != 34 else int(36 * s)
        draw.line([(x1, int(y * s)), (x2, int(y * s))], fill=blue, width=line_w)
    cx, cy, r = int(46 * s), int(44 * s), max(2, int(10 * s))
    draw.ellipse([cx - r, cy - r, cx + r, cy + r], fill=green)
    draw.line(
        [(int(41 * s), int(44 * s)), (int(44 * s), int(47 * s)), (int(53 * s), int(37 * s))],
        fill=(255, 255, 255, 255),
        width=max(1, round(3 * s)),
        joint="curve",
    )
    return img


def main() -> None:
    ICONS.mkdir(parents=True, exist_ok=True)

    if SOURCE.exists():
        master = from_source_png(SOURCE)
        print(f"Source: {SOURCE} ({SOURCE.stat().st_size} bytes)")
    else:
        master = draw_legacy_vector(256)
        print("Source: legacy vector (app_source.png not found)")

    resize_icon(master, PNG_SIZE).save(OUT_PNG, format="PNG", optimize=True)
    master.save(OUT_ICO, format="ICO", sizes=[(s, s) for s in ICO_SIZES])
    print(f"Generated: {OUT_PNG} ({OUT_PNG.stat().st_size} bytes, {PNG_SIZE}x{PNG_SIZE})")
    print(f"Generated: {OUT_ICO} ({OUT_ICO.stat().st_size} bytes, sizes={ICO_SIZES})")


if __name__ == "__main__":
    main()
