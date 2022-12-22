import sys
from PIL import Image

with Image.open("logo_template.png") as template:
    logo = template.convert("RGB")
    ice4 = sys.stdin.buffer.read()
    ice4 = ice4.replace(b"#!", b"#!" + b" " * (4096 - len(ice4)), 1)
    for y in range(logo.height):
        for x in range(logo.width):
            r, g, b = logo.getpixel((x, y))
            byte = ice4[(y * logo.width + x) // 2]
            nibble = byte >> 4 if x % 2 == 0 else byte & 0b1111
            b = (nibble << 4) | (b & 0b1111)
            logo.putpixel((x, y), (r, g, b))
    logo.save("logo.png")
