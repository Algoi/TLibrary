import matplotlib.pyplot as plt
from math import sqrt

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
plt.plot([point.x for point in points], [point.y for point in points], color='red')
plt.plot([point.x + dev[0] for point in points], [point.y + dev[1] for point in points], color='blue')

# def len(v):
#     return sqrt(v[0]**2 + v[1]**2)

# print(len((8,4)))

# 显示图形
plt.show()
