# Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
# Renders the Xbox app's tile and splash images from Ymir's own logo (apps/ymir-sdl3/res/ymir.png).
# The PNGs are committed under xbox/Assets; rerun only when the logo or sizes change. Needs Pillow.
import os
from PIL import Image

here = os.path.dirname(os.path.abspath(__file__))
logo_src = Image.open(os.path.join(here, '..', 'apps', 'ymir-sdl3', 'res', 'ymir.png')).convert('RGBA')
background = (0, 0, 0, 255)  # matches BackgroundColor in AppxManifest.xml

def logo(size):
    return logo_src.resize((size, size), Image.LANCZOS)

# name: (width, height, logo size); the bigger tiles keep the logo on the tile background colour
for name, (w, h, s) in {'StoreLogo': (50, 50, 50), 'Square44x44Logo': (44, 44, 44),
                        'Square150x150Logo': (150, 150, 112), 'Wide310x150Logo': (310, 150, 112),
                        'SplashScreen': (620, 300, 220)}.items():
    img = Image.new('RGBA', (w, h), background if name in ('Wide310x150Logo', 'SplashScreen', 'Square150x150Logo') else (0, 0, 0, 0))
    img.alpha_composite(logo(s), ((w - s) // 2, (h - s) // 2))
    img.save(os.path.join(here, 'Assets', name + '.png'), optimize=True)
    print(name, w, h)
