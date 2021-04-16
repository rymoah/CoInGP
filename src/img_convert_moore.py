from PIL import Image
import random
import math
import sys

N_PIXELS = 100
IMAGE = sys.argv[1]


def window(img, x, y, radius):
    inputs = []
    outputs = None
    for i in range(x - radius, x + radius + 1):
        for j in range(y - radius, y + radius + 1):
            if i != x or j != y:
                inputs.append(img.getpixel((i, j))[0])
            else:
                outputs = img.getpixel((i, j))[0]
    return inputs, outputs


def slide_window(img, radius, test):
    cases = []
    for x in range(radius, img.width - radius):
        for y in range(radius, img.height - radius):
            overlap = [(i, j) for i in range(x-radius, x+radius+1)
                       for j in range(y-radius, y+radius+1) if (i, j) in tests]
            if overlap == []:
                cases.append(window(img, x, y, radius))
    return cases


def make_test(img, radius):
    tests = []
    random.seed(a=0)
    for x in range(radius, img.width - radius, radius + 1):
        ys = random.sample(range(radius, img.height - radius, radius + 1),
                           N_PIXELS)
        for y in ys:
            tests.append((x, y))
    return tests


im = Image.open(IMAGE).convert('LA')
im2 = Image.open(IMAGE).convert('LA')

radius = 1
tests = make_test(im, radius)
for point in tests:
    im.putpixel(point, (0, 255))
im.save('greyscale.png')

train = slide_window(im, 1, tests)

f = open("train.txt", "w")
for case in train:
    for i in case[0]:
        f.write(f"{i} ")
    f.write(f"{case[1]}\n")
f.close()

f = open("test.txt", "w")
test_values = []
for x, y in tests:
    a, b = window(im2, x, y, radius)
    for i in a:
        f.write(f"{i} ")
    f.write(f"{b}\n")
f.close()

f = open("points.txt", "w")
for x, y in tests:
    f.write(f"{x} {y}\n")
f.close()

f = open("baselines.txt", "w")
error = 0
for case in train:
    prediction = sum(case[0])//len(case[0])
    error += (prediction - case[1])**2
rmse =  math.sqrt(error/len(train))
f.write(f"{rmse}\n")
error = 0
for x, y in tests:
    a, b = window(im2, x, y, radius)
    prediction = sum(a)//len(a)
    error += (prediction - b)**2
    im.putpixel((x, y), (prediction, 255))
rmse = math.sqrt(error/len(tests))
f.write(f"{rmse}\n")
f.close()
im.save("baseline.png")
