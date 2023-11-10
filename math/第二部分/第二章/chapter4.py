import matplotlib.pyplot as plt
from math import sqrt
import math
from random import uniform

# 实现多个向量相加
def addVecs(vec):
    return [sum(x[0] for x in vec), sum(y[1] for y in vec)]

print(addVecs([(1,2),(2,4),(3,6),(4,8)]))

# 平移
def translate(tran, vecs):
    return [(tran[0] + v[0], tran[1] + v[1]) for v in vecs]

print(translate([1,1], [(-1, -1), (2, -1)]))

# 多个恐龙
# 定义一个表示点的类
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

# 定义恐龙形状的坐标点
points = [Point(6,4), Point(3,1), Point(1,2), Point(-1,5), Point(-2,5), Point(-3,4), Point(-4,4), 
 Point(-5,3), Point(-5,2), Point(-2,2), Point(-5,1), Point(-4,0), Point(-2,1), Point(-1,0), Point(0,-3),  
 Point(-1,-4), Point(1,-4), Point(2,-3), Point(1,-2), Point(3,-1), Point(5,1), Point(6,4)]

# 偏移
dev = [-1.5, -2.5]

# 使用plot函数将这些点连接起来
for i in range(-10, 10):
    # plt.plot([point.x + i for point in points], [point.y + i for point in points], color='blue')
    pass

# plt.show()

# 绘制scale
# plt.plot([0, sqrt(2)], [0, sqrt(3)], color='blue', linewidth=5)
# plt.plot([0, math.pi * sqrt(2)] , [0, math.pi * sqrt(3)], color='red')

# plt.show()

# 可能出现的位置
u = (-1, 1)
v = (1, 1)
def random_r():
 return uniform(-3,3)
def random_s(): 
 return uniform(-1,1)

