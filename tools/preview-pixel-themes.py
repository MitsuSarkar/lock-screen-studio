"""Render a design preview with the same glyph outlines and layout constants."""
from pathlib import Path
import html
import json

ROOT = Path(__file__).resolve().parent.parent
data = json.loads((ROOT / 'reference/pixel-glyphs.json').read_text())
glyphs = data['glyphs']

def text_svg(text, size, fg, accent=None, tracking=0, shadow='#101320'):
    width = sum(glyphs[str(ord(c))]['advance'] * size / data['em'] + tracking for c in text) - tracking
    height = size * data['height'] / data['em']
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width+2}" height="{height+2}">']
    for layer in (0, 1):
        x = 0
        for i, c in enumerate(text):
            g = glyphs[str(ord(c))]
            color = shadow if layer == 0 else (accent if accent and i > 2 else fg)
            if layer and c == '|': color = '#BE8DAE'
            offset = 1 if layer == 0 else 0
            parts.append(f'<path d="{g["path"]}" fill="{color}" transform="translate({x+offset},{offset}) scale({size/data["em"]})"/>')
            x += g['advance'] * size / data['em'] + tracking
    parts.append('</svg>')
    return ''.join(parts), width + 2, height + 2

themes = [
    ('Coffee', 'bottomLeft', '#FFF3DF', '#D6B58B', '#211724', '#372C3A', True, False),
    ('Dusk', 'topLeft', '#F2EDE7', '#CDA69B', '#201E2A', '#3A2E42', False, False),
    ('Sky', 'topRight', '#FFFFFF', '#FFE5B0', '#547E94', '#5BA0CD', True, False),
    ('Night', 'topLeft', '#F3EDF8', '#56C5DC', '#252237', '#23283D', False, True),
    ('Sakura', 'topLeft', '#384654', '#AE718B', '#FFFFFF', '#E1E3E9', False, False),
    ('Rain', 'topLeft', '#E4EEFF', '#83B7DB', '#101526', '#15233F', False, False),
    ('Hollow', 'bottom', '#ECE8DC', '#AFAB9D', '#111110', '#22251F', False, False),
    ('Sunset', 'topLeft', '#FFE2B5', '#EEBCA4', '#75465D', '#765365', False, False),
]
out = ['''<!doctype html><meta charset="utf-8"><style>
*{box-sizing:border-box}body{margin:0;background:#101216;color:#e6e9ef;font:14px "Segoe UI",sans-serif;padding:24px}
h1{font-size:24px;margin:0 0 6px}p{color:#a9b1bd;margin:0 0 22px}.gallery{display:grid;grid-template-columns:1fr 1fr;gap:18px}
.label{font-size:15px;margin:0 0 8px}.scene{height:230px;position:relative;overflow:hidden;border:1px solid #ffffff16}
.clock,.date{position:absolute}.footer{margin-top:18px;font-size:12px;color:#858e9e}
</style><h1>Lock Screen Studio · Pixel themes</h1><p>Actual embedded glyph outlines · layout preview · supply your own Windows wallpaper</p><div class="gallery">''']
for name, anchor, fg, accent, shadow, bg, above, two in themes:
    clock, cw, ch = text_svg('12|48' if two else '12:48', 47, fg, accent if two else None, shadow=shadow)
    date, dw, dh = text_svg('SATURDAY, 21 MARCH', 8.7, accent, tracking=0.67, shadow=shadow)
    cx, cy = 26, 22
    if anchor == 'bottomLeft': cy = 230 - 22 - ch
    if anchor == 'topRight': cx, cy = 448 - 26 - cw, 37
    dx, dy = cx, cy - dh - 5 if above else cy + ch + 4
    if name == 'Hollow':
        cx = (448 + dw + 12 - cw) / 2
        cy = 230 - 15 - ch
        dx, dy = cx - 12 - dw, cy + (ch-dh)/2
    out.append(f'<section><div class="label">Pixel {name}</div><div class="scene" style="background:{bg}">')
    out.append(f'<div class="clock" style="left:{cx}px;top:{cy}px">{clock}</div>')
    out.append(f'<div class="date" style="left:{dx}px;top:{dy}px">{date}</div></div></section>')
out.append('</div><div class="footer">Preview uses flat backgrounds. No Qylock artwork or credential UI is included.</div>')
(ROOT / 'build/pixel-themes-preview.html').write_text(''.join(out), encoding='utf-8')
print('Saved build/pixel-themes-preview.html')
