import math
import matplotlib.pyplot as plt

class Transformations:
    def __init__(self):
        pass

    # 移动，横坐标和纵坐标各自偏移指定数量
    @staticmethod
    def to_move(v, devx, devy):
        return [v[0] + devx, v[1] + devy]

    @staticmethod
    def to_moves(vls, devx, devy):
        return [Transformations.to_move(v, devx, devy) for v in vls]

    # 缩放，将向量与某个数相乘可以实现缩放，如果是个负数，会往反方向缩放
    @staticmethod
    def to_scale(v, u):
        return [v[0] * u, v[1] * u]

    @staticmethod
    def to_scales(vls, u):
        return [Transformations.to_scale(p, u) for p in vls]

    # 笛卡尔坐标转极坐标
    @staticmethod
    def to_polar(v):
        x, y = v[0], v[1]
        len = math.sqrt(x ** 2 + y ** 2)
        polar = math.atan2(y, x)
        return [len, polar]

    @staticmethod
    def to_polars(vls):
        return [Transformations.to_polar(point) for point in vls]

    # 极坐标转笛卡尔坐标
    @staticmethod
    def to_cartesian(pol):
        len, polar = pol[0], pol[1]
        return [len * math.cos(polar), len * math.sin(polar)]

    def to_cartesians(pols):
        return [Transformations.to_cartesian(point) for point in pols]

    # 旋转，参数的是一个弧度，然后一个旋转的角度
    @staticmethod
    def to_rotate(pol, angle):
        return [pol[0], pol[1] + angle * math.pi / 180]

    @staticmethod
    def to_rotates(pols, angle):
        return [Transformations.to_rotate(point, angle) for point in pols]

    # 旋转一定角度后平移一定距离
    def to_dev_rotate(self, v, dev, angle):
        pol_v = self.to_polar(v)
        rotateLP = self.to_rotate(pol_v, angle)
        rotateLD = self.to_cartesian(rotateLP)
        result = self.to_move(rotateLD, dev[0], dev[1]) # 平移
        return result

    def to_dev_rotates(self, vls, dev, angle):
        return [self.to_dev_rotate(v, dev, angle) for v in vls]
    
    # 求两点距离
    @staticmethod
    def getLen(loc1, loc2):
        return math.sqrt((loc1[0] - loc2[0]) ** 2 + (loc1[1] - loc2[1]) ** 2)

# 求三边长已知的情况下的某个夹角
A = [-2, 2]
B = [-5, 2]
C = [-5, 1]
a = Transformations.getLen(B, C)
b = Transformations.getLen(A, C)
c = Transformations.getLen(A, B)

cosa = (b ** 2 + c ** 2 - a ** 2) / (2 * b * c)
cosb = (c ** 2 + a ** 2 - b ** 2) / (2 * a * c)
result1 = math.acos(cosa)
result2 = math.acos(cosb)
print("all results = ", cosa, "\t", result1 * 180 / math.pi, "\t", result2 * 180 / math.pi)
