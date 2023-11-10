import matplotlib.pyplot as plt
import math
from math import asin, acos

def toAngle(pol):
    len, pa = pol[0], pol[1]
    return (len * math.cos(pa), len * math.sin(pa))

print(toAngle((5, 37 * math.pi / 180)))

print(asin(3 / math.sqrt(13)) * 180 / math.pi)

print(math.sqrt((-1.34)**2+ (2.68)**2) )

print(math.tan(22 * math.pi / 180))

# plt.plot([0, 8.5 * (-0.574)], [0, 8.5 * 0.819])
# plt.show()

# 极坐标转笛卡尔坐标
def to_cartesian(polar_vector):
    length, angle = polar_vector[0], polar_vector[1]
    return (length * math.cos(angle), length * math.sin(angle))

locs = [(math.cos(5 * x * math.pi / 500.0), 2 * math.pi * x / 1000.0) for x in range(0,1000)]
# 转笛卡尔坐标
locs_d = [(to_cartesian(p)) for p in locs]

plt.plot([x[0] for x in locs_d], [y[1] for y in locs_d])
plt.show()

# 笛卡尔坐标转极坐标
def to_polar(cart_vector):
    x, y = cart_vector[0], cart_vector[1]
    pol = math.atan2(y, x)
    len = math.sqrt(x ** 2 + y ** 2)
    return (len, pol)

print(to_polar([1, 1]))
print(math.sqrt(2), "\t", math.pi / 4)


# 求两点距离
def getLen(loc1, loc2):
    return math.sqrt((loc1[0] - loc2[0]) ** 2 + (loc1[1] - loc2[1]) ** 2)

A = [-2, 2]
B = [-5, 2]
C = [-5, 1]
a = getLen(B, C)
b = getLen(A, C)
c = getLen(A, B)

cosa = (b ** 2 + c ** 2 - a ** 2) / (2 * b * c)
cosb = (c ** 2 + a ** 2 - b ** 2) / (2 * a * c)
result1 = math.acos(cosa)
result2 = math.acos(cosb)
print("all results = ", cosa, "\t", result1 * 180 / math.pi, "\t", result2 * 180 / math.pi)