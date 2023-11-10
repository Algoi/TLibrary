import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

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

p = [0, 0, 0]
v = [-1, -3, 3]
w = [3, 2, 4]

ax.plot([p[0], v[0]], [p[1], v[1]], [p[2], v[2]], color='red', linewidth=3, label='Line Segment1')
ax.plot([p[0], w[0]], [p[1], w[1]], [p[2], w[2]], color='green', linewidth=3, label='Line Segment1')

r = subVector(v, w)
ax.plot([p[0], r[0]], [p[1], r[1]], [p[2], r[2]], color='yellow', linewidth=3, label='Line Segment1')

print(r)

# 设置坐标轴标签
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# 显示图形
plt.show()

