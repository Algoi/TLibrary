import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from math import sqrt, cos

# 创建一个3D坐标轴
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 用于向量相加，接收一条线段和增量
def addVector(origin, increament):
    return [p1 + p2 for p1, p2 in zip(origin, increament)]

# 向量乘法
def multiVector(origin, u):
    return [x*u for x in origin]

# 向量减法
def subVector(origin, decreament):
    return [p1 - p2 for p1, p2 in zip(origin, decreament)]

# 三维向量长度
def length(origin):
    result = 0
    for x in origin:
        result += (x ** 2)
    return sqrt(result)

# 点积。两个向量同向为正，反向为负。垂直为0
# 向量越长，其点积的绝对值越大
def dot(v1, v2):
    return sum([x1 * x2 for x1, x2 in zip(v1, v2)])

p = [0, 0, 0]
v = [-1, -3, 3]
w = [-1, -1, 2]

print(dot((1, 0), (0, 1)))
print(dot((3, 4), (2, 3)))

print(length(3, 4) * length(2, 3) * cos())

# ax.plot([p[0], v[0]], [p[1], v[1]], [p[2], v[2]], color='red', linewidth=3, label='Line Segment')


# 设置坐标轴标签
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# 显示图形
plt.show()

