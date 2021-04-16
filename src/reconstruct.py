from PIL import Image
import csv
import sys

im_origin = Image.open(sys.argv[1]).convert('LA')
im = Image.open(sys.argv[1]).convert('LA')

points = []
pixels = []

with open('points.txt') as pointfile:
    with open('out.txt') as valuefile:
        point_csv = csv.reader(pointfile, delimiter=' ')
        value_csv = csv.reader(valuefile, delimiter=' ')
        for (p, v) in zip(point_csv, value_csv):
            p = (int(p[0]), int(p[1]))
            v = int(v[0])
            im.putpixel(p, (v, 255))
im.save('corrected.png')

for i in range(0, im.width):
    for j in range(0, im.height):
        a = im_origin.getpixel((i, j))
        b = im.getpixel((i, j))
        im_origin.putpixel((i, j), (min(255, abs(a[0] - b[0])*10), 255))
im_origin.save("difference.png")
