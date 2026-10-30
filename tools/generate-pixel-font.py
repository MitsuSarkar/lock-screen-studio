"""Embed OFL-licensed Pixelify Sans glyph outlines for font-independent XAML."""
from pathlib import Path
import json
import sys

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools/python-packages'))
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont
from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.pens.transformPen import TransformPen

font = instantiateVariableFont(TTFont(ROOT / 'vendor/PixelifySans.ttf'), {'wght': 400})
glyphs = font.getGlyphSet()
cmap = font.getBestCmap()
units = font['head'].unitsPerEm
ascender = font['hhea'].ascent
height = ascender - font['hhea'].descent
records = []
for code in sorted(cmap):
    # Include every mapped character, so localized dates can use this font too.
    if code < 32 or code > 0xFFFF:
        continue
    name = cmap[code]
    pen = SVGPathPen(glyphs)
    glyphs[name].draw(TransformPen(pen, (1, 0, 0, -1, 0, ascender)))
    records.append((code, font['hmtx'][name][0], pen.getCommands()))
license_text = (ROOT / 'vendor/PixelifySans-OFL.txt').read_text(encoding='utf-8')
out = ['// Font outlines: Pixelify Sans Regular; licensed under SIL OFL 1.1.',
       '/*\n' + license_text + '\n*/',
       f'constexpr double kStudioPixelEm = {units};',
       f'constexpr double kStudioPixelHeight = {height};',
       'struct StudioPixelGlyph { wchar_t character; int advance; const wchar_t* data; };',
       'static const StudioPixelGlyph kStudioPixelGlyphs[] = {']
out += [f'    {{static_cast<wchar_t>({code}), {advance}, L"{path}"}},' for code, advance, path in records]
out += ['};', '']
(ROOT / 'src/pixel-font.inc').write_text('\n'.join(out), encoding='utf-8')
# Geometry data is also used by the independent visual QA renderer.
(ROOT / 'reference/pixel-glyphs.json').write_text(json.dumps({
    'em': units, 'height': height,
    'glyphs': {str(code): {'advance': advance, 'path': path} for code, advance, path in records}
}), encoding='utf-8')
print(f'Generated {len(records)} glyphs; em={units}, line height={height}')
